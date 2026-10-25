/**
 *    Name:        ecdsa.c
 *    Description: HMAC-SHA256 and ECDSA on P-256 with SHA-256 for the APK tool: the curve's arithmetic,
 *                 signing with RFC 6979's nonce, verifying
 *    Version:     0.1.0
 *    Created:     25-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Plain arithmetic (the tool signs one APK at a time, on the player's computer): numbers of eight 32-bit
 *    words, multiplied with Montgomery's method modulo the field's prime p and modulo the curve's order n
 *    (both odd, so one routine serves both); inverses as Fermat's power m - 2. Points in Jacobian
 *    coordinates, doubled with the formulas for a = -3 (P-256's), multiplied bit by bit from the top. The
 *    nonce is RFC 6979's: the same key and hash always give the same signature, and nothing random is
 *    needed to sign. Nothing here hides its timing: it runs once on the player's own computer.
 */
#include "ecdsa.h"
#include "crypto.h"

#include <string.h>

#define WORDS 8

typedef uint32_t number[WORDS];    /* below 2^256, its words least significant first */

/* A modulus for Montgomery's multiplication (R = 2^256) */
typedef struct
{
	number m;
	uint32_t inverse;    /* -m^-1 mod 2^32 */
	number r2;           /* R^2 mod m: a number times it is the number multiplied by R */
	number one;          /* R mod m: 1 multiplied by R */
} modulus;

/* A point in Jacobian coordinates (x / z^2, y / z^3), each multiplied by R mod p; z 0 the point at infinity */
typedef struct
{
	number x, y, z;
} point;

typedef struct
{
	modulus p, n;
	number b;    /* the curve's b, multiplied by R */
	point g;     /* its base point */
} curve;

/* P-256 (FIPS-186-4, D.1.2.3): y^2 = x^3 - 3x + b over the prime p, the base point G of order n */
static const uint8_t curve_p[32] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
static const uint8_t curve_n[32] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84, 0xF3, 0xB9, 0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51,
};
static const uint8_t curve_b[32] = {
	0x5A, 0xC6, 0x35, 0xD8, 0xAA, 0x3A, 0x93, 0xE7, 0xB3, 0xEB, 0xBD, 0x55, 0x76, 0x98, 0x86, 0xBC,
	0x65, 0x1D, 0x06, 0xB0, 0xCC, 0x53, 0xB0, 0xF6, 0x3B, 0xCE, 0x3C, 0x3E, 0x27, 0xD2, 0x60, 0x4B,
};
static const uint8_t curve_gx[32] = {
	0x6B, 0x17, 0xD1, 0xF2, 0xE1, 0x2C, 0x42, 0x47, 0xF8, 0xBC, 0xE6, 0xE5, 0x63, 0xA4, 0x40, 0xF2,
	0x77, 0x03, 0x7D, 0x81, 0x2D, 0xEB, 0x33, 0xA0, 0xF4, 0xA1, 0x39, 0x45, 0xD8, 0x98, 0xC2, 0x96,
};
static const uint8_t curve_gy[32] = {
	0x4F, 0xE3, 0x42, 0xE2, 0xFE, 0x1A, 0x7F, 0x9B, 0x8E, 0xE7, 0xEB, 0x4A, 0x7C, 0x0F, 0x9E, 0x16,
	0x2B, 0xCE, 0x33, 0x57, 0x6B, 0x31, 0x5E, 0xCE, 0xCB, 0xB6, 0x40, 0x68, 0x37, 0xBF, 0x51, 0xF5,
};

/* ---- HMAC-SHA256 ---- */

void hmac_sha256(const uint8_t *key, size_t key_size, const void *data, size_t size, uint8_t out[32])
{
	uint8_t block[64], inner[32];
	sha256 hash;

	/* (the key first, then the data: 'out' may be either) */
	memset(block, 0, sizeof(block));
	if (key_size > sizeof(block))
		sha256_of(key, key_size, block);
	else
		memcpy(block, key, key_size);
	for (int byte = 0; byte < 64; byte++)
		block[byte] ^= 0x36;
	sha256_init(&hash);
	sha256_update(&hash, block, sizeof(block));
	sha256_update(&hash, data, size);
	sha256_final(&hash, inner);
	for (int byte = 0; byte < 64; byte++)
		block[byte] ^= 0x36 ^ 0x5C;
	sha256_init(&hash);
	sha256_update(&hash, block, sizeof(block));
	sha256_update(&hash, inner, sizeof(inner));
	sha256_final(&hash, out);
	memset(block, 0, sizeof(block));
	memset(inner, 0, sizeof(inner));
}

/* ---- numbers ---- */

static void from_bytes(number out, const uint8_t bytes[32])
{
	for (int word = 0; word < WORDS; word++)
		out[word] = (uint32_t)bytes[31 - word * 4] | (uint32_t)bytes[30 - word * 4] << 8
			| (uint32_t)bytes[29 - word * 4] << 16 | (uint32_t)bytes[28 - word * 4] << 24;
}

static void to_bytes(const number in, uint8_t bytes[32])
{
	for (int word = 0; word < WORDS; word++)
		for (int byte = 0; byte < 4; byte++)
			bytes[31 - word * 4 - byte] = (uint8_t)(in[word] >> (8 * byte));
}

static int compare(const number a, const number b)
{
	for (int word = WORDS - 1; word >= 0; word--)
		if (a[word] != b[word])
			return a[word] < b[word] ? -1 : 1;
	return 0;
}

static int is_zero(const number a)
{
	uint32_t all = 0;

	for (int word = 0; word < WORDS; word++)
		all |= a[word];
	return all == 0;
}

/* a + b, its carry out */
static uint32_t add(number out, const number a, const number b)
{
	uint64_t carry = 0;

	for (int word = 0; word < WORDS; word++)
	{
		carry += (uint64_t)a[word] + b[word];
		out[word] = (uint32_t)carry;
		carry >>= 32;
	}
	return (uint32_t)carry;
}

/* a - b, its borrow out */
static uint32_t subtract(number out, const number a, const number b)
{
	uint64_t borrow = 0;

	for (int word = 0; word < WORDS; word++)
	{
		const uint64_t difference = (uint64_t)a[word] - b[word] - borrow;

		out[word] = (uint32_t)difference;
		borrow = (difference >> 32) & 1u;
	}
	return (uint32_t)borrow;
}

/* ---- modulo m ---- */

/* a + b mod m, both below m */
static void add_mod(const modulus *mod, number out, const number a, const number b)
{
	if (add(out, a, b) || compare(out, mod->m) >= 0)
		subtract(out, out, mod->m);
}

/* a - b mod m, both below m */
static void subtract_mod(const modulus *mod, number out, const number a, const number b)
{
	if (subtract(out, a, b))
		add(out, out, mod->m);
}

/* A number below 2m, below m */
static void reduce(const modulus *mod, number a)
{
	if (compare(a, mod->m) >= 0)
		subtract(a, a, mod->m);
}

/* a b / R mod m (Montgomery's, word by word), both below m */
static void multiply_mod(const modulus *mod, number out, const number a, const number b)
{
	uint32_t sum[WORDS + 2] = { 0 };

	for (int i = 0; i < WORDS; i++)
	{
		uint64_t carry = 0, total;
		uint32_t factor;

		/* sum += a b[i] */
		for (int j = 0; j < WORDS; j++)
		{
			total = (uint64_t)sum[j] + (uint64_t)a[j] * b[i] + carry;
			sum[j] = (uint32_t)total;
			carry = total >> 32;
		}
		total = (uint64_t)sum[WORDS] + carry;
		sum[WORDS] = (uint32_t)total;
		sum[WORDS + 1] = (uint32_t)(total >> 32);
		/* sum += factor m, which ends its lowest word in 0, and sum / 2^32 */
		factor = sum[0] * mod->inverse;
		total = (uint64_t)sum[0] + (uint64_t)factor * mod->m[0];
		carry = total >> 32;
		for (int j = 1; j < WORDS; j++)
		{
			total = (uint64_t)sum[j] + (uint64_t)factor * mod->m[j] + carry;
			sum[j - 1] = (uint32_t)total;
			carry = total >> 32;
		}
		total = (uint64_t)sum[WORDS] + carry;
		sum[WORDS - 1] = (uint32_t)total;
		sum[WORDS] = sum[WORDS + 1] + (uint32_t)(total >> 32);
	}
	/* (below 2m) */
	if (sum[WORDS] || compare(sum, mod->m) >= 0)
		subtract(sum, sum, mod->m);
	memcpy(out, sum, sizeof(number));
}

static void modulus_init(modulus *mod, const uint8_t bytes[32])
{
	number value = { 1 };
	uint32_t inverse = 1;

	from_bytes(mod->m, bytes);
	/* Newton's iteration: the bits of m's inverse mod 2^32 double each round */
	for (int round = 0; round < 5; round++)
		inverse *= 2u - mod->m[0] * inverse;
	mod->inverse = 0u - inverse;
	/* R and R^2 mod m: 1 doubled 256 and 512 times */
	for (int bit = 1; bit <= 512; bit++)
	{
		add_mod(mod, value, value, value);
		if (bit == 256)
			memcpy(mod->one, value, sizeof(number));
	}
	memcpy(mod->r2, value, sizeof(number));
}

/* A number below m multiplied by R, and back */
static void into_montgomery(const modulus *mod, number out, const number a)
{
	multiply_mod(mod, out, a, mod->r2);
}

static void from_montgomery(const modulus *mod, number out, const number a)
{
	const number one = { 1 };

	multiply_mod(mod, out, a, one);
}

/* 1 / a mod m (a multiplied by R, and so the result): a^(m - 2), m prime */
static void invert_mod(const modulus *mod, number out, const number a)
{
	number exponent, result;

	memcpy(exponent, mod->m, sizeof(number));
	exponent[0] -= 2;    /* (p's and n's lowest words are above 2) */
	memcpy(result, mod->one, sizeof(number));
	for (int bit = 255; bit >= 0; bit--)
	{
		multiply_mod(mod, result, result, result);
		if (exponent[bit / 32] >> (bit % 32) & 1u)
			multiply_mod(mod, result, result, a);
	}
	memcpy(out, result, sizeof(number));
}

/* ---- the curve ---- */

static void curve_init(curve *c)
{
	number value;

	modulus_init(&c->p, curve_p);
	modulus_init(&c->n, curve_n);
	from_bytes(value, curve_b);
	into_montgomery(&c->p, c->b, value);
	from_bytes(value, curve_gx);
	into_montgomery(&c->p, c->g.x, value);
	from_bytes(value, curve_gy);
	into_montgomery(&c->p, c->g.y, value);
	memcpy(c->g.z, c->p.one, sizeof(number));
}

/* 2P: delta = z^2, gamma = y^2, beta = x gamma, alpha = 3 (x - delta) (x + delta); x' = alpha^2 - 8 beta,
   z' = (y + z)^2 - gamma - delta, y' = alpha (4 beta - x') - 8 gamma^2 */
static void double_point(const curve *c, point *out, const point *in)
{
	const modulus *const p = &c->p;
	number delta, gamma, beta, alpha, four_beta, x, y, z, t;

	if (is_zero(in->z))
	{
		*out = *in;
		return;
	}
	multiply_mod(p, delta, in->z, in->z);
	multiply_mod(p, gamma, in->y, in->y);
	multiply_mod(p, beta, in->x, gamma);
	subtract_mod(p, t, in->x, delta);
	add_mod(p, alpha, in->x, delta);
	multiply_mod(p, t, t, alpha);
	add_mod(p, alpha, t, t);
	add_mod(p, alpha, alpha, t);
	add_mod(p, z, in->y, in->z);
	multiply_mod(p, z, z, z);
	subtract_mod(p, z, z, gamma);
	subtract_mod(p, z, z, delta);
	add_mod(p, four_beta, beta, beta);
	add_mod(p, four_beta, four_beta, four_beta);
	multiply_mod(p, x, alpha, alpha);
	subtract_mod(p, x, x, four_beta);
	subtract_mod(p, x, x, four_beta);
	subtract_mod(p, y, four_beta, x);
	multiply_mod(p, y, alpha, y);
	multiply_mod(p, t, gamma, gamma);
	add_mod(p, t, t, t);
	add_mod(p, t, t, t);
	add_mod(p, t, t, t);
	subtract_mod(p, y, y, t);
	memcpy(out->x, x, sizeof(number));
	memcpy(out->y, y, sizeof(number));
	memcpy(out->z, z, sizeof(number));
}

/* P + Q: u1 = x1 z2^2, u2 = x2 z1^2, s1 = y1 z2^3, s2 = y2 z1^3, h = u2 - u1, r = s2 - s1;
   x' = r^2 - h^3 - 2 u1 h^2, y' = r (u1 h^2 - x') - s1 h^3, z' = z1 z2 h */
static void add_points(const curve *c, point *out, const point *a, const point *b)
{
	const modulus *const p = &c->p;
	number z1z1, z2z2, u1, u2, s1, s2, h, r, hh, hhh, v, x, y, z;

	if (is_zero(a->z))
	{
		*out = *b;
		return;
	}
	if (is_zero(b->z))
	{
		*out = *a;
		return;
	}
	multiply_mod(p, z1z1, a->z, a->z);
	multiply_mod(p, z2z2, b->z, b->z);
	multiply_mod(p, u1, a->x, z2z2);
	multiply_mod(p, u2, b->x, z1z1);
	multiply_mod(p, s1, a->y, b->z);
	multiply_mod(p, s1, s1, z2z2);
	multiply_mod(p, s2, b->y, a->z);
	multiply_mod(p, s2, s2, z1z1);
	if (compare(u1, u2) == 0)
	{
		/* (the same x: the same point, doubled, or its opposite, which leaves the point at infinity) */
		if (compare(s1, s2) == 0)
			double_point(c, out, a);
		else
			memset(out, 0, sizeof(*out));
		return;
	}
	subtract_mod(p, h, u2, u1);
	subtract_mod(p, r, s2, s1);
	multiply_mod(p, hh, h, h);
	multiply_mod(p, hhh, h, hh);
	multiply_mod(p, v, u1, hh);
	multiply_mod(p, x, r, r);
	subtract_mod(p, x, x, hhh);
	subtract_mod(p, x, x, v);
	subtract_mod(p, x, x, v);
	subtract_mod(p, y, v, x);
	multiply_mod(p, y, r, y);
	multiply_mod(p, s1, s1, hhh);
	subtract_mod(p, y, y, s1);
	multiply_mod(p, z, a->z, b->z);
	multiply_mod(p, z, z, h);
	memcpy(out->x, x, sizeof(number));
	memcpy(out->y, y, sizeof(number));
	memcpy(out->z, z, sizeof(number));
}

/* k P, k a plain number: doubled and added bit by bit from the top */
static void multiply_point(const curve *c, point *out, const number k, const point *in)
{
	point result;

	memset(&result, 0, sizeof(result));
	for (int bit = 255; bit >= 0; bit--)
	{
		double_point(c, &result, &result);
		if (k[bit / 32] >> (bit % 32) & 1u)
			add_points(c, &result, &result, in);
	}
	*out = result;
}

/* The point's plain x and y: false for the point at infinity */
static int to_affine(const curve *c, const point *in, number x, number y)
{
	number inverse, inverse2;

	if (is_zero(in->z))
		return 0;
	invert_mod(&c->p, inverse, in->z);
	multiply_mod(&c->p, inverse2, inverse, inverse);
	multiply_mod(&c->p, x, in->x, inverse2);
	multiply_mod(&c->p, inverse2, inverse2, inverse);
	multiply_mod(&c->p, y, in->y, inverse2);
	from_montgomery(&c->p, x, x);
	from_montgomery(&c->p, y, y);
	return 1;
}

/* A public key's point (plain x and y below p) when it is on the curve */
static int point_from_public_key(const curve *c, const uint8_t public_key[64], point *out)
{
	number x, y, left, right;

	from_bytes(x, public_key);
	from_bytes(y, public_key + 32);
	if (compare(x, c->p.m) >= 0 || compare(y, c->p.m) >= 0)
		return 0;
	into_montgomery(&c->p, out->x, x);
	into_montgomery(&c->p, out->y, y);
	memcpy(out->z, c->p.one, sizeof(number));
	/* y^2 = x^3 - 3x + b */
	multiply_mod(&c->p, left, out->y, out->y);
	multiply_mod(&c->p, right, out->x, out->x);
	multiply_mod(&c->p, right, right, out->x);
	for (int times = 0; times < 3; times++)
		subtract_mod(&c->p, right, right, out->x);
	add_mod(&c->p, right, right, c->b);
	return compare(left, right) == 0;
}

/* ---- ECDSA ---- */

int ecdsa_key_valid(const uint8_t key[32])
{
	number d, n;

	from_bytes(d, key);
	from_bytes(n, curve_n);
	return !is_zero(d) && compare(d, n) < 0;
}

void ecdsa_public_key(const uint8_t key[32], uint8_t public_key[64])
{
	curve c;
	point q;
	number d, x, y;

	curve_init(&c);
	from_bytes(d, key);
	multiply_point(&c, &q, d, &c.g);
	to_affine(&c, &q, x, y);
	to_bytes(x, public_key);
	to_bytes(y, public_key + 32);
	memset(d, 0, sizeof(d));
}

/* RFC 6979, 3.2: k from HMAC-SHA256 keyed by the private key and the hash, the next one each time one
   cannot be used; r = (k G).x mod n, s = (e + r d) / k mod n */
void ecdsa_sign(const uint8_t key[32], const uint8_t hash[32], uint8_t r_out[32], uint8_t s_out[32])
{
	curve c;
	point kg;
	number d, e, k, r, s, x, y, kr, rr, dr, er;
	uint8_t v[32], hmac_key[32], seed[32 + 1 + 32 + 32];

	curve_init(&c);
	from_bytes(d, key);
	from_bytes(e, hash);
	reduce(&c.n, e);
	/* V = 01..., K = 00...; K = HMAC_K(V || 00 || x || h), V = HMAC_K(V); the same with 01 */
	memset(v, 0x01, sizeof(v));
	memset(hmac_key, 0x00, sizeof(hmac_key));
	memcpy(seed + 33, key, 32);
	to_bytes(e, seed + 65);
	for (uint8_t round = 0; round < 2; round++)
	{
		memcpy(seed, v, 32);
		seed[32] = round;
		hmac_sha256(hmac_key, 32, seed, sizeof(seed), hmac_key);
		hmac_sha256(hmac_key, 32, v, 32, v);
	}
	for (;;)
	{
		hmac_sha256(hmac_key, 32, v, 32, v);
		from_bytes(k, v);
		if (!is_zero(k) && compare(k, c.n.m) < 0)
		{
			multiply_point(&c, &kg, k, &c.g);
			to_affine(&c, &kg, x, y);
			memcpy(r, x, sizeof(number));
			reduce(&c.n, r);
			if (!is_zero(r))
			{
				into_montgomery(&c.n, kr, k);
				invert_mod(&c.n, kr, kr);
				into_montgomery(&c.n, rr, r);
				into_montgomery(&c.n, dr, d);
				into_montgomery(&c.n, er, e);
				multiply_mod(&c.n, rr, rr, dr);
				add_mod(&c.n, er, er, rr);
				multiply_mod(&c.n, s, kr, er);
				from_montgomery(&c.n, s, s);
				if (!is_zero(s))
					break;
			}
		}
		/* K = HMAC_K(V || 00), V = HMAC_K(V) */
		memcpy(seed, v, 32);
		seed[32] = 0x00;
		hmac_sha256(hmac_key, 32, seed, 33, hmac_key);
		hmac_sha256(hmac_key, 32, v, 32, v);
	}
	to_bytes(r, r_out);
	to_bytes(s, s_out);
	memset(d, 0, sizeof(d));
	memset(k, 0, sizeof(k));
	memset(kr, 0, sizeof(kr));
	memset(dr, 0, sizeof(dr));
	memset(v, 0, sizeof(v));
	memset(hmac_key, 0, sizeof(hmac_key));
	memset(seed, 0, sizeof(seed));
}

/* (e / s) G + (r / s) Q, whose x mod n is r */
int ecdsa_verify(const uint8_t public_key[64], const uint8_t hash[32], const uint8_t r_in[32], const uint8_t s_in[32])
{
	curve c;
	point q, u1g, u2q;
	number r, s, e, w, u1, u2, x, y;

	curve_init(&c);
	from_bytes(r, r_in);
	from_bytes(s, s_in);
	if (is_zero(r) || is_zero(s) || compare(r, c.n.m) >= 0 || compare(s, c.n.m) >= 0
		|| !point_from_public_key(&c, public_key, &q))
		return 0;
	from_bytes(e, hash);
	reduce(&c.n, e);
	into_montgomery(&c.n, w, s);
	invert_mod(&c.n, w, w);
	multiply_mod(&c.n, u1, e, w);    /* (a plain number times one multiplied by R: plain) */
	multiply_mod(&c.n, u2, r, w);
	multiply_point(&c, &u1g, u1, &c.g);
	multiply_point(&c, &u2q, u2, &q);
	add_points(&c, &u1g, &u1g, &u2q);
	if (!to_affine(&c, &u1g, x, y))
		return 0;
	reduce(&c.n, x);
	return compare(x, r) == 0;
}

/* An INTEGER of DER: the fewest bytes, a 0 first when its top bit is set (it is positive) */
static size_t der_integer(const uint8_t value[32], uint8_t *out)
{
	size_t start = 0, length;
	int pad;

	while (start < 31 && value[start] == 0)
		start++;
	pad = (value[start] & 0x80) != 0;
	length = 32 - start + (size_t)pad;
	out[0] = 0x02;
	out[1] = (uint8_t)length;
	out[2] = 0;
	memcpy(out + 2 + pad, value + start, 32 - start);
	return 2 + length;
}

size_t ecdsa_der(const uint8_t r[32], const uint8_t s[32], uint8_t der[ECDSA_SIGNATURE_SIZE])
{
	size_t size = der_integer(r, der + 2);

	size += der_integer(s, der + 2 + size);
	der[0] = 0x30;
	der[1] = (uint8_t)size;
	return 2 + size;
}

/* ---- the standards' vectors ---- */

static int hex_is(const uint8_t *bytes, const char *hex)
{
	for (size_t at = 0; hex[at * 2]; at++)
	{
		unsigned value = 0;

		for (int digit = 0; digit < 2; digit++)
		{
			const char c = hex[at * 2 + digit];

			value = value * 16 + (unsigned)(c <= '9' ? c - '0' : (c | 0x20) - 'a' + 10);
		}
		if (bytes[at] != value)
			return 0;
	}
	return 1;
}

const char *ecdsa_self_test(void)
{
	/* RFC 6979, A.2.5: P-256's key and its signatures of "sample" and "test" with SHA-256 */
	static const uint8_t key[32] = {
		0xC9, 0xAF, 0xA9, 0xD8, 0x45, 0xBA, 0x75, 0x16, 0x6B, 0x5C, 0x21, 0x57, 0x67, 0xB1, 0xD6, 0x93,
		0x4E, 0x50, 0xC3, 0xDB, 0x36, 0xE8, 0x9B, 0x12, 0x7B, 0x8A, 0x62, 0x2B, 0x12, 0x0F, 0x67, 0x21,
	};
	static const char *const messages[2] = { "sample", "test" };
	static const char *const signatures[2][2] = {
		{ "efd48b2aacb6a8fd1140dd9cd45e81d69d2c877b56aaf991c34d0ea84eaf3716",
			"f7cb1c942d657c41d436c7a1b6e29f65f3e900dbb9aff4064dc4ab2f843acda8" },
		{ "f1abb023518351cd71d881567b1ea663ed3efcf6c5132b354f28d3b0b7d38367",
			"019f4113742a2b14bd25926b49c649155f267e60d3814b4c0cc84250e46f0083" },
	};
	uint8_t bytes[131], digest[32], public_key[64], r[32], s[32];

	/* RFC 4231: cases 1, 2 and 6 (a key longer than SHA-256's block) */
	memset(bytes, 0x0B, 20);
	hmac_sha256(bytes, 20, "Hi There", 8, digest);
	if (!hex_is(digest, "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7"))
		return "HMAC-SHA256 (RFC 4231 case 1)";
	hmac_sha256((const uint8_t *)"Jefe", 4, "what do ya want for nothing?", 28, digest);
	if (!hex_is(digest, "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843"))
		return "HMAC-SHA256 (RFC 4231 case 2)";
	memset(bytes, 0xAA, 131);
	hmac_sha256(bytes, 131, "Test Using Larger Than Block-Size Key - Hash Key First", 54, digest);
	if (!hex_is(digest, "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54"))
		return "HMAC-SHA256 (RFC 4231 case 6)";
	ecdsa_public_key(key, public_key);
	if (!hex_is(public_key, "60fed4ba255a9d31c961eb74c6356d68c049b8923b61fa6ce669622e60f29fb6"
		"7903fe1008b8bc99a41ae9e95628bc64f2f1b20c2d7e9f5177a3c294d4462299"))
		return "P-256 public key (RFC 6979 A.2.5)";
	for (int message = 0; message < 2; message++)
	{
		sha256_of(messages[message], strlen(messages[message]), digest);
		ecdsa_sign(key, digest, r, s);
		if (!hex_is(r, signatures[message][0]) || !hex_is(s, signatures[message][1]))
			return message == 0 ? "ECDSA P-256 signature (RFC 6979 A.2.5, sample)"
				: "ECDSA P-256 signature (RFC 6979 A.2.5, test)";
		if (!ecdsa_verify(public_key, digest, r, s))
			return "ECDSA P-256 verification (RFC 6979 A.2.5)";
		digest[31] ^= 1;
		if (ecdsa_verify(public_key, digest, r, s))
			return "ECDSA P-256 verification of another hash";
	}
	return NULL;
}

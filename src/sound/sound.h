/**
 *    Name:        sound.h
 *    Description: the sound program's ports, variables, tables, structures and functions
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    13-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#ifndef SONIC_SOUND_H
#define SONIC_SOUND_H

#include "sound/support.h"

/* the board's I/O ports the program uses */
#define FM1_ADDRESS 0x80u
#define FM1_DATA 0x81u
#define FM1_PART2_ADDRESS 0x82u
#define FM1_PART2_DATA 0x83u
#define FM2_ADDRESS 0x90u
#define FM2_DATA 0x91u
#define FM2_PART2_ADDRESS 0x92u
#define FM2_PART2_DATA 0x93u
#define ROM_BANK_LOW 0xA0u
#define ROM_BANK_HIGH 0xB0u
#define IRQ_CONTROL 0xC0u
#define IRQ_ACKNOWLEDGE 0xC1u
#define IRQ_VECTOR0_INPUT 0xD0u
#define IRQ_VECTOR1_INPUT 0xD1u
#define IRQ_VECTOR2_INPUT 0xD2u
#define IRQ_MASK 0xD3u
#define IRQ_CONTROL_D8 0xD8u

/* the sound program's variables, in the RAM it shares with the V60 (E000-FFFF) */
#define unread_e000_ADDRESS 0xE000u
#define unread_e000 SOUND_RAM8(unread_e000_ADDRESS)
#define midi_queue_read_ADDRESS 0xE002u
#define midi_queue_read SOUND_RAM8(midi_queue_read_ADDRESS)
#define midi_status_ADDRESS 0xE008u
#define midi_status SOUND_RAM8(midi_status_ADDRESS)
#define sample_or_patch_ADDRESS 0xE00Du
#define sample_or_patch SOUND_RAM8(sample_or_patch_ADDRESS)
#define unread_e010_ADDRESS 0xE010u
#define unread_e010 SOUND_RAM16(unread_e010_ADDRESS)
#define note_code_ADDRESS 0xE012u
#define note_code SOUND_RAM8(note_code_ADDRESS)
#define note_level_ADDRESS 0xE013u
#define note_level SOUND_RAM8(note_level_ADDRESS)
#define pitch_bend_ADDRESS 0xE014u
#define pitch_bend SOUND_RAM16S(pitch_bend_ADDRESS)
#define note_pan_ADDRESS 0xE017u
#define note_pan SOUND_RAM8(note_pan_ADDRESS)
#define fm_pan_modulation_ADDRESS 0xE01Bu
#define fm_pan_modulation SOUND_RAM8(fm_pan_modulation_ADDRESS)
#define music_volume_ADDRESS 0xE01Cu
#define music_volume SOUND_RAM8(music_volume_ADDRESS)
#define kit_pitch_low_ADDRESS 0xE01Du
#define kit_pitch_low SOUND_RAM8(kit_pitch_low_ADDRESS)
#define kit_pitch_high_ADDRESS 0xE01Eu
#define kit_pitch_high SOUND_RAM8(kit_pitch_high_ADDRESS)
#define fm_channel_ADDRESS 0xE01Fu
#define fm_channel SOUND_RAM8(fm_channel_ADDRESS)
#define pcm_voices_used_ADDRESS 0xE020u
#define pcm_voices_used SOUND_RAM8(pcm_voices_used_ADDRESS)
#define pcm_note_channel_ADDRESS 0xE025u
#define pcm_note_channel SOUND_RAM8(pcm_note_channel_ADDRESS)
#define pcm_channels_on_ADDRESS 0xE026u
#define pcm_channels_on SOUND_RAM8(pcm_channels_on_ADDRESS)
#define refill_voice_ADDRESS 0xE027u
#define refill_voice SOUND_RAM16(refill_voice_ADDRESS)
#define refill_voices_left_ADDRESS 0xE029u
#define refill_voices_left SOUND_RAM8(refill_voices_left_ADDRESS)
#define rom_bank_low_ADDRESS 0xE02Cu
#define rom_bank_low SOUND_RAM8(rom_bank_low_ADDRESS)
#define rom_bank_high_ADDRESS 0xE02Du
#define rom_bank_high SOUND_RAM8(rom_bank_high_ADDRESS)
#define fm_voices_to_end_ADDRESS 0xE02Eu
#define fm_voices_to_end SOUND_RAM8(fm_voices_to_end_ADDRESS)
#define next_fm_voice_ADDRESS 0xE02Fu
#define next_fm_voice SOUND_RAM16(next_fm_voice_ADDRESS)
#define sample_detune_ADDRESS 0xE031u
#define sample_detune SOUND_RAM8S(sample_detune_ADDRESS)
#define vibrato_voice_ADDRESS 0xE032u
#define vibrato_voice SOUND_RAM16(vibrato_voice_ADDRESS)
#define vibrato_center_ADDRESS 0xE034u
#define vibrato_center SOUND_RAM16(vibrato_center_ADDRESS)
#define new_fm_voice_ADDRESS 0xE036u
#define new_fm_voice SOUND_RAM16(new_fm_voice_ADDRESS)
#define channel_pitch_bends_ADDRESS 0xE040u
#define channel_pitch_bends SOUND_RAM16(channel_pitch_bends_ADDRESS)
#define channel_controls_ADDRESS 0xE060u
#define channel_controls ((uint8_t *)sound_ram_at(channel_controls_ADDRESS))
#define channel_vibrato_presets_ADDRESS 0xE070u
#define channel_vibrato_presets ((uint8_t *)sound_ram_at(channel_vibrato_presets_ADDRESS))
#define midi_queue_ADDRESS 0xE100u
#define midi_queue SOUND_RAM8(midi_queue_ADDRESS)
#define voice_0_allocation_order_ADDRESS 0xE406u
#define voice_0_allocation_order SOUND_RAM8(voice_0_allocation_order_ADDRESS)
#define voice_1_allocation_order_ADDRESS 0xE418u
#define voice_1_allocation_order SOUND_RAM8(voice_1_allocation_order_ADDRESS)
#define voice_2_allocation_order_ADDRESS 0xE42Au
#define voice_2_allocation_order SOUND_RAM8(voice_2_allocation_order_ADDRESS)
#define voice_3_allocation_order_ADDRESS 0xE43Cu
#define voice_3_allocation_order SOUND_RAM8(voice_3_allocation_order_ADDRESS)
#define voice_4_allocation_order_ADDRESS 0xE44Eu
#define voice_4_allocation_order SOUND_RAM8(voice_4_allocation_order_ADDRESS)
#define voice_5_allocation_order_ADDRESS 0xE460u
#define voice_5_allocation_order SOUND_RAM8(voice_5_allocation_order_ADDRESS)
#define voice_6_allocation_order_ADDRESS 0xE472u
#define voice_6_allocation_order SOUND_RAM8(voice_6_allocation_order_ADDRESS)
#define voice_7_allocation_order_ADDRESS 0xE484u
#define voice_7_allocation_order SOUND_RAM8(voice_7_allocation_order_ADDRESS)
#define sound_command_ADDRESS 0xE700u
#define sound_command SOUND_RAM8(sound_command_ADDRESS)
#define sound_command_group_ADDRESS 0xE701u
#define sound_command_group SOUND_RAM8(sound_command_group_ADDRESS)
#define playlist_song_choice_1_ADDRESS 0xE709u
#define playlist_song_choice_1 SOUND_RAM8(playlist_song_choice_1_ADDRESS)
#define playlist_song_choice_2_ADDRESS 0xE70Au
#define playlist_song_choice_2 SOUND_RAM8(playlist_song_choice_2_ADDRESS)
#define player_end_flag_ADDRESS 0xE70Bu
#define player_end_flag SOUND_RAM8(player_end_flag_ADDRESS)
#define tick_countdown_ADDRESS 0xE740u
#define tick_countdown SOUND_RAM8(tick_countdown_ADDRESS)
#define midi_queue_write_ADDRESS 0xE758u
#define midi_queue_write SOUND_RAM8(midi_queue_write_ADDRESS)
#define midi_queue_count_ADDRESS 0xE759u
#define midi_queue_count SOUND_RAM8(midi_queue_count_ADDRESS)
#define music_player_flags_ADDRESS 0xE900u
#define music_player_flags SOUND_RAM8(music_player_flags_ADDRESS)
#define music_fade_speed_ADDRESS 0xE918u
#define music_fade_speed SOUND_RAM8(music_fade_speed_ADDRESS)
#define music_fade_wait_ADDRESS 0xE919u
#define music_fade_wait SOUND_RAM8(music_fade_wait_ADDRESS)
#define spare_players_ADDRESS 0xF100u
#define spare_players SOUND_RAM8(spare_players_ADDRESS)
#define sound_cpu_hold_ADDRESS 0xFFFFu
#define sound_cpu_hold SOUND_RAM8(sound_cpu_hold_ADDRESS)

/* data in the program ROM the code reads */
#define fm_global_init_ADDRESS 0x0273u
#define fm_global_init SOUND_ROM8(fm_global_init_ADDRESS)
#define fm_operator_init_ADDRESS 0x0289u
#define fm_operator_init SOUND_ROM8(fm_operator_init_ADDRESS)
#define pcm_register_init_ADDRESS 0x031Bu
#define pcm_register_init SOUND_ROM8(pcm_register_init_ADDRESS)
#define voice_chip_channels_ADDRESS 0x0387u
#define voice_chip_channels SOUND_ROM8(voice_chip_channels_ADDRESS)
#define pcm_channel_bits_ADDRESS 0x06F1u
#define pcm_channel_bits ((const uint8_t *)sound_rom_at(pcm_channel_bits_ADDRESS))
#define fm_patch_registers_ADDRESS 0x134Bu
#define fm_patch_registers SOUND_ROM8(fm_patch_registers_ADDRESS)
#define fm_level_registers_ADDRESS 0x1360u
#define fm_level_registers SOUND_ROM8(fm_level_registers_ADDRESS)
#define fm_fnum_tables_ADDRESS 0x137Cu
#define fm_fnum_tables SOUND_ROM16(fm_fnum_tables_ADDRESS)
#define part_templates_ADDRESS 0x1594u
#define part_templates SOUND_ROM16(part_templates_ADDRESS)
#define instruments_ADDRESS 0x193Bu
#define instruments SOUND_ROM16(instruments_ADDRESS)
#define note_codes_ADDRESS 0x1A3Eu
#define note_codes ((const uint8_t *)sound_rom_at(note_codes_ADDRESS))
#define velocity_levels_ADDRESS 0x1BFEu
#define velocity_levels SOUND_ROM8(velocity_levels_ADDRESS)
#define pcm_pitch_tables_ADDRESS 0x1E96u
#define pcm_pitch_tables SOUND_ROM16(pcm_pitch_tables_ADDRESS)
#define pitch_bend_steps_ADDRESS 0x232Eu
#define pitch_bend_steps ((const uint8_t *)sound_rom_at(pitch_bend_steps_ADDRESS))
#define fm_patches_ADDRESS 0x242Eu
#define fm_patches SOUND_ROM16(fm_patches_ADDRESS)
#define drum_kits_ADDRESS 0x3F2Eu
#define drum_kits SOUND_ROM16(drum_kits_ADDRESS)
#define vibrato_presets_ADDRESS 0x45E2u
#define vibrato_presets SOUND_ROM16(vibrato_presets_ADDRESS)
#define sound_groups_ADDRESS 0x4A78u
#define sound_groups SOUND_ROM16(sound_groups_ADDRESS)
#define songs_ADDRESS 0x4C66u
#define songs SOUND_ROM16(songs_ADDRESS)
#define pcm_sample_headers_ADDRESS 0x6429u
#define pcm_sample_headers SOUND_ROM16(pcm_sample_headers_ADDRESS)

/* the sample bank's window and the RF5C68 (read and written through the bus) */
#define ROM_BANK_WINDOW 0xA000u
#define PCM_CONTROL 0xC007u
#define PCM_WAVE_WINDOW 0xD000u
#define PCM_SILENT_LOOP 0xD2BCu

/* data in the program ROM sound_symbols.txt labels */
#define playlist_command_table_ADDRESS 0x4706u

/* the sound program's structures in the shared RAM */
#pragma pack(push, 1)
struct fm_voice
{
	uint8_t flags;
	uint8_t chip_channel;
	uint8_t note;
	uint8_t midi_channel;
	uint8_t pad_04[0x1];
	uint8_t level;
	uint8_t pad_06[0x1];
	uint8_t vibrato_delay;
	uint8_t vibrato_wait;
	uint8_t vibrato_range;
	uint8_t vibrato_depth;
	uint8_t vibrato_rise;
	uint8_t vibrato_fall;
	uint8_t vibrato_phase;
	uint8_t vibrato_rate;
	uint16_t vibrato_offset;
	uint8_t vibrato_block;
};

struct midi_part
{
	uint8_t flags;
	uint8_t midi_channel;
	uint8_t volume;
	uint8_t program;
	int8_t transpose;
	int8_t fm_detune;
	uint8_t pad_06[0x2];
	uint8_t effective_volume;
};

struct sample_voice
{
	uint8_t flags;
	uint8_t chip_channel;
	uint8_t note;
	uint8_t midi_channel;
	uint8_t pad_04[0x1];
	uint8_t level;
	uint8_t allocation_order;
	uint8_t sample_bank_high;
	uint8_t sample_bank_low;
	uint8_t wave_page;
	uint8_t source_page;
	uint8_t header_bank_step;
	uint8_t header_address_low;
	uint8_t header_address_high;
	uint16_t pitch_step;
	uint16_t stream_position;
};

struct sequence_player
{
	uint8_t flags;
	uint8_t pad_01[0x1];
	uint16_t position;
	uint8_t pad_04[0x2];
	uint16_t delta_time;
	uint8_t pad_08[0x2];
	uint8_t meta_51_value;
	uint8_t pad_0B[0x9];
	uint8_t running_status;
	uint8_t fade_volume;
	uint8_t pad_16[0x2];
	uint8_t fade_speed;
	uint8_t fade_wait;
	uint8_t fade_channel_volume;
	uint8_t bank;
	uint8_t pad_1C[0x4];
	uint16_t playlist;
	uint8_t pad_22[0xDE];
};

#pragma pack(pop)

_Static_assert(sizeof(struct fm_voice) == 0x12, "struct fm_voice has the size of the game's blocks");
_Static_assert(sizeof(struct midi_part) == 0x9, "struct midi_part has the size of the game's blocks");
_Static_assert(sizeof(struct sample_voice) == 0x12, "struct sample_voice has the size of the game's blocks");
_Static_assert(sizeof(struct sequence_player) == 0x100, "struct sequence_player has the size of the game's blocks");

#define FM_VOICE(address) ((struct fm_voice *)sound_ram_at(address))
#define MIDI_PART(address) ((struct midi_part *)sound_ram_at(address))
#define SAMPLE_VOICE(address) ((struct sample_voice *)sound_ram_at(address))
#define SEQUENCE_PLAYER(address) ((struct sequence_player *)sound_ram_at(address))
#define FM_VOICE_ADDRESS(n) (0xE490u + (n) * 0x12u)    /* the 12 of them, from 0 */
#define MIDI_PART_ADDRESS(n) (0xE300u + (n) * 0x9u)    /* the 13 of them, from 0 */
#define SAMPLE_VOICE_ADDRESS(n) (0xE400u + (n) * 0x12u)    /* the 8 of them, from 0 */
#define SEQUENCE_PLAYER_ADDRESS(n) (0xE900u + (n) * 0x100u)    /* the 8 of them, from 0 */

void sound_reset(void);
void sound_irq(void);
void sound_main_loop(void);
void sound_init(uint8_t unused_c);
uint16_t init_fm_chips(void);
uint8_t write_fm_global(uint8_t fm_register, uint8_t value);
void write_fm_channel(uint8_t fm_register, uint8_t value);
void write_fm1(uint8_t fm_register, uint8_t value);
void write_fm1_part2(uint8_t fm_register, uint8_t value);
void write_fm2(uint8_t fm_register, uint8_t value);
void write_fm2_part2(uint8_t fm_register, uint8_t value);
void init_pcm_chip(uint32_t unused_c);
void write_pcm_register(uint8_t pcm_register, uint8_t register_value);
void init_track_channels(uint32_t unused_c);
void init_pcm_tracks(void);
void init_midi_parts(void);
uint16_t read_table_word(uint16_t table, uint8_t index);
void handle_midi_message(void);
void skip_midi_data(uint8_t first_byte);
void play_midi_message(uint16_t message_byte, uint16_t part);
uint16_t retune_voice(uint16_t loop_count, uint16_t part, uint16_t voice);
void update_fm_modulation(uint16_t *p_loop_count, uint16_t *p_voice);
void read_midi_byte(uint16_t *p_byte_copy, uint16_t *p_queue_slot, uint8_t *p_byte);
void start_pcm_sample(uint16_t voice);
void refill_pcm_voice(void);
void stop_pcm_voice(uint16_t voice);
uint16_t multiply_bytes(uint16_t factor, uint16_t value);
void copy_sample_page(uint16_t *p_wave_address, uint16_t *p_sample_address);
void fm_note_off(uint16_t *p_de, uint16_t *p_saved_channel, uint8_t *p_a);
uint8_t stop_fm_voice(uint16_t voice);
void release_held_fm_voice(uint16_t voice);
void apply_fm_music_volume(uint8_t chip_channel, uint16_t level, uint16_t part);
void apply_fm_channel_volume(uint8_t chip_channel, uint16_t level, uint16_t part);
void load_fm_patch(uint16_t *p_channel_in_part, uint16_t *p_register_list, uint16_t *p_patch_data);
void sequence_tick(void);
void playlist_command(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page, uint16_t *p_playlist, uint8_t *p_playlist_byte);
void step_player_if_playing(uint16_t player_address, uint16_t mailbox_page);
void step_player(uint16_t player_address, uint16_t mailbox_page);
uint8_t chain_next_event(uint16_t player, uint8_t data_byte);
void clear_player_end_flag(uint16_t mailbox_page);
void poll_sound_command(void);
void start_song(uint16_t player, uint8_t song_number);
void start_sequence(uint16_t header, uint16_t player);
void select_channel_player(uint16_t *p_header, uint16_t *p_player);
uint16_t clear_player(uint16_t player);
void fade_music_volume(uint16_t mailbox_page);
void fade_player_volume(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page);
uint16_t next_sequence_byte(uint16_t cursor, uint16_t player);
void step_fm_vibrato(uint16_t voice);

#endif

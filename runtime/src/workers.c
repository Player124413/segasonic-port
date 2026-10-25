/**
 *    Name:        workers.c
 *    Description: threads that take bands of a frame's rows (workers.h): Windows' threads, POSIX threads, or
 *                 on the Switch libnx's, each on a core of its own
 *    Version:     0.1.0
 *    Created:     23-10-2026
 *    Modified:    23-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    Each call is a new job: the threads wake, take their band (worker n the band n + 1; the caller band 0)
 *    and say they are done; the caller waits for all of them. Rows are split evenly, so the bands differ by
 *    one row at most.
 */
#include "s32/workers.h"

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
typedef SRWLOCK pool_lock;
typedef CONDITION_VARIABLE pool_signal;
#define LOCK(lock) AcquireSRWLockExclusive(lock)
#define UNLOCK(lock) ReleaseSRWLockExclusive(lock)
#define WAIT(signal, lock) SleepConditionVariableSRW(signal, lock, INFINITE, 0)
#define WAKE_ALL(signal) WakeAllConditionVariable(signal)
#define WAKE(signal) WakeConditionVariable(signal)
#elif defined(__SWITCH__)
#include <switch.h>
typedef Mutex pool_lock;
typedef CondVar pool_signal;
#define LOCK(lock) mutexLock(lock)
#define UNLOCK(lock) mutexUnlock(lock)
#define WAIT(signal, lock) condvarWait(signal, lock)
#define WAKE_ALL(signal) condvarWakeAll(signal)
#define WAKE(signal) condvarWakeOne(signal)
#else
#include <pthread.h>
typedef pthread_mutex_t pool_lock;
typedef pthread_cond_t pool_signal;
#define LOCK(lock) pthread_mutex_lock(lock)
#define UNLOCK(lock) pthread_mutex_unlock(lock)
#define WAIT(signal, lock) pthread_cond_wait(signal, lock)
#define WAKE_ALL(signal) pthread_cond_broadcast(signal)
#define WAKE(signal) pthread_cond_signal(signal)
#endif

#define WORKERS_MOST 7

static struct
{
	int count;
	pool_lock lock;
	pool_signal job_ready;
	pool_signal job_done;
	unsigned job;            /* the job's number: a worker takes each once */
	int done;                /* workers done with it */
	s32_rows_work work;
	void *context;
	int first;
	int last;
#ifdef __SWITCH__
	Thread threads[WORKERS_MOST];
#endif
} pool;

/* Band 'band' of 'bands' of rows first..last */
static void run_band(s32_rows_work work, void *context, int first, int last, int band, int bands)
{
	const int rows = last - first + 1;
	const int from = first + rows * band / bands;
	const int to = first + rows * (band + 1) / bands - 1;

	if (from <= to)
		work(context, from, to);
}

static void worker_loop(int number)
{
	unsigned seen = 0;

	for (;;)
	{
		s32_rows_work work;
		void *context;
		int first, last;

		LOCK(&pool.lock);
		while (pool.job == seen)
			WAIT(&pool.job_ready, &pool.lock);
		seen = pool.job;
		work = pool.work;
		context = pool.context;
		first = pool.first;
		last = pool.last;
		UNLOCK(&pool.lock);

		run_band(work, context, first, last, number + 1, pool.count + 1);

		LOCK(&pool.lock);
		if (++pool.done == pool.count)
			WAKE(&pool.job_done);
		UNLOCK(&pool.lock);
	}
}

#if defined(_WIN32)
static DWORD WINAPI worker_entry(LPVOID number)
{
	worker_loop((int)(intptr_t)number);
	return 0;
}
#elif defined(__SWITCH__)
static void worker_entry(void *number)
{
	worker_loop((int)(intptr_t)number);
}
#else
static void *worker_entry(void *number)
{
	worker_loop((int)(intptr_t)number);
	return NULL;
}
#endif

void s32_workers_start(int count)
{
	if (pool.count > 0 || count <= 0)
		return;
	if (count > WORKERS_MOST)
		count = WORKERS_MOST;
#if defined(_WIN32)
	InitializeSRWLock(&pool.lock);
	InitializeConditionVariable(&pool.job_ready);
	InitializeConditionVariable(&pool.job_done);
#elif defined(__SWITCH__)
	mutexInit(&pool.lock);
	condvarInit(&pool.job_ready);
	condvarInit(&pool.job_done);
#else
	pthread_mutex_init(&pool.lock, NULL);
	pthread_cond_init(&pool.job_ready, NULL);
	pthread_cond_init(&pool.job_done, NULL);
#endif
	for (int number = 0; number < count; number++)
	{
#if defined(_WIN32)
		HANDLE thread = CreateThread(NULL, 0, worker_entry, (LPVOID)(intptr_t)number, 0, NULL);

		if (!thread)
			break;
		CloseHandle(thread);
#elif defined(__SWITCH__)
		/* (an application has cores 0 to 2 and runs on 0: the workers on 1 and 2) */
		if (R_FAILED(threadCreate(&pool.threads[number], worker_entry, (void *)(intptr_t)number, NULL, 0x10000,
			0x2C, 1 + number % 2)) || R_FAILED(threadStart(&pool.threads[number])))
			break;
#else
		pthread_t thread;

		if (pthread_create(&thread, NULL, worker_entry, (void *)(intptr_t)number) != 0)
			break;
		pthread_detach(thread);
#endif
		pool.count++;
	}
}

int s32_workers_count(void)
{
	return pool.count;
}

void s32_workers_rows(s32_rows_work work, void *context, int first, int last)
{
	if (pool.count == 0)
	{
		work(context, first, last);
		return;
	}
	LOCK(&pool.lock);
	pool.work = work;
	pool.context = context;
	pool.first = first;
	pool.last = last;
	pool.done = 0;
	pool.job++;
	WAKE_ALL(&pool.job_ready);
	UNLOCK(&pool.lock);

	run_band(work, context, first, last, 0, pool.count + 1);

	LOCK(&pool.lock);
	while (pool.done < pool.count)
		WAIT(&pool.job_done, &pool.lock);
	UNLOCK(&pool.lock);
}

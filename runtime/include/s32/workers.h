/**
 *    Name:        workers.h
 *    Description: the frame's rows shared among threads: the video draws each layer, and mixes them, a band
 *                 of rows on each, the same pixels one thread draws
 *    Version:     0.1.0
 *    Created:     23-10-2026
 *    Modified:    23-10-2026
 *    Author:      Sebastian Toyos
 *
 *    license:BSD-3-Clause
 *    A port starts the threads its machine has room for (a handheld's cores); with none, the default, the
 *    caller draws every row, as the checks do. The rows of a frame do not depend on each other: each band
 *    reads what the layers and the board hold and writes its own rows.
 */
#ifndef S32_WORKERS_H
#define S32_WORKERS_H

#ifdef __cplusplus
extern "C" {
#endif

/* The work for rows 'first' to 'last' (inclusive) */
typedef void (*s32_rows_work)(void *context, int first, int last);

/* Starts 'count' threads beside the caller's (at most 7; 0 or less: none) */
void s32_workers_start(int count);

/* The threads started */
int s32_workers_count(void);

/* 'work' over rows 'first' to 'last': a band for each thread and one for the caller, which returns when
   every band is done */
void s32_workers_rows(s32_rows_work work, void *context, int first, int last);

#ifdef __cplusplus
}
#endif

#endif

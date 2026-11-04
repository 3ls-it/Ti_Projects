/*
* @file    include/gol_types.h
* @brief   Declares structs to hold initial values
*
* Copyright (c) 2026 J Adams <jfa63@duck.com>
*
* SPDX-License-Identifier: BSD-2-Clause
*/
#ifndef GOL_TYPES_H
#define GOL_TYPES_H

typedef struct {
    short x;
    short y;
} Cell;

/*
 * Coordinates in the life2 simulation world.
 *
 * TIGCC long is 32 bits, allowing coordinates outside
 * the visible 159x99 viewport.
 */
typedef struct {
    long x;
    long y;
} WorldCell;
#endif

/*
* @file    include/blinker.h
* @brief   Defines a blinker initial condition
*
* Copyright (c) 2026 J Adams <jfa63@duck.com>
*
* SPDX-License-Identifier: BSD-2-Clause
*/
#ifndef GOL_PATTERN_BLINKER_H
#define GOL_PATTERN_BLINKER_H

#include "gol_types.h"


static const Cell blinker[] = {
    { 78, 49 },
    { 79, 49 },
    { 80, 49 }
};

#define BLINKER_COUNT \
    (sizeof(blinker) / sizeof(blinker[0]))

#endif

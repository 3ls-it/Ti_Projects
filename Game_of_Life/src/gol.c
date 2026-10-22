/*
* @file    src/gol.c
* @brief   John Conway's Game of Life cellular automata
*
* Copyright (c) 2026 J Adams <jfa63@duck.com>
*
* SPDX-License-Identifier: BSD-2-Clause
*/

#define SAVE_SCREEN
#define MIN_AMS 100
#define USE_TI89

#define WORLD_WIDTH   159
#define WORLD_HEIGHT   99
#define WORLD_X_MAX   (WORLD_WIDTH - 1)
#define WORLD_Y_MAX   (WORLD_HEIGHT - 1)
#define MAX_CELLS 1000 // Testing

#include <graph.h>
#include <kbd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/gol_types.h"
#include "../include/blinker.h"

static unsigned char world_a[WORLD_HEIGHT][WORLD_WIDTH];
static unsigned char world_b[WORLD_HEIGHT][WORLD_WIDTH];
static Cell next_cells[MAX_CELLS];
static int next_cnt;



// draw_border() written by Zeljko Juric for Cave Blaster
void
draw_border(void)
{
    SCR_RECT screen_area={{0,0,159,99}};

	int i;
    ClrScr();
    for(i=0;i<=3;i+=2)
        DrawClipRect(MakeWinRect(i,i,159-i,99-i),&screen_area,A_NORMAL);
}


void
start_screen(void)
{
    draw_border();
    FontSetSys(F_8x10);
    DrawStr(27, 10, "Game of Life", A_REPLACE);
    return;
}


// A small wraper for GetPix()
short
pn(short x, short y)
{
    if (GetPix(x, y))
        return 1;
    return 0;
}


// Load a pattern to the "World"
static void
load_pattern_to_world(unsigned char buffer[][WORLD_WIDTH],
                      const Cell *cells,
                      unsigned short count)
{
    unsigned short i;

    memset(buffer, 0, sizeof world_a);

    for (i = 0; i < count; ++i)
        buffer[cells[i].y][cells[i].x] = 1;
}


// Draw the world at current generation
static void
draw_world(const unsigned char buffer[][WORLD_WIDTH])
{
    short x;
    short y;

    clrscr();

    for (y = 0; y < WORLD_HEIGHT; ++y)
    {
        for (x = 0; x < WORLD_WIDTH; ++x)
        {
            if (buffer[y][x])
                DrawPix(x, y, A_NORMAL);
        }
    }
}


// Count neighbors
short
pixel_count(short x, short y)
{
    short cnt = \
        pn(x-1, y-1) + \
        pn(x, y-1) + \
        pn(x+1, y-1) + \
        pn(x-1, y) + \
        pn(x+1, y) + \
        pn(x-1, y+1) + \
        pn(x, y+1) + \
        pn(x+1, y+1);
    return cnt;
}


void
load_pattern(const Cell *cells, unsigned short count)
{
    unsigned short i;
    clrscr();

    for (i = 0; i < count; ++i)
        DrawPix(cells[i].x, cells[i].y, A_NORMAL);
}


void
draw_cells(const Cell *cells, unsigned short count)
{
    unsigned short i;

    for (i = 0; i < count; ++i)
        DrawPix(cells[i].x, cells[i].y, A_NORMAL);
}


static void
calculate_next_generation(
    const unsigned char current[][WORLD_WIDTH],
    unsigned char next[][WORLD_WIDTH])
{
    short x;
    short y;
    unsigned char neighbors;

    const unsigned char *row_above;
    const unsigned char *row_current;
    const unsigned char *row_below;
    unsigned char *row_next;

    for (y = 1; y < WORLD_Y_MAX; ++y)
    {
        row_above   = current[y - 1];
        row_current = current[y];
        row_below   = current[y + 1];
        row_next    = next[y];

        for (x = 1; x < WORLD_X_MAX; ++x)
        {
            neighbors =
                row_above[x - 1] +
                row_above[x]     +
                row_above[x + 1] +
                row_current[x - 1] +
                row_current[x + 1] +
                row_below[x - 1] +
                row_below[x]     +
                row_below[x + 1];

            if (neighbors == 3 ||
                (neighbors == 2 && row_current[x]))
            {
                row_next[x] = 1;
            }
			else
			{
				row_next[x] = 0;
			}
        }
    }
}


// Entry point
void
_main(void)
{
    unsigned char (*current_world)[WORLD_WIDTH];
    unsigned char (*next_world)[WORLD_WIDTH];
    unsigned char (*temp_world)[WORLD_WIDTH];

    short step;

    current_world = world_a;
    next_world = world_b;

    load_pattern_to_world(current_world,
                          blinker,
                          BLINKER_COUNT);

    draw_world(current_world);

    for (step = 0; step < 4; ++step)
    {
        calculate_next_generation(current_world, next_world);

        //draw_world(next_world);

        temp_world = current_world;
        current_world = next_world;
        next_world = temp_world;
    }

    exit(0);
#if 0
    clrscr();
    start_screen();
    FontSetSys(F_4x6);
    printf_xy(5, 40, "(Press any key to continue)");
    ngetchx();

    load_pattern(blinker, BLINKER_COUNT);

    int nsteps = 4;
    int step = 1;
    while (step <= nsteps)
    {
        unsigned int i = 0;
        next_cnt = 0;

        short x;
        for (x=1; x < WORLD_X_MAX; x++)
        {
            short y;
            for (y=1; y = WORLD_Y_MAX; y++)
            {
                short ncnt = pixel_count(x, y);
                if ((ncnt == 3) || ((ncnt == 2) && GetPix(x, y)))
                {
                    next_cells[i].x = x;
                    next_cells[i].y = y;
                    next_cnt++; 
                    ++i;
                }
            }
        }
        step++;
		clrscr();
        //draw_cells(next_cells, next_cnt); 
        load_pattern_to_world(blinker, BLINKER_COUNT);
        draw_world();
    }

    printf("\nDone");
    ngetchx(); //Pause before clearing
    exit(0);
#endif
}

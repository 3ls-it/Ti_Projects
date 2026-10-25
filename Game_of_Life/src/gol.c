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
#define USE_TI92PLUS
#define USE_V200

#define WORLD_WIDTH   159
#define WORLD_HEIGHT   99
#define WORLD_X_MAX   (WORLD_WIDTH - 1)
#define WORLD_Y_MAX   (WORLD_HEIGHT - 1)
#define MAX_CELLS 2000
#define DEFAULT_STEPS 240

#include <args.h>
#include <graph.h>
#include <kbd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../include/gol_types.h"
#include "../include/blinker.h"
#include "../include/gosper_glider_gun.h"
#include "../include/max.h"

static unsigned char world_a[WORLD_HEIGHT][WORLD_WIDTH];
static unsigned char world_b[WORLD_HEIGHT][WORLD_WIDTH];

static unsigned char neighbor_counts[WORLD_HEIGHT][WORLD_WIDTH];
static unsigned char cells_overflowed;

static Cell cells_a[MAX_CELLS];
static Cell cells_b[MAX_CELLS];

static Cell *current_cells = cells_a;
static Cell *next_cells = cells_b;

static unsigned short current_count;
static unsigned short next_count;
static const Cell *selected_pattern;
static unsigned short selected_pattern_count;



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


static void
add_neighbor_counts(short x, short y)
{
    neighbor_counts[y - 1][x - 1]++;
    neighbor_counts[y - 1][x]++;
    neighbor_counts[y - 1][x + 1]++;

    neighbor_counts[y][x - 1]++;
    neighbor_counts[y][x + 1]++;

    neighbor_counts[y + 1][x - 1]++;
    neighbor_counts[y + 1][x]++;
    neighbor_counts[y + 1][x + 1]++;
}


static void
build_neighbor_counts(const Cell *cells,
                      unsigned short count)
{
    unsigned short i;

    memset(neighbor_counts, 0, sizeof neighbor_counts);

    for (i = 0; i < count; ++i)
        add_neighbor_counts(cells[i].x, cells[i].y);
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


static void
update_world_display(
    const unsigned char current[][WORLD_WIDTH],
    const unsigned char next[][WORLD_WIDTH])
{
    short x;
    short y;

    for (y = 0; y < WORLD_HEIGHT; ++y)
    {
        for (x = 0; x < WORLD_WIDTH; ++x)
        {
            if (current[y][x] != next[y][x])
            {
                if (next[y][x])
                    DrawPix(x, y, A_NORMAL);
                else
                    DrawPix(x, y, A_REVERSE);
            }
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
    const unsigned char *row_current;
    unsigned char *row_next;
    next_count = 0;
	cells_overflowed = 0;

    for (y = 1; y < WORLD_Y_MAX; ++y)
    {
        row_current = current[y];
        row_next = next[y];

        for (x = 1; x < WORLD_X_MAX; ++x)
        {
            neighbors = neighbor_counts[y][x];

            if (neighbors == 3 ||
                (neighbors == 2 && row_current[x]))
            {
                row_next[x] = 1;

                if (next_count < MAX_CELLS)
                {
                    next_cells[next_count].x = x;
                    next_cells[next_count].y = y;
                    ++next_count;
                }
				else
				{
					cells_overflowed = 1;
				}
            }
            else
            {
                row_next[x] = 0;
            }
        }
    }
}


static void
select_pattern(void)
{
    ESI argptr;
    const char *name;

    /* Default pattern when no argument is supplied. */
    selected_pattern = blinker;
    selected_pattern_count = BLINKER_COUNT;

    InitArgPtr(argptr);

    if (GetArgType(argptr) != STR_TAG)
        return;

    name = GetStrnArg(argptr);

    if (strcmp(name, "max") == 0)
    {
        selected_pattern = max;
        selected_pattern_count = MAX_COUNT;
    }
}


// Entry point
void
_main(void)
{
    unsigned char (*current_world)[WORLD_WIDTH];
    unsigned char (*next_world)[WORLD_WIDTH];
    unsigned char (*temp_world)[WORLD_WIDTH];
	Cell *temp_cells;
	unsigned short i;
    unsigned short step;

    current_world = world_a;
    next_world = world_b;

	select_pattern();

	if (selected_pattern_count > MAX_CELLS)
	{
		printf_xy(2, 2, "Pattern too large");
		printf_xy(2, 10, "Limit: %u", MAX_CELLS);
		ngetchx();
		exit(0);
	}

	current_count = selected_pattern_count;

	for (i = 0; i < current_count; ++i)
		current_cells[i] = selected_pattern[i];

    clrscr();
    start_screen();
    FontSetSys(F_4x6);
    printf_xy(5, 40, "(Press any key to continue)");
    ngetchx();

	load_pattern_to_world(current_world,
						  selected_pattern,
						  selected_pattern_count);

    draw_world(current_world);

    for (step = 0; step < DEFAULT_STEPS; ++step)
	{
		build_neighbor_counts(current_cells,
							  current_count);

		calculate_next_generation(current_world,
								   next_world);

		if (cells_overflowed)
		{
			printf_xy(2, 2, "Population limit exceeded");
			printf_xy(2, 10, "Limit: %u", MAX_CELLS);
			ngetchx();
			exit(0);
		}
		current_count = next_count;

		update_world_display(current_world, next_world);

		temp_world = current_world;
		current_world = next_world;
		next_world = temp_world;

    	temp_cells = current_cells;
    	current_cells = next_cells;
	    next_cells = temp_cells;

        if (kbhit())
        {
            ngetchx();       /* Consume the key. */
            break;           /* User requested stop. */
        }
	}

	printf_xy(2, 2, "Stopped at: %u", step);
	printf_xy(2, 2, "Live: %u  ", current_count);
	ngetchx();
    exit(0);
}

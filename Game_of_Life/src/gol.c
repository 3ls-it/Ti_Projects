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

#include <tigcclib.h>

#include "../include/gol_types.h"
#include "../include/blinker.h"

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



// Entry point
void
_main(void)
{
    clrscr();
    draw_border();
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
        for (x=1; x<=WORLD_X_MAX; x++)
        {
            short y;
            for (y=1; y<=WORLD_Y_MAX; y++)
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
        draw_cells(next_cells, next_cnt); 
    }

    printf("\nDone");
    ngetchx(); //Pause before clearing
    exit(0);
}

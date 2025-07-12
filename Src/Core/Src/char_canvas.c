
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "char_canvas.h"

static uint8_t canvas[2][ROW_SIZE_PAL*COLUMN_SIZE] = {0};
static uint8_t canvas_active = 0;
static bool canvas_write = false;
static bool canvas_next = false;


void charCanvasInit(void)
{
#if 0
    uint8_t ch=0;
    for(int x=0; x<ROW_SIZE_PAL*COLUMN_SIZE; x++){
        canvas[0][x] = ch;
        canvas[1][x] = ch++;
    }
#else
    memset(canvas, 0, sizeof(canvas));
#endif
}

void charCanvasClear(void)
{
    memset(&canvas[canvas_active ^ 1][0], ' ', sizeof(canvas[0]));
}

void charCanvasDraw(void)
{
    if ( canvas_write ){
        canvas_next = true;
        canvas_write = false;
    };
}

void charCanvasWrite(uint8_t row, uint8_t column, uint8_t* data, uint8_t len)
{
    if (row < ROW_SIZE_PAL){
        memcpy(&canvas[canvas_active ^ 1][row*COLUMN_SIZE + column], data, len);
        canvas_write = true;
    }
}

void charCanvasNext(void)
{
    if (canvas_next){
        canvas_next = false;
        canvas_active ^= 1;
    }
}

uint8_t* charCanvasGet(uint16_t row)
{
    return &canvas[canvas_active][row * COLUMN_SIZE];
}



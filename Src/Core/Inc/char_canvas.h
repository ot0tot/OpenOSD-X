#ifndef __CHARCANVAS_H
#define __CHARCANVAS_H

void charCanvasInit(void);
void charCanvasClear(void);
void charCanvasDraw(void);
void charCanvasWrite(uint8_t row, uint8_t column, uint8_t* data, uint8_t len);
void charCanvasNext(void);
uint8_t* charCanvasGet(uint16_t row);


#endif

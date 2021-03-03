#ifndef __SSD1283A_H
#define __SSD1283A_H

#include <stdint.h>
#include "cmsis_os.h"
#include "semphr.h"
#include "bmp.h"

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


#define DISP_W 129
#define DISP_H 129

#define HALF_DISP_W 64
#define HALF_DISP_H 64

#define FIRST_DISP_W 31
#define SECOND_DISP_W 63
#define THIRD_DISP_W 95



#define hightY 12
#define offsetTextY(i) (9 + hightY * i)
#define offsetItemY(i) (offsetTextY(i) - 1)



class SSD1283A {
const uint16_t TFTLCD_DELAY16 = 0xFFFF;

public:
    SSD1283A();
    void init(void);


    void drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawLineV(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawLineH(int16_t x, int16_t y, int16_t w, uint16_t color);
    void fillScreen(uint16_t color);
    void drawImage(uint8_t x, uint8_t y, const tImage *image, bool transparent = true, uint16_t color = BLACK, uint16_t ncolor = WHITE);
    uint8_t lenPixText(const char *str, const tFont &font);
    uint8_t drawText(uint8_t X, uint8_t Y, const char *str, const tFont &font, bool transparent = true, uint16_t color = BLACK, uint16_t ncolor = WHITE);
    uint8_t drawChar(uint8_t X, uint8_t Y, char c, const tFont &font, bool transparent = true, uint16_t color = BLACK, uint16_t ncolor = WHITE);


    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h);
    void setBackLight(bool lit);
private:
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    void setRotation(uint8_t r);
    void invertDisplay(bool i);
    void setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void setVerticalScroll(int16_t top, int16_t scrollines, int16_t offset);


    const tChar* _find_char_by_code(int code, const tFont &font);
    void _startTransaction();
    void _endTransaction();
    void _writeCommand(uint8_t cmd);
    void _writeData(uint8_t data);
    void _writeData16(uint16_t data);
    void _writeData16(uint16_t data, uint16_t n);
    void _writeData16(const uint16_t* data, uint16_t n);
    void _writeCommandData16(uint8_t cmd, uint16_t data);
    void _writeCommandDataTransaction16(uint8_t cmd, uint16_t data);
    void _setWindowAddress(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
    void _init_table16(const void *table, int16_t size);
  private:
    uint16_t WIDTH, HEIGHT, _width, _height, _rotation;
    uint16_t _inversion_bit;

    SemaphoreHandle_t xSemaphore;
    StaticSemaphore_t xMutexBuffer;
};

#endif // __SH1106_H

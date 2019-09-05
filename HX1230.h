#ifndef HX1230_H_
#define HX1230_H_

#include <mbed.h>
#include "Adafruit_GFX.h"
#include "glcdfont.c"


#define BLACK 0
#define WHITE 1

#define TFTWIDTH   96
#define TFTHEIGHT  68

//#define swap(a, b) { int16_t t = a; a = b; b = t; }

class HX1230: public Adafruit_GFX
{
private:
    DigitalOut * _mosi;
    DigitalOut * _clk;
    DigitalOut * _cs;
    DigitalOut * _reset;
    uint8_t _rotation;
    unsigned char ucScreen[864]; // local copy of the image buffer
    int _screenOffset; // current write offset of screen data
    void spiTransfer(uint8_t *pBuf, uint8_t bLen, uint8_t isData);
    void writeCommand(unsigned char c);
    void setPosition(int x, int y);
    void writeDataBlock(unsigned char *ucBuf, int iLen);
public:
    HX1230(PinName mosi, PinName clk, PinName cs, PinName rst);
    void init(void);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color);
    void drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillScreen(unsigned char ucData);
};

#endif
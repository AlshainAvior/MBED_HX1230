#include "HX1230.h"

HX1230::HX1230(PinName mosi, PinName clk, PinName cs, PinName rst) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT)
{
    this->_mosi = new DigitalOut(mosi);
    this->_clk = new DigitalOut(clk);
    this->_cs = new DigitalOut(cs);
    this->_reset = new DigitalOut(rst);
    *_cs = 1;
    *_reset = 1;
}

void HX1230::init(void)
{
    *_cs = 0;
    *_reset = 1;
    wait_ms(50);
    *_reset = 0;
    wait_ms(50);
    *_reset = 1;

    // power up 
    const unsigned char ucPowerUp[] = {0x2f, 0x90, 0xa6, 0xa4, 0xaf, 0x40, 0xb0, 0x10, 0x00};
    spiTransfer((unsigned char *)ucPowerUp, sizeof(ucPowerUp), 0);

    fillScreen(0);

    // set contrast
    writeCommand(0x80);

    /*
    // TODO implement setRotation
      writeCommand(0xa1); // set SEG direction (A1 to flip horizontal)
      writeCommand(0xc8); // set COM direction (C8 to flip vert)
    */
}

void HX1230::spiTransfer(uint8_t *pBuf, uint8_t bLen, uint8_t isData)
{
    unsigned char i, b, c;
    *_cs = 0;
    wait_us(20);
    for (i = 0; i < bLen; i++)
    {
        c = *pBuf++;
        // Write D/C bit
        *_mosi = isData;    // set data bit (MSB first)
        *_clk = 1;          // toggle clock
        wait_us(20);
        *_clk = 0;          // high->low transition latches data
        wait_us(20);
        for (b = 0; b < 8; b++)
        {
            *_mosi = (c & 0x80);    // set data bit (MSB first)
            *_clk = 1;              // toggle clock
            wait_us(20);
            *_clk = 0;              // high->low transition latches data
            c <<= 1; // shift over data
        } // for each bit
    } // for each byte

    *_cs = 1;
}

void HX1230::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    int i;
    unsigned char uc, ucOld;

    i = ((y >> 3) * 96) + x;
    if (i < 0 || i > 863) // off the screen
        return;
    uc = ucOld = ucScreen[i];
    uc &= ~(0x1 << (y & 7));

    if (color)
    {
        uc |= (0x1 << (y & 7));
    }

    if (uc != ucOld) // pixel changed
    {
        setPosition(x, y>>3);
        writeDataBlock(&uc, 1);
    }
}

void HX1230::drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color)
{
    for (int16_t i = 0; i < w; i++)
    {
        drawPixel(x0 + i, y0, color);
    }
}

void HX1230::drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color)
{
    for (int16_t i = 0; i < h; i++)
    {
        drawPixel(x0, y0 + i, color);
    }
}

void HX1230::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    for (int16_t i = 0; i < h; i++)
    {
        drawFastHLine(x, y + i, w, color);
    }
}

void HX1230::writeCommand(unsigned char c)
{
    spiTransfer(&c, 1, 0);
}

void HX1230::setPosition(int x, int y)
{
    if (x > 95 || y > 8)
    {
        return; // invalid
    }

    writeCommand(0xb0 | y); // set Y
    writeCommand(0x10 | (x >> 4)); // set X MSB
    writeCommand(0x00 | (x & 0xf)); // set X LSB
    _screenOffset = (y*96)+x;
}

// Write a block of pixel data to the LCD
// Length can be anything from 1 to 504 (whole display)
void HX1230::writeDataBlock(unsigned char *ucBuf, int iLen)
{
    if (_screenOffset < 0)
    {
        return; // invalid
    }

    //   Keep a copy in local buffer
    memcpy(&ucScreen[_screenOffset], ucBuf, iLen);

    _screenOffset += iLen;

    spiTransfer(ucBuf, iLen, 1);
}

void HX1230::fillScreen(unsigned char ucData)
{
    int x, y;
    unsigned char temp[32];

    memset(temp, ucData, 32);
    for (y=0; y<9; y++)
    {
        setPosition(0,y); // set to (0,Y)
        for (x=0; x<3; x++)
        {
            writeDataBlock(temp, 32); // fill with data unsigned char
        } // for x
    } // for y;
}
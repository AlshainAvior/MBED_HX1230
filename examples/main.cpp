#include <mbed.h>
#include "FreeSans9pt7b.h"
#include "HX1230.h"

#define MOSI_PIN PB_14
#define SCLK_PIN PB_15
#define CS_PIN PB_13
#define RST_PIN PB_12

int main() 
{
  HX1230 * lcd = new HX1230(MOSI_PIN, SCLK_PIN, CS_PIN, RST_PIN);
  lcd->init();
  //lcd->fillScreen(0xAA);
  lcd->setTextSize(1);
  lcd->setTextColor(WHITE, BLACK);
  lcd->setCursor(0, 0);
  lcd->println("Test123");
  lcd->drawPixel(10, 10, BLACK);
  lcd->drawPixel(11, 11, 1);

  while(1) 
  {
    wait(1);
  }
}
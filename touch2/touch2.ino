#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <bb_captouch.h>
#include "SPI.h"
#include <Wire.h>

#define TFT_CS   15
#define TFT_RST  -1
#define TFT_DC    2
#define TFT_MISO 12  // Data in
#define TFT_MOSI 13  // Data out
#define TFT_CLK  14  // Clock out
#define TFT_BL   27  // back.light
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25
#define TADR 0x15
BBCapTouch bbct;

void screen_init(){
  tft.fillScreen(ILI9341_BLACK); 
  tft.drawRect( 0,0,239,319,ILI9341_WHITE); 
}

void setup() {
  Serial.begin(115200);
  pinMode(TFT_BL, OUTPUT); digitalWrite(TFT_BL, HIGH);
  tft.begin(); screen_init();
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
}

void loop() {
 int i;
 TOUCHINFO ti;
  if (bbct.getSamples(&ti)) {
    if(ti.count==2) screen_init();
    for (int i=0; i<ti.count; i++){
      tft.fillCircle(ti.x[i],ti.y[i], 10, ILI9341_GREEN);
    }
  }
}

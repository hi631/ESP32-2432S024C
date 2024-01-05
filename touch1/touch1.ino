#include <bb_captouch.h>
#include <Wire.h>

// These defines are for a low cost ESP32 LCD board with the GT911 touch controller
#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25
#define TADR 0x15

BBCapTouch bbct;
const char *szNames[] = {"Unknown", "FT6x36", "GT911", "CST820"};

void setup() {
  uint8_t wbf[16];

  Serial.begin(115200);
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
  int iType = bbct.sensorType();
  Serial.printf("Sensor type = %s\n", szNames[iType]);
} /* setup() */

void loop() {
 int i;
 TOUCHINFO ti;
  if (bbct.getSamples(&ti)) {
    for (int i=0; i<ti.count; i++){
      if(i==0) Serial.println(""); else Serial.print("  ");
      Serial.print("Touch ");Serial.print(i+1);Serial.print(": ");;
      Serial.print("  x: ");Serial.print(ti.x[i]);
      Serial.print("  y: ");Serial.print(ti.y[i]);
      Serial.print("  size: ");
      Serial.print(ti.area[i]);
    }  // for each touch point
  }
  delay(10);
} /* loop() */

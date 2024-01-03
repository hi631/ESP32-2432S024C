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

void dumpreg(int radr, int rlen){
  unsigned char rbf[16];
  for(int i=0; i<rlen; i++) {
    if((i & 0x0f)==0) Serial.printf("\n%02x:", radr+i);
    bbct.I2CReadRegister(0x15,radr+i, rbf, 1);
    //Wire.beginTransmission(TADR);
    //Wire.write((uint8_t)i);
    //Wire.endTransmission();
    //Wire.requestFrom((uint8_t)TADR, (uint8_t)1);
    //rbf[0] = Wire.read();
    Serial.printf(" %02x",rbf[0]);
  }
}

void setreg(uint8_t ad, uint8_t dt){
  //for(int i=0; i<256; i++){
    Wire.beginTransmission((uint8_t)TADR);
    Wire.write((uint8_t)ad);
    Wire.write(dt);
    Wire.endTransmission();
  //}
}

void setup() {
  uint8_t wbf[16];

  Serial.begin(115200);
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
  int iType = bbct.sensorType();
  Serial.printf("Sensor type = %s\n", szNames[iType]);

  setreg(0x00, 0x07);

  dumpreg( 0x00, 256);

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

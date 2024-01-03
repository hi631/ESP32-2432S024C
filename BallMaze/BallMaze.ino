#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Wire.h>

//#define LGFX_AUTODETECT
#define LGFX_ESP32_2432S024C
#include <LGFX_AUTODETECT.hpp>  // クラス"LGFX"を準備します

// use bb_captouch
#include <bb_captouch.h>
#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25
BBCapTouch bbct;
const char *szNames[] = {"Unknown", "FT6x36", "GT911", "CST820"};

#if defined( LGFX_M5STACK ) || defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE ) // M5Stack

 #define BUTTON_A_PIN 39
 #define BUTTON_B_PIN 38

 static constexpr std::uint32_t draw_cycle = 3;
 static constexpr float zoom_min = 2;
 static constexpr float zoom_max = 32;

#elif defined( LGFX_M5STACK_CORE2 ) || defined( ARDUINO_M5STACK_Core2 )

 #define BUTTON_A_PIN -1
 #define BUTTON_B_PIN -1

 static constexpr std::uint32_t draw_cycle = 3;
 static constexpr float zoom_min = 2;
 static constexpr float zoom_max = 32;

#elif defined ( LGFX_M5STICKC ) || defined ( ARDUINO_M5Stick_C )

 #define BUTTON_A_PIN 37
 #define BUTTON_B_PIN 39

 static constexpr std::uint32_t draw_cycle = 1;
 static constexpr float zoom_min = 1.4;
 static constexpr float zoom_max = 16;

#elif defined( LGFX_ODROID_GO ) || defined( ARDUINO_ODROID_ESP32 ) // ODROID-GO

 #define BUTTON_A_PIN 32
 #define BUTTON_B_PIN 33

 static constexpr std::uint32_t draw_cycle = 3;
 static constexpr float zoom_min = 2;
 static constexpr float zoom_max = 32;

#elif defined( LGFX_TTGO_TWATCH ) || defined( ARDUINO_T ) // TTGO T-Watch

 #define BUTTON_A_PIN 36
 #define BUTTON_B_PIN -1

 static constexpr std::uint32_t draw_cycle = 2;
 static constexpr float zoom_min = 2;
 static constexpr float zoom_max = 32;

#elif defined( LGFX_WIO_TERMINAL ) || defined (ARDUINO_WIO_TERMINAL) || defined(WIO_TERMINAL)

 #define BUTTON_A_PIN 0x0200|28
 #define BUTTON_B_PIN 0x0200|27

 static constexpr std::uint32_t draw_cycle = 3;
 static constexpr float zoom_min = 2;
 static constexpr float zoom_max = 32;

#else
 //#define BUTTON_A_PIN -1
 //#define BUTTON_B_PIN -1
 int BUTTON_A_PIN = 0;
 int BUTTON_B_PIN = 0;
 static constexpr std::uint32_t draw_cycle = 3;
 static constexpr float zoom_min = 2;
 static constexpr float zoom_max = 16;

#endif

static LGFX lcd;
static LGFX_Sprite sp(&lcd);
static std::int32_t px, py;
static float ox, oy;
static float cx, cy, cr;
static float ax, ay;
static float zoom;
static float angle, rad;
static float add_angle   = 0;
static float add_advance = 0;
static std::uint32_t draw_count = 0;
static std::uint32_t skip_count = 0;

static constexpr float deg_to_rad = 0.017453292519943295769236907684886;
static constexpr float gravity = -0.005;
static constexpr float zoom_speed = 0.02;

static void create_maze(void)
{
  sp.clear(0);
  sp.setColor(1);
  sp.fillRect(0,                0, sp.width(), 16);
  sp.fillRect(0, sp.height() - 16, sp.width(), 16);
  sp.fillRect(            0, 0, 16, sp.height()-16);
  sp.fillRect(sp.width()-16, 0, 16, sp.height()-16);
  sp.setColor(3);
  for (int y = 1; y < sp.height(); y += 2)
  {
    for (int x = 1; x < sp.width(); x += 2)
    {
      sp.writePixel(x, y);
      int xx = x;
      int yy = y;
      do
      {
        xx = x;
        yy = y;
        switch (random(4)) {
        case 0: xx = x + 1; break;
        case 1: xx = x - 1; break;
        case 2: yy = y + 1; break;
        case 3: yy = y - 1; break;
        }
      } while (3 == sp.readPixelValue(xx, yy));
      sp.writePixel(xx, yy);
    }
  }
}

static void draw(void)
{
  draw_count += draw_cycle;
  std::uint_fast8_t blink = 127+abs(((int)(draw_count<<1) & 255)-128);
  sp.setPaletteColor(1, 127, 127, blink);
  sp.setPaletteColor(2, 127, blink, 127);

  float fx = (cx - ox);
  float fy = (cy - oy);
  float len = sqrtf(fx * fx + fy * fy) * zoom;
  float theta = atan2f(fx, fy) + rad;
  sp.pushRotateZoom(px - sinf(theta) * len, py - cosf(theta) * len, angle, zoom, zoom);
}

static void game_init(void)
{
  px = lcd.width()>>1;
  py = lcd.height()/3;

  create_maze();
  ox = sp.getPivotX();
  oy = sp.getPivotY();

  cx = (sp.width() >>1);
  cy = (sp.height()>>1);
  cr = 0.2;
  zoom = zoom_min;
}

static int lc = 0;  // test.count
static int slider[2] = {120,120};
static int32_t bb_getTouch(std::int32_t *tx, std::int32_t *ty)
{
  int32_t tc;
  int32_t up[2];
  TOUCHINFO ti;
  bool pressed = bbct.getSamples(&ti);
  tc = 0; up[0] = 0; up[1]= 0;
  BUTTON_A_PIN = 0; BUTTON_B_PIN = 0;
  if(pressed==true){
    for(int i =0; i<ti.count; i++){
      up[i] = 1;
      if(ti.y[i]<80)  { BUTTON_A_PIN = 1; slider[1] = 239-ti.x[i]; up[i] = 0; }
      if(ti.y[i]>240) { BUTTON_B_PIN = 1; slider[0] = 239-ti.x[i]; up[i] = 0; }
      if(up[i]==1) {
        *tx = ti.x[i]; *ty = ti.y[i]; tc = ti.count;
      }
    }
  }
  disp_bottun();
/*
  if(pressed==true) {
    Serial.printf("%d pc:%d  ",lc++, ti.count);
    Serial.printf("%d %d ",BUTTON_A_PIN,BUTTON_B_PIN);
    Serial.printf("0:%d %d ", ti.x[0],ti.y[0]);
    Serial.printf("s:%d %d ", slider[0],slider[1]);
    if(ti.count==2) Serial.printf(" 1:%d %d ", ti.x[1],ti.y[1]);
    Serial.printf("\n");
  }
*/ 
  return tc;
}

static void disp_bottun(){
    lcd.startWrite();
    lcd.fillRect( slider[0]-20,  0,40,40,0x00ffffU);
    lcd.fillRect( slider[1]-20,280,40,40,0x00ffffU);
    lcd.endWrite();
}
static bool game_main(void)
{
  std::int32_t tx, ty, tc;
  //tc = lcd.getTouch(&tx, &ty);
  tc = bb_getTouch(&tx, &ty);

  float ofs_angle = (float)0.01+((float)slider[1]/(float)800);
  if (tc) add_angle += (tx > lcd.width()>>1) ? ofs_angle : -ofs_angle; //0.6 : -0.6;
  zoom = 1 + slider[0] / 16;
  if (zoom < zoom_min) { zoom = zoom_min; }
  if (zoom > zoom_max) { zoom = zoom_max; }
/*
  if ((BUTTON_A_PIN >=0 && BUTTON_B_PIN >=0 && (lgfx::gpio_in(BUTTON_A_PIN) == 0 && lgfx::gpio_in(BUTTON_B_PIN) == 0)) || tc > 1)
  {
    zoom *= 1.0 - zoom_speed;
    if (zoom < zoom_min) { zoom = zoom_min; }
  }
  else
  {
    zoom *= 1.0 + zoom_speed;
    if (zoom > zoom_max) { zoom = zoom_max; }

    add_angle -= add_angle / 10;

    if (tc)
    {
      add_angle += (tx > lcd.width()>>1) ? 0.6 : -0.6;
    }
    else if (BUTTON_A_PIN >= 0 && BUTTON_B_PIN >= 0)
    {
      if (BUTTON_A_PIN >=0 && lgfx::gpio_in(BUTTON_A_PIN) == 0) add_angle += 0.6;
      if (BUTTON_B_PIN >=0 && lgfx::gpio_in(BUTTON_B_PIN) == 0) add_angle -= 0.6;
    }
    else
    {
      static bool pressed;
      static bool flow;
      if (BUTTON_A_PIN >=0 && lgfx::gpio_in(BUTTON_A_PIN) == 0)
      {
        if (!pressed)
        {
          pressed = true;
          flow = !flow;
        }
        add_angle += flow ? 0.6 : -0.6;
      }
      else
      {
        pressed = false;
      }
    }
  }
*/
  angle += add_angle;
  add_angle = add_angle * 9 / 10;
  rad = angle * - deg_to_rad;

  ax += sinf(rad) * gravity;
  ay -= cosf(rad) * gravity;
  ax = ax * 9.7 / 10;
  ay = ay * 9.7 / 10;

  float addy = (ay<0.0) ? -cr:cr;
  auto tmpy = roundf(cy+ay+addy);

  if ( 3 == sp.readPixelValue(roundf(cx), tmpy))
  {
    cy = tmpy - addy + (ay < 0.0 ? 0.5 : -0.5);
    ay = -ay * 9.0 / 10;
  }
  else
  {
    cy += ay;
  }

  float addx = (ax<0.0) ? -cr:cr;
  auto tmpx = roundf(cx+ax+addx);
  if ( 3 == sp.readPixelValue(tmpx, roundf(cy)))
  {
    cx = tmpx - addx + (ax < 0.0 ? 0.5 : -0.5);
    ax = -ax * 9.0 / 10;
  }
  else
  {
    cx += ax;
  }

  std::uint32_t pv = sp.readPixelValue(roundf(cx), roundf(cy));
  if ( 0 == pv)
  {
    sp.drawPixel(roundf(cx), roundf(cy), 2);
  }
  else if (1 == pv) return true;

  if (++skip_count == draw_cycle)
  {
    skip_count = 0;
    draw();
  }
  return false;
}


void setup(void)
{
  lgfx::lgfxPinMode(BUTTON_A_PIN, lgfx::pin_mode_t::input);
  lgfx::lgfxPinMode(BUTTON_B_PIN, lgfx::pin_mode_t::input);

  Serial.begin(115200);
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
  int iType = bbct.sensorType();
  Serial.printf("Sensor type = %s\n", szNames[iType]);

  lcd.init();
  lcd.startWrite();
  lcd.setColorDepth(16);
  sp.setColorDepth(2);
  sp.createSprite(257, 257);
  game_init();
}

void loop(void)
{
  if (!game_main())
  {
    lcd.fillCircle(px, py, roundf(cr*zoom), 0xFFFF00U);
  }
  else
  {
    std::int32_t py0 = lcd.height() >> 1;
    float cx0 = (cx + (sp.width() >>1)) / 2;
    float cy0 = (cy + (sp.height()>>1)) / 2;
    for (;;)
    {
      draw();
      if (py < py0) ++py;
      if (cx != cx0) cx += (cx0 - cx) / 256;
      if (cy != cy0) cy += (cy0 - cy) / 256;

      angle += .5;
      rad = angle * - deg_to_rad;
      zoom *= 1 - zoom_speed;
      if (zoom < zoom_min)
      {
        std::int32_t tx, ty;
        zoom = zoom_min;
        if ((BUTTON_A_PIN >=0 && lgfx::gpio_in(BUTTON_A_PIN) == 0)
         || (BUTTON_B_PIN >=0 && lgfx::gpio_in(BUTTON_B_PIN) == 0)
         || bb_getTouch(&tx, &ty) //lcd.getTouch(&tx, &ty)
         ) break;
      }
    }
    game_init();
  }
}

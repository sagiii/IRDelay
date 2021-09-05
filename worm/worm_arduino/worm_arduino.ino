#define LGFX_AUTODETECT
#include <M5StickC.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "worm_drawable.hpp"

WormGeometry geom(1, 0.2, 1.2, 2, 15);
Display disp(80, 70, 80, 160, 80);
WormDrawable worm(geom, disp);
Display disp2(80, 50, 60, 160, 80);
WormDrawable worm2(geom, disp2);
Display disp3(80, 30, 40, 160, 80);
WormDrawable worm3(geom, disp3);
Display disp4(80, 10, 20, 160, 80);
WormDrawable worm4(geom, disp4);

static LGFX lcd;
static LGFX_Sprite sprite(&lcd);

Timer timer;

void setup()
{
  M5.begin();
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(128);
  sprite.createSprite(M5.Lcd.height(), M5.Lcd.width()); // rotateしているので、幅と高さが入れ替わっている
  lcd.setColorDepth(24);
  sprite.setColorDepth(24);

  worm.head_magnify = 1.2;
  worm.speed = 1;
  worm.init();
  worm.setPosition(0, 1);
  worm.thick = 0.5;
  worm.width_magnify = 4;
  worm.body_color_mode = WormDrawable::GRADATION;
  worm4.speed = 1.1;
  worm4.length = 0.7;
  worm4.body_color_mode = WormDrawable::RAINBOW;
  worm3.speed = 0.5;
  worm3.length = 1.5;
  worm3.body_color_mode = WormDrawable::STRIPE;
  worm3.body_color0 = Color(30, 30, 180);
  worm3.body_color1 = Color(180, 180, 20);
  worm2.speed = 0.7;
  worm2.length = 3;
  worm2.division = 30;
  worm2.body_color_mode = WormDrawable::RANDOM;
  worm2.reverse_mergin = 1;
}

void loop()
{
  float dt = timer.wrap(millis()) / 1000.;
  M5.update();
  if (M5.BtnA.wasReleased()) {
    if (worm.speed > 0) {
      worm.speed = 0;
    } else {
      worm.speed = 0.1;
    }
  }
  sprite.fillScreen(TFT_WHITE);
  worm4.draw(sprite, dt);
  worm3.draw(sprite, dt);
  worm2.draw(sprite, dt);
  worm.draw(sprite, dt);
  delay(1); // 描画色が変になるのを防ぐ
  sprite.pushSprite(0, 0);
}

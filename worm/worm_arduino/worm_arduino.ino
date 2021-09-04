#define LGFX_AUTODETECT
#include <M5StickC.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "worm_drawable.hpp"

WormGeometry geom(1, 0.2, 1.2, 2, 15);
Display disp(80, 70, 80, 160, 80);
WormDrawable worm(geom, disp);

static LGFX lcd;
static LGFX_Sprite sprite(&lcd);

Timer timer;


void printOut(Display::Out const &out)
{
#define BOOL2TF(x) ((x) ? "T" : "F")
  Serial.printf("left(%s), right(%s), above(%s), below(%s)", BOOL2TF(out.left), BOOL2TF(out.right), BOOL2TF(out.above), BOOL2TF(out.below));
}

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
  worm.ease0.poly = Easing::QUART;
  worm.ease0.in = true;
  worm.ease0.out = true;
  worm.ease1.poly = Easing::QUAD;
  worm.ease1.in = true;
  worm.ease1.out = true;
  worm.init();
  worm.setPosition(0, 1);
  worm.thick = 0.5;
  worm.width_magnify = 4;
  worm.body_color_mode = WormDrawable::GRADATION;
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
  worm.draw(sprite, dt);
  delay(1); // 描画色が変になるのを防ぐ
  sprite.pushSprite(0, 0);

  if (worm.direction > 0 && disp.isOutD(worm.linksd[0]).right) { // turn left
    worm.setPosition(1.3, -1);
  } else if (worm.direction < 0 && disp.isOutD(worm.linksd[0]).left) { // turn right
    worm.setPosition(-1.3, 1);
  }
}

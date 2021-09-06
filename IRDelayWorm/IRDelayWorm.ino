#define LGFX_AUTODETECT
#include <M5StickC.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "worm_drawable.hpp"
#include "ir_delay.hpp"

const int ON_WARN = 30; // ONになる前の警告

static LGFX lcd;
static LGFX_Sprite sprite(&lcd);

Timer timer;

#define SINGLE (0)


#if SINGLE
Display disp(80, 70, 80, 160, 80);
WormGeometry geom(0.5, 0.1, 1.1, 2, 15);
WormBehavioral worm(geom, disp);
#else
std::vector<Display> displays;
std::vector<WormBehavioral*> worms; // NOTE: ポインタでない場合、Spriteを含むインスタンスがvector内部でコピーコンストラクタ経由でコピーされるため？Spriteを使う瞬間にパニックする。（createSpriteをコンストラクタで行わなければ良い説はあり）
const int MAX_WORMS = 10;

void create_depthed_worms(void)
{
  WormGeometry geom(0.5, 0.1, 1.1, 2, 15);
  // 手前は倍率80、奥は倍率10、その間を指数補間
  for (int i = 0; i < MAX_WORMS; i++) {
    float mag = exp(fmap(i, 0, MAX_WORMS - 1, log(80), log(10)));
    Display disp(80, mag * 7 / 8, mag, 160, 80);
    displays.push_back(disp);
    worms.push_back(new WormBehavioral(geom, disp));
  }
}

void delete_worms(void)
{
  for (auto i = worms.begin(); i != worms.end(); i++) {
    delete (*i);
  }
}
#endif

void irUpdated(IRDelay &ir)
{
  if (ir.is_on) { // onになった
  } else { // offの間
  }
}

IRDelay ir(irUpdated);

void setup()
{
  M5.begin();
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(128);
  sprite.createSprite(M5.Lcd.height(), M5.Lcd.width()); // rotateしているので、幅と高さが入れ替わっている
  lcd.setColorDepth(24);
  sprite.setColorDepth(24);
#if SINGLE
  worm.init();
  worm.setPosition(-1, 1);
#else
  create_depthed_worms();
  for (auto i = worms.begin(); i != worms.end(); i++) {
    (*i)->init();
    (*i)->setPosition(-1, 1);
    {
      static int j = 0;
      Serial.printf("worm addr[%d] = %lu\n", j, *i);
      j++;
    }
  }
#endif
}

void randomize_worm(WormBehavioral &worm)
{
  worm.thick = frand(.1, .2);
  worm.length = frand(.5, 1);
  worm.division = random(10, 20);
  worm.head_color = Color(random(100, 255), random(40), random(40));
  worm.eye_color = Color(random(220, 255), random(220, 255), random(100));
  worm.body_color0 = Color(random(0, 200), random(100, 255), random(0, 200));
  worm.body_color1 = Color(random(0, 200), random(100, 255), random(0, 200));
  switch(random(2)) {
    case 0:
    worm.body_color_mode = WormDrawable::GRADATION;
    break;
    case 1:
    worm.body_color_mode = WormDrawable::STRIPE;
    break;
  }
  worm.speed = frand(1, 2);
  worm.head_magnify = frand(.9, 1.1);
  worm.reverse_margin = frand(.2, .5);
  worm.width_magnify = frand(2, 4);
  worm.setPosition(-1.1, 1);
  worm.randomize_out_behavior = true;
}

Color background;

void loop()
{
  M5.update();

  if (M5.BtnA.wasReleased()) {
    if (ir.is_on) {
      // あおむし始まり
      int r = random(220, 255);
      int g = random(r, 255);
      int b = random(200, r);
      background = Color(r, g, b);
    }
    ir.turnOff();
    // あおむしの初期化（ランダマイズ、ちいちゃく）
#if SINGLE
    randomize_worm(worm);
    worm.start(ON_DELAY);
#else
    auto found = std::find_if(worms.begin(), worms.end(), [](WormBehavioral *w){ return w->status == WormBehavioral::NONE || w->status == WormBehavioral::FINISHED; });
    if (found != worms.end()) {
      Serial.printf("worm addr = %lu, size = %d\n", *found, worms.size());
      randomize_worm(**found);
      (*found)->start(ON_DELAY);
      Serial.println("started");
    }
#endif
  }

  if (M5.BtnB.wasReleased()) {
    ir.turnOn();
#if SINGLE
    worm.finish();
#else
    for (auto i = worms.begin(); i != worms.end(); i++) {
      (*i)->finish();
    }
#endif
  }

  ir.update(millis());

  if (ir.is_on) {
    sprite.fillScreen(TFT_BLACK);
    sprite.setColor(TFT_GREEN);
    float offset = - 4 * fabs(sin(millis() / 1000. * .5 * 2 * PI));
    float cursor_x1 = 159 + offset, cursor_y = 40;
    float cursor_width = 4, cursor_height = 4;
    sprite.fillTriangle(
      cursor_x1 - cursor_width, cursor_y + cursor_height / 2,
      cursor_x1 - cursor_width, cursor_y - cursor_height / 2,
      cursor_x1, cursor_y);
    sprite.pushSprite(0, 0);
  } else {
    sprite.fillScreen(background.to24Bit());
    float dt = timer.wrap(millis()) / 1000.;
#if SINGLE
    worm.draw(sprite, dt);
#else
    for (int i = 0; i < worms.size(); i++) {
      worms[worms.size() - 1 - i]->draw(sprite, dt); // 奥から描画
    }
#endif
    // FIXME : カーソル描画をクラス化/関数化して楽する
    sprite.setColor(TFT_DARKGRAY);
    {
      float offset = 4 * fabs(sin(millis() / 1000. * .5 * 2 * PI));
      float cursor_y1 = 1 + offset, cursor_x = 80;
      float cursor_width = 4, cursor_height = 4;
      sprite.fillTriangle(
        cursor_x, cursor_y1,
        cursor_x - cursor_width / 2, cursor_y1 + cursor_height,
        cursor_x + cursor_width / 2, cursor_y1 + cursor_height);
    }
    {
      float offset = - 4 * fabs(sin(millis() / 1000. * .5 * 2 * PI));
      float cursor_x1 = 159 + offset, cursor_y = 40;
      float cursor_width = 4, cursor_height = 4;
      sprite.fillTriangle(
        cursor_x1 - cursor_width, cursor_y + cursor_height / 2,
        cursor_x1 - cursor_width, cursor_y - cursor_height / 2,
        cursor_x1, cursor_y);
    }
    delay(1); // 描画色が変になるのを防ぐ
    sprite.pushSprite(0, 0);
  }
}

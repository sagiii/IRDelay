#define LGFX_AUTODETECT
#include <M5StickC.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include "ir_delay.hpp"

const uint32_t UART_BAUD = 115200;

// GFX
static LGFX lcd;
static LGFX_Sprite sprite(&lcd);

void draw(IRDelay &ir) {
  if (ir.is_on) {
    sprite.fillScreen(0x000000u);
    sprite.setColor(0xFFFFFFu);
    sprite.drawString("ON", 10, 10);
  } else {
    if (ir.on_count > 30) {
      sprite.fillScreen(0xFFFFFFu);
    } else {
      sprite.fillScreen(0x0000FFu);
    }
    sprite.setColor(0x000000u);
    sprite.drawString("OFF", 10, 10);
    sprite.drawNumber(ir.on_count, 10, 30);
  }
  sprite.pushSprite(0, 0);
}

IRDelay ir(draw);

void setup() {
  Serial.begin(UART_BAUD);
  M5.begin();
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(128);
  sprite.createSprite(M5.Lcd.height(), M5.Lcd.width()); // rotateしているので、幅と高さが入れ替わっている
  lcd.setColorDepth(24);
  sprite.setColorDepth(24);
  sprite.setFont(&fonts::lgfxJapanGothic_16);
  ir.turnOn();
}

void loop() {
  M5.update();
  if (M5.BtnA.wasReleased()) {
    ir.turnOff();
  }

  if (M5.BtnB.wasReleased()) {
    ir.turnOn();
  }

  ir.update(millis());
}

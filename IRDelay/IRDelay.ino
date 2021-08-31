#define LGFX_AUTODETECT
#include <M5StickC.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>

// IR
const uint32_t UART_BAUD = 115200;
const int PIN_IR_TX = 32;
const int IR_FREQ = 38; // [kHz]

IRsend ir_send(PIN_IR_TX);

const uint16_t irRawDataOn[] = {2596, 2616,  862, 818,  858, 816,  862, 1850,  862, 1850,  864, 1848,  862, 1850,  862, 818,  860, 816,  860};  // UNKNOWN 8F0C8B3A
const uint16_t irRawDataOff[] = {2594, 2618,  862, 818,  862, 1848,  862, 816,  860, 818,  860, 1848,  862, 818,  860, 1848,  862, 1850,  864};  // UNKNOWN D410BC59

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))

// GFX
static LGFX lcd;
static LGFX_Sprite sprite(&lcd);


bool is_on = true;
const uint16_t ON_DELAY = 300;
//const uint16_t ON_DELAY = 40;
uint16_t on_count = 0;

void draw(bool is_on, uint16_t on_count) {
  if (is_on) {
    sprite.fillScreen(0x000000u);
    sprite.setColor(0xFFFFFFu);
    sprite.drawString("ON", 10, 10);
  } else {
    if (on_count > 30) {
      sprite.fillScreen(0xFFFFFFu);
    } else {
      sprite.fillScreen(0x0000FFu);
    }
    sprite.setColor(0x000000u);
    sprite.drawString("OFF", 10, 10);
    sprite.drawNumber(on_count, 10, 30);
  }
  sprite.pushSprite(0, 0);
}

void sendIr(bool is_on) {
  if (is_on) {
    ir_send.sendRaw(irRawDataOn, SIZEOF(irRawDataOn), IR_FREQ);
  } else {
    ir_send.sendRaw(irRawDataOff, SIZEOF(irRawDataOff), IR_FREQ);
  }
}


void setup() {
  Serial.begin(UART_BAUD);
  M5.begin();
  ir_send.begin();
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(128);
  sprite.createSprite(M5.Lcd.height(), M5.Lcd.width()); // rotateしているので、幅と高さが入れ替わっている
  lcd.setColorDepth(24);
  sprite.setColorDepth(24);
  sendIr(is_on);
  sprite.setFont(&fonts::lgfxJapanGothic_16);
  draw(is_on, on_count);
}

void turn_on() {
  // turn on, stop count down
  is_on = true;
  sendIr(is_on);
  draw(is_on, on_count);
}

void turn_off() {
  // turn off, start count down
  is_on = false;
  on_count = ON_DELAY;
  sendIr(is_on);
  draw(is_on, on_count);
}

void loop() {
  M5.update();
  if (M5.BtnA.wasReleased()) {
    turn_off();
  }

  if (M5.BtnB.wasReleased()) {
    turn_on();
  }

  {
    static uint32_t sec0 = 0;
    uint32_t sec1 = millis() / 1000;
    if (sec1 != sec0) {
      sec0 = sec1;
      // process once per second
      if ((!is_on) && on_count > 0) {
        on_count --;
        draw(is_on, on_count);
        if (on_count == 0) {
          turn_on();
        }
      }
    }
  }
}

#pragma once

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>

#include <stdint.h>

const int PIN_IR_TX = 32;
const int IR_FREQ = 38; // [kHz]
const uint16_t ON_DELAY = 300; // [sec]
const uint16_t irRawDataOn[] = {2596, 2616,  862, 818,  858, 816,  862, 1850,  862, 1850,  864, 1848,  862, 1850,  862, 818,  860, 816,  860};  // UNKNOWN 8F0C8B3A
const uint16_t irRawDataOff[] = {2594, 2618,  862, 818,  862, 1848,  862, 816,  860, 818,  860, 1848,  862, 818,  860, 1848,  862, 1850,  864};  // UNKNOWN D410BC59

#define SIZEOF(a) (sizeof(a) / sizeof(a[0]))

struct IRDelay;
typedef void (*cb_updated_t)(IRDelay&);

struct IRDelay {
    IRsend ir_send;
    bool is_on;
    uint16_t on_count;
    cb_updated_t cb_updated;
    IRDelay(cb_updated_t cb_updated_ = 0)
        : ir_send(PIN_IR_TX)
        , is_on(true)
        , on_count(0)
        , cb_updated(cb_updated_)
        {
            ir_send.begin();
        }
    void send() {
        if (is_on) {
            ir_send.sendRaw(irRawDataOn, SIZEOF(irRawDataOn), IR_FREQ);
        } else {
            ir_send.sendRaw(irRawDataOff, SIZEOF(irRawDataOff), IR_FREQ);
        }
    }
    void turnOn() {
        is_on = true;
        send();
        if (cb_updated) cb_updated(*this);
    }
    void turnOff(uint16_t on_delay = ON_DELAY) {
        is_on = false;
        on_count = on_delay;
        send();
        if (cb_updated) cb_updated(*this);
    }
    /**
     * 更新関数。1秒に1回は呼ぶこと。
     */
    void update(unsigned long ms) {
        static uint32_t sec0 = 0; // FIXME : static非推奨（インスタンスが1つなので大丈夫だけど）
        uint32_t sec1 = ms / 1000;
        if (sec1 != sec0) {
            sec0 = sec1;
            // process once per second
            if ((!is_on) && on_count > 0) {
                on_count --;
                if (cb_updated) cb_updated(*this);
                if (on_count == 0) {
                    turnOn();
                }
            }
        }
    }
};

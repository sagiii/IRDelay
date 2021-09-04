#pragma once

#include <stdint.h>

struct Color24Bit {
    union {
        uint32_t u24;
        struct {
            uint8_t b, g, r; // for LovyanGFX
        };
    };
};

struct Color {
    uint8_t r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {}
    /**
     * mix this with color2.
     * ratio = 0 : the same as this
     * ratio = 1 : the same as color2
     */
    Color mix(Color &color2, float ratio2) {
        return Color(
            this->r * (1 - ratio2) + color2.r * ratio2,
            this->g * (1 - ratio2) + color2.g * ratio2,
            this->b * (1 - ratio2) + color2.b * ratio2
        );
    }
    uint32_t to24Bit() {
        Color24Bit c;
        c.r = r;
        c.g = g;
        c.b = b;
        return c.u24;
    }
};

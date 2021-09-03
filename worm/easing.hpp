#pragma once 

#include "arduino.hpp"

using namespace std;

struct Easing {
    bool in, out;
    enum POLY {
        LINE = 1, QUAD, CUBIC, QUART
    } poly;
    Easing() : in(false), out(false), poly(LINE) {}
    ~Easing() {}
    float v(float t) {
        if (t < 0) {
            return 0;
        } else if (t > 1) {
            return 1;
        }
        if ((!in && !out) || poly == LINE) {
            return t;
        }
        switch(poly) {
            case QUAD:
            if (in && !out) {
                return t * t;
            } else if (!in && out) {
                return - t * t + 2 * t;
            } else if (in && out) {
                return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
            }
            break;
            case CUBIC:
            if (in && !out) {
                return t * t * t;
            } else if (!in && out) {
                return 1 - pow(1 - t, 3);
            } else if (in && out) {
                return t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
            }
            break;
            case QUART:
            if (in && !out) {
                return t * t * t * t;
            } else if (!in && out) {
                return 1 - pow(1 - t, 4);
            } else if (in && out) {
                return t < 0.5 ? 8 * t * t * t * t : 1 - pow(-2 * t + 2, 4) / 2;
            }
            break;
            default:
            break;
        }
        return t;
    }
};

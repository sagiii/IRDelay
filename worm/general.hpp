#pragma once

#ifndef ESP32
#include "arduino.hpp"
#endif

float frand()
{
    return random(0, 1024) / 1023.;
}

float fmap(float x, float x0, float x1, float y0, float y1)
{
    return (x - x0) / (x1 - x0) * (y1 - y0) + y0;
}

/**
 * 正規化されたcosine
 */
float cos_n(float x)
{
    return cos(x) / 2 + .5;
}

float rad2deg(float rad)
{
    return 180 * rad / PI;
}

float deg2rad(float deg)
{
    return deg / 180 * PI;
}

struct Timer {
  unsigned long t0;
  Timer() : t0(0) {}
  Timer(unsigned long t0_) : t0(t0_) {}
  unsigned long wrap(unsigned long t1) {
    unsigned long wrap_time = t1 - t0;
    t0 = t1;
    return wrap_time;
  }
};

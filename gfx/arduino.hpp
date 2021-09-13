#pragma once

#include <cmath>
#include <stdlib.h>

float map(float x, float x0, float x1, float y0, float y1)
{
    return (x - x0) / (x1 - x0) * (y1 - y0) + y0;
}

const float PI = 3.14159265;

long random(long x0, long x1)
{
    return x0 + rand() % (x1 - x0);
}

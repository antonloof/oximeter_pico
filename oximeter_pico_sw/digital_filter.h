#pragma once

#include "pico/stdlib.h"

typedef struct filter
{
    float *b, *a, *z, *x;
    uint blen, alen;

} filter;

float filter_process(float x, filter *f);

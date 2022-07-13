#include "digital_filter.h"

float filter_process(float x, filter *f)
{
    float res = 0;
    for (int i = 0; i < f->blen - 1; i++)
    {
        f->x[i + 1] = f->x[i];
    }
    f->x[0] = x;
    for (int i = 0; i < f->alen; i++)
    {
        res -= f->a[i] * f->z[i];
    }
    for (int i = 0; i < f->blen; i++)
    {
        res += f->b[i] * f->x[i];
    }
    for (int i = 0; i < f->alen - 1; i++)
    {
        f->z[i + 1] = f->z[i];
    }
    f->z[0] = res;
    return res;
}
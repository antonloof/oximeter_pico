#pragma once

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"

#include "7seg.pio.h"

#define DISPLAY_COUNT_MAX (32 - 7)
#define SEGMENT_COUNT_MAX 8

typedef struct seg7_state
{

    uint32_t pin_order[32]; // pin start_pin + i is connected to function start_pin + i
    // the first dig_count functions are connected to the common pins for the digits the latter seg_count are connected to segment g-a + decimal point
    // .pin_order = {d0, d1, d2, d3 .., g,f,e,d,c,b,a,dp} replace letters with offsets from pin start_pin
    uint32_t seg_count; // 8 if decimal point is connected else 7
    uint32_t dig_count; // number of digits connected max is 32-seg_count
    uint32_t start_pin;

    uint32_t buf_1[DISPLAY_COUNT_MAX], buf_2[DISPLAY_COUNT_MAX];
    uint32_t *reading, *writing;
    uint sm;
    uint dma_c;
    PIO pio;
    uint swap_buffers;
    void (*irq)(void);

} seg7_state;

void seg7_init(seg7_state *state);
void seg7_display(uint score, uint leading_zero, seg7_state *state);
void seg7_dma_irq_handler(seg7_state *state); // should be called in irq method of seg7_state needed cus iäm bad at c
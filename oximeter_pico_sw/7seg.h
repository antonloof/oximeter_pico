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
    uint32_t dig_i_to_display_i[DISPLAY_COUNT_MAX]; // digit i is connected dig_order[i] pins after the first digit
    uint32_t seg_order[SEGMENT_COUNT_MAX];          // segment i (a-g+dot) is connected seg_order[i] pins after the first segment
    uint32_t seg_count;                             // 8 if decimal point is connected else 7
    uint32_t dig_count;                             // number of digits connected max is 32-seg_count
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
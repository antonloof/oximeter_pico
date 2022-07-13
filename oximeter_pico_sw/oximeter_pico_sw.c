#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "7seg.h"
#include "hardware/adc.h"
#include "digital_filter.h"
#include "heartbeat_detector.h"

#define SAMPLE_RATE_HZ 1000.0f
#define HZ_TO_BPM 60.0f

void seven_seg_irq();

seg7_state seven_seg = {
    .dig_count = 4,
    .seg_count = 4,
    .seg_order = {0, 1, 2, 3},
    .dig_i_to_display_i = {0, 1, 2, 3},
    .pio = pio0,
    .start_pin = 4,
    .irq = &seven_seg_irq};

void seven_seg_irq()
{
    seg7_dma_irq_handler(&seven_seg);
}

bool repeating_timer_callback(struct repeating_timer *t);
void init_sampling(uint pin);
void process_sample(float sample);
volatile bool should_sample;

float fb[3] = {0.007792936291952f, 0.0f, -0.007792936291952f};
float fa[2] = {-1.984355370350682f, 0.984414127416097f};
float fx[3], fz[2];

filter input_filter = {
    .a = fa,
    .b = fb,
    .alen = 2,
    .blen = 3,
    .x = fx,
    .z = fz};

repeating_timer_t timer;
void new_heartrate_callback(uint32_t *periods);
heartbeat_detector hb = {
    .peak_to_peak_filter = 0.9995,
    .callback = new_heartrate_callback};

int main()
{
    init_sampling(26);
    seg7_init(&seven_seg);
    seg7_display(1234, false, &seven_seg);
    add_repeating_timer_ms(-1, repeating_timer_callback, NULL, &timer);

    while (1)
    {
        if (should_sample)
        {
            process_sample(adc_read());
        }
    }

    return 0;
}

void init_sampling(uint pin)
{
    should_sample = false;
    adc_init();
    adc_gpio_init(pin);
    adc_select_input(pin - 26);
    heartbeat_detector_init(&hb);
}

bool repeating_timer_callback(struct repeating_timer *t)
{
    should_sample = true;
    return true; // continue repeating timer :D
}

void process_sample(float sample)
{
    float filtered = filter_process((float)sample, &input_filter);
    heartbeat_detector_sample(filtered, &hb);
}

void new_heartrate_callback(uint32_t *periods)
{
    seg7_display((uint)round(SAMPLE_RATE_HZ / (float)periods[0] * HZ_TO_BPM), false, &seven_seg);
}
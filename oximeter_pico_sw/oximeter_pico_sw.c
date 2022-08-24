#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "7seg.h"
#include "hardware/adc.h"
#include "digital_filter.h"
#include "heartbeat_detector.h"

#define SAMPLE_RATE_HZ 1000.0f
#define HZ_TO_BPM 60.0f

#define PHOTODIODE_IR_PIN 28
#define PHOTODIODE_BB_PIN 27

#define DISPLAY_BASE_PIN 11

#define RED_LED_PIN 8
#define GREEN_LED_PIN 9
#define IR_LED_PIN 7

void seven_seg_irq();

seg7_state seven_seg = {
    .dig_count = 4,
    .seg_count = 8,
    .pin_order = {3, 2, 1, 0, 8, 11, 10, 9, 7, 5, 4, 6},
    .pio = pio0,
    .start_pin = DISPLAY_BASE_PIN,
    .irq = &seven_seg_irq};

void seven_seg_irq()
{
    seg7_dma_irq_handler(&seven_seg);
}

bool repeating_timer_callback(struct repeating_timer *t);
void init_sampling(uint pin);
void process_sample(float sample);
volatile bool should_sample;

// second order BW bandpass from 0.1 to 3 hz at a sample rate of 1kS/s
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
    stdio_init_all();
    init_sampling(PHOTODIODE_BB_PIN);
    seg7_init(&seven_seg);
    // enable the ir led for now, we need to use all leds to measure oxygen content of the blood
    // pulse oximetry is left as an exercise to the reader
    gpio_set_drive_strength(GREEN_LED_PIN, GPIO_DRIVE_STRENGTH_12MA);
    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, true);
    gpio_put(GREEN_LED_PIN, 1);

    add_repeating_timer_ms(-1, repeating_timer_callback, NULL, &timer);

    while (1)
    {
        if (should_sample)
        {
            process_sample((float)adc_read());
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
    printf("%f %f\n", filtered, sample);
    heartbeat_detector_sample(filtered, &hb);
    should_sample = false;
}

void new_heartrate_callback(uint32_t *periods)
{

    seg7_display((uint)round(SAMPLE_RATE_HZ / (float)periods[0] * HZ_TO_BPM), false, &seven_seg);
}
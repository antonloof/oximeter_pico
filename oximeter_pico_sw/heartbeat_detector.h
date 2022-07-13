#pragma once

#include "math.h"
#include "pico/stdlib.h"

#define HEARTBEAT_PERIOD_LEN 5

typedef struct heartbeat_detector
{
    float last_sample, last_last_sample;
    float min_value, max_value;
    float peak_to_peak_filter;
    uint32_t highest_peak_i;
    float highest_peak;
    float lowest_valley_after, lowest_valley_before;
    bool has_peak;
    uint32_t next_sample_i;
    uint32_t last_peak_i;
    uint32_t heartbeat_period[HEARTBEAT_PERIOD_LEN];
    void (*callback)(uint32_t *);
} heartbeat_detector;

void heartbeat_detector_init(heartbeat_detector *hb);
void heartbeat_detector_sample(float sample, heartbeat_detector *hb);
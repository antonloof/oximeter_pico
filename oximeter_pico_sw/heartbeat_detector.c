#include "heartbeat_detector.h"

float filter_for_peak_threshold(float sample, heartbeat_detector *hb);

void heartbeat_detector_init(heartbeat_detector *hb)
{
    hb->has_peak = false;
    hb->highest_peak = -INFINITY;
    hb->highest_peak_i = 0;
    hb->lowest_valley_after = INFINITY;
    hb->lowest_valley_before = INFINITY;
    hb->last_peak_i = 0;
}

void heartbeat_detector_sample(float sample, heartbeat_detector *hb)
{
    float peak_th = filter_for_peak_threshold(sample, hb);
    hb->lowest_valley_after = MIN(hb->lowest_valley_after, sample);
    if (!hb->has_peak)
    {
        hb->lowest_valley_before = MIN(hb->lowest_valley_before, sample);
    }

    // fill last and last last sample
    if (hb->next_sample_i > 1)
    {
        if (hb->last_sample > sample && hb->last_sample > hb->last_last_sample)
        {
            if (hb->highest_peak < hb->last_sample && hb->lowest_valley_before + peak_th < hb->last_sample)
            {
                hb->highest_peak = hb->last_sample;
                hb->highest_peak_i = hb->next_sample_i - 1;
                hb->has_peak = true;
                hb->lowest_valley_before = MIN(hb->lowest_valley_after, hb->lowest_valley_before);
            }
            hb->lowest_valley_after = INFINITY;
        }
        else if (hb->has_peak)
        {
            if (hb->lowest_valley_before + peak_th < hb->highest_peak)
            {
                if (hb->lowest_valley_after + peak_th < hb->highest_peak)
                {
                    hb->highest_peak = -INFINITY;
                    hb->lowest_valley_before = INFINITY;
                    hb->has_peak = false;
                    uint32_t peak_period = hb->last_peak_i - hb->highest_peak_i;
                    hb->last_peak_i = hb->highest_peak_i;
                    for (int i = 1; i < HEARTBEAT_PERIOD_LEN; i++)
                    {
                        hb->heartbeat_period[i] = hb->heartbeat_period[i - 1];
                    }
                    hb->heartbeat_period[0] = peak_period;
                    hb->callback(hb->heartbeat_period);
                }
            }
        }
    }

    hb->last_last_sample = hb->last_sample;
    hb->last_sample = sample;
    hb->next_sample_i++;
}

float filter_for_peak_threshold(float sample, heartbeat_detector *hb)
{
    hb->min_value = MIN(hb->min_value * hb->peak_to_peak_filter, sample);
    hb->max_value = MAX(hb->max_value * hb->peak_to_peak_filter, sample);

    return (hb->max_value - hb->min_value) * 0.5;
}

#include "7seg.h"

uint8_t dig_to_segments[] = {
    0b1111110, // 0
    0b0110000, // 1
    0b1101101, // 2
    0b1111001, // 3
    0b0110011, // 4
    0b1011011, // 5
    0b1011111, // 6
    0b1110000, // 7
    0b1111111, // 8
    0b1111011, // 9
};

void seg7_init_dma(seg7_state *state);
void seg7_init_dma_buf(uint32_t *buf);
void seg7_init_pio(seg7_state *state);
void scoreboard_dma_irq_handler();
uint32_t update_bit_order(uint32_t in, uint32_t *from_to, uint32_t len);

void seg7_init(seg7_state *state)
{
    state->swap_buffers = 0;
    state->reading = state->buf_1;
    state->writing = state->buf_2;
    seg7_init_dma(state);
    seg7_init_dma_buf(state->reading);
    seg7_init_dma_buf(state->writing);
    seg7_init_pio(state);

    state->irq(); // start it all
}

uint32_t update_bit_order(uint32_t in, uint32_t *from_to, uint32_t len)
{
    uint32_t res = 0;
    for (int i = 0; i < len; i++)
    {
        int val = (in & (1 << i)) ? 1 : 0;
        res |= val << from_to[i];
    }
    return res;
}

void seg7_display(uint score, uint leading_zero, seg7_state *state)
{
    uint32_t clear_segments_mask = ((1 << (state->dig_count + 1)) - 1) << state->seg_count;
    for (int i = 0; i < DISPLAY_COUNT_MAX; i++)
    {
        state->writing[state->dig_i_to_display_i[i]] &= clear_segments_mask; // clear segments
        if (score || !i || leading_zero)
        {
            uint32_t segments = dig_to_segments[score % 10];
            state->writing[state->dig_i_to_display_i[i]] |= update_bit_order(segments, state->seg_order, SEGMENT_COUNT_MAX);
        }
        score /= 10;
    }
    state->swap_buffers = 1;
}

void seg7_init_pio(seg7_state *state)
{
    state->sm = pio_claim_unused_sm(state->pio, true);

    uint16_t seven_segment_program_instructions[1] = {
        0x7f00 | ((state->seg_count + state->dig_count) & 0b11111), //  0: out    pins, seg_count+dig_count               [31]
    };
    struct pio_program seven_segment_program = {
        .instructions = seven_segment_program_instructions,
        .length = 2,
        .origin = -1,
    };

    uint offset = pio_add_program(state->pio, &seven_segment_program);

    for (uint i = 0; i < state->seg_count + state->dig_count; i++)
    {
        pio_gpio_init(state->pio, state->start_pin + i);
    }

    pio_sm_config conf = seven_segment_program_get_default_config(offset);
    pio_sm_set_consecutive_pindirs(state->pio, state->sm, state->start_pin, state->seg_count + state->dig_count, true);

    sm_config_set_out_pins(&conf, state->start_pin, state->seg_count + state->dig_count);
    sm_config_set_out_shift(&conf, true, true, state->seg_count + state->dig_count);
    sm_config_set_clkdiv(&conf, 48); // run it quite slowly

    pio_sm_init(state->pio, state->sm, offset, &conf);
    pio_sm_set_enabled(state->pio, state->sm, true);
}

void seg7_dma_irq_handler(seg7_state *state)
{
    dma_hw->ints1 = 1u << state->dma_c; // clear interrupt
    if (state->swap_buffers)
    {
        uint32_t *tmp = state->reading;
        state->reading = state->writing;
        state->writing = tmp;
        state->swap_buffers = 0;
    }
    dma_channel_set_read_addr(state->dma_c, state->reading, true); // retrigger dma
}

void seg7_init_dma_buf(uint32_t *buf)
{
    for (uint i = 0; i < DISPLAY_COUNT_MAX; i++)
    {
        buf[i] = 1 << (SEGMENT_COUNT_MAX + i);
    }
}

void seg7_init_dma(seg7_state *state)
{
    state->dma_c = dma_claim_unused_channel(true);
    dma_channel_config conf = dma_channel_get_default_config(state->dma_c);

    channel_config_set_transfer_data_size(&conf, DMA_SIZE_32);
    channel_config_set_dreq(&conf, DREQ_PIO1_TX0 * pio_get_index(state->pio) + state->sm);
    dma_channel_set_irq1_enabled(state->dma_c, true);

    irq_set_enabled(DMA_IRQ_1, true);
    irq_set_exclusive_handler(DMA_IRQ_1, state->irq);
    dma_channel_configure(state->dma_c, &conf, &state->pio->txf[state->sm], NULL, state->dig_count, false);
}

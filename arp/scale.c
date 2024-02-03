#include "scale.h"
#include "arp_config.h"
#include <cstdint>

/*****
 * Private Constants
 *****/
#define SCALE_MAX_FREQ 1500

/*****
 * Definitions
 *****/
void scale_init(scale_t *self)
{
    float  freq = 36.0f;
    float  third;
    size_t i = 0;
    while(i < SCALE_SIZE)
    {
        while(freq > SCALE_MAX_FREQ)
        {
            freq *= 0.5f;
        }
        self->scale[i] = freq;
        i++;
        third = freq * 1.25f;
        while(third > SCALE_MAX_FREQ)
        {
            third *= 0.5f;
        }
        self->scale[i] = third;
        i++;
        freq *= 1.5;
    }
}

float scale_freq_at(scale_t *self, uint8_t idx) {
    return self->scale[idx];
}

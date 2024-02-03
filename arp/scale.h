#ifndef ARP_SCALE_H_
#define ARP_SCALE_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "arp_config.h"
#include <stdint.h>

    typedef struct scale
    {
        float scale[SCALE_SIZE];
    } scale_t;

    void  scale_init(scale_t *self);
    float scale_freq_at(scale_t *self, uint8_t idx);

#ifdef __cplusplus
}
#endif

#endif /* ARP_SCALE_H_ */

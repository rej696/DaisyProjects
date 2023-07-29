#ifndef ARP_ARP_H_
#define ARP_ARP_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef struct arp arp_t;
    typedef struct arp *arp_handle_t;

    typedef enum arp_direction
    {
        ARP_DIR_FWD,
        ARP_DIR_REV,
    } arp_direction_t;

    arp_handle_t arp_get(void);
    void arp_init(arp_t *self);
    void arp_note_on(arp_t *self, uint8_t num, uint8_t vel);
    void arp_note_off(arp_t *self, uint8_t num);
    void arp_set_note_on(arp_t self, void (*note_on_cb)(uint8_t, uint8_t));
    void arp_set_note_off(arp_t self, void (*note_off_cb)(uint8_t));
    void arp_set_direction(arp_t self, arp_direction_t direction);
    void arp_set_rand_chance(arp_t *self, float rand_chance);
    void arp_trigger(arp_t *self);
    void arp_clear(arp_t *self);

#ifdef __cplusplus
}
#endif

#endif /* ARP_ARP_H_ */

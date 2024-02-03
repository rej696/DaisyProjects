#ifndef ARP_ARP_H_
#define ARP_ARP_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "arp_config.h"
#include <stdint.h>

    typedef enum arp_direction
    {
        ARP_DIR_FWD,
        ARP_DIR_REV,
    } arp_direction_t;

    struct arp_note
    {
        uint8_t num;
        uint8_t vel;
        uint8_t next;
        uint8_t prev;
    };

    typedef struct arp
    {
        struct arp_note notes[NOTES_COUNT + 1];
        uint8_t         input_order[NOTES_COUNT];
        arp_direction_t direction;
        float           rand_chance;
        uint8_t         note_length;
        uint8_t         bottom_idx;
        uint8_t         current_idx;
        uint8_t         pulse_counter;
        uint8_t         size;
        void (*note_on_cb)(uint8_t, uint8_t);
        void (*note_off_cb)(uint8_t);
    } arp_t;

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

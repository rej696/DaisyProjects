#include "arp.h"
#include "arp_config.h"
#include <cstdint>
#include <string.h>

/*****
 * Private Constants
 *****/
#define ARP_SENTINEL 0xFF
#define ARP_EMPTY 0xFE
#define ARP_UNLINKED 0xFD

/*****
 * Private Types
 *****/
struct arp_note
{
    uint8_t num;
    uint8_t vel;
    uint8_t next;
    uint8_t prev;
};

struct arp
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
};

arp_t g_arp;

/*****
 * Static Functions
 *****/
static void    arp_remove_note(arp_t *self, uint8_t idx);
static uint8_t arp_next_note_idx(arp_t *self);
static uint8_t arp_prev_note_idx(arp_t *self);
static uint8_t arp_calc_note_len(arp_t *self);
static float   arp_rand_float(float min, float max);
static int     arp_rand_int(int min, int max);

/*****
 * Definitions
 *****/
arp_handle_t arp_get() {
    return &g_arp;
}

void arp_init(arp_t *self)
{
    self->note_length   = 0;
    self->bottom_idx    = 0;
    self->current_idx   = 0;
    self->pulse_counter = 0;
    self->size          = 0;
    self->rand_chance   = 0;
    self->direction     = ARP_DIR_FWD;
    self->note_on_cb    = NULL;
    self->note_off_cb   = NULL;
    arp_calc_note_len(self);
    arp_clear(self);
}

void arp_note_on(arp_t *self, uint8_t num, uint8_t vel)
{
    uint8_t slot = 1;
    while(slot <= NOTES_COUNT)
    {
        if(self->notes[slot].num == ARP_EMPTY)
        {
            break;
        }
        slot++;
    }

    if(slot > NOTES_COUNT)
    {
        slot = self->input_order[0];
        arp_remove_note(self, slot);
    }

    uint8_t idx = self->bottom_idx;
    while(self->notes[idx].num < num)
    {
        idx = self->notes[idx].next;
    }

    if(idx == self->bottom_idx)
    {
        self->bottom_idx = slot;
    }

    self->notes[slot].num                   = num;
    self->notes[slot].vel                   = vel;
    self->notes[slot].next                  = idx;
    self->notes[slot].prev                  = self->notes[idx].prev;
    self->notes[self->notes[idx].prev].next = slot;
    self->notes[idx].prev                   = slot;

    self->input_order[self->size - 1] = slot;
    self->size++;
}

void arp_note_off(arp_t *self, uint8_t num)
{
    uint8_t idx = 0;
    for(uint8_t i = 0; i < self->size; ++i)
    {
        if(self->notes[i].num == num)
        {
            idx = i;
            break;
        }
    }
    if(idx != 0)
    {
        arp_remove_note(self, idx);
    }
}

void arp_set_note_on(arp_t self, void (*note_on_cb)(uint8_t, uint8_t))
{
    self.note_on_cb = note_on_cb;
}

void arp_set_note_off(arp_t self, void (*note_off_cb)(uint8_t))
{
    self.note_off_cb = note_off_cb;
}

void arp_set_direction(arp_t self, arp_direction_t direction)
{
    self.direction = direction;
}

void arp_set_rand_chance(arp_t *self, float rand_chance)
{
    self->rand_chance = rand_chance;
}

void arp_trigger(arp_t *self)
{
    if(self->size <= 1)
        return;

    if((self->pulse_counter == self->note_length) && self->current_idx > 0)
    {
        self->note_off_cb(self->notes[self->current_idx].num);
    }

    self->pulse_counter++;
    if(self->pulse_counter <= (PPQN / 4))
        return;
    self->pulse_counter = 0;

    uint8_t note_idx;
    switch(self->direction)
    {
        case ARP_DIR_FWD:
        {
            note_idx = arp_next_note_idx(self);
            break;
        }
        case ARP_DIR_REV:
        {
            note_idx = arp_prev_note_idx(self);
            break;
        }
    }

    if(self->rand_chance > 0.05 && self->rand_chance < 0.95)
    {
        float rnd = arp_rand_float(0.0, 1.0);
        if(rnd <= self->rand_chance)
        {
            note_idx = arp_rand_int(1, self->size - 1);
        }
    }

    self->current_idx = note_idx;

    self->note_on_cb(self->notes[note_idx].num, self->notes[note_idx].vel);
}


void arp_clear(arp_t *self)
{
    (void)memset(self->input_order, 0, sizeof(uint8_t) * NOTES_COUNT);
    self->notes[0].num  = ARP_SENTINEL;
    self->notes[0].next = 0;
    self->notes[0].prev = 0;
    self->size          = 1;

    for(uint8_t i = self->size; i <= NOTES_COUNT; ++i)
    {
        self->notes[i].num  = ARP_EMPTY;
        self->notes[i].next = ARP_UNLINKED;
        self->notes[i].prev = ARP_UNLINKED;
    }
}

/****
 * Static Definitions
 *****/
static void arp_remove_note(arp_t *self, uint8_t idx)
{
    // TODO
}

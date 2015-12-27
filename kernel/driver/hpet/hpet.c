#include "hpet.h"

uint64_t hpet_addr = 0;

void process_hpet(hpet_t *hpet) {
    log("HPET #%d:%d, located at %x, ticking at %d", hpet->hpet_number, hpet->comparator_count, hpet->address.address, hpet->minimum_tick);

    // Although there might by multiple HPET, we only use one
    hpet_addr = hpet->address.address;

    uint64_t gcidr = * (uint64_t *) hpet_addr;
    log("%d counters available, period time %d (fs)", (gcidr >> 8) & 0x1f, (gcidr >> 32) & 0xffffffff);
}
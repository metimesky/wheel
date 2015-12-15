#include "timer.h"

volatile uint64_t tick = 0UL;

void busy_wait(int msec) {
    uint64_t end = tick + msec;
    while(tick < end) {}
}
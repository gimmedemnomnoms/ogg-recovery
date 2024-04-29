#ifndef __longblock__
#define __longblock__
#include <stdint.h>

struct longblock {
    uint8_t contblock[4096];
} __attribute__((__packed__));
#endif


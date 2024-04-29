#ifndef __shortblock__
#define __shortblock__
#include <stdint.h>

struct shortblock {
    uint32_t devdata[1024];
} __attribute__((__packed__));
#endif


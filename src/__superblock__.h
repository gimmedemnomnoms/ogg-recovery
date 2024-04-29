#ifndef __superblock__
#define __superblock__
#include <stdint.h>

struct superblock {
    uint32_t numinodes;
    uint32_t numblocks;
    uint32_t something[4];
    uint32_t blocksize;
    uint32_t something2[6];
    uint16_t something3[2];
    uint16_t sig;
} __attribute__((__packed__));
#endif

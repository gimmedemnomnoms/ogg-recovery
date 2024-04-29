#ifndef __MBR__
#define __MBR__
#include <stdint.h>

typedef struct PartitionEntry {
    uint8_t status;
    uint8_t chsStart[3];
    uint8_t type;
    uint8_t chsEnd[3];
    uint32_t lbaBegin;
    uint32_t sectors;
} PartitionEntry;

struct Disk_MBR {
    uint8_t Code[446];
    PartitionEntry partitions[4];
    uint16_t signature;
} __attribute__((__packed__));
#endif

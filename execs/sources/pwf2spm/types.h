#ifndef PTR2TEX_TYPES_H
#define PTR2TEX_TYPES_H

#include <cstring>
#include <inttypes.h>
#include <vector>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef u8 byte;

struct tex0_data_t {
    uint64_t identifier;  // TEX0 Header
    uint64_t unk[3];      // needs research
    uint64_t clamp;       // CLAMP
    uint64_t reserved;    // needs research

    // Compare checker
    bool operator==(const tex0_data_t& other) const {
        return std::memcmp(this, &other, sizeof(tex0_data_t)) == 0;
    }
};

struct tex0pack_t {
    std::vector<tex0_data_t> texdata;
    std::vector<uint8_t> format;
};

#endif  // PTR2TEX_TYPES_H

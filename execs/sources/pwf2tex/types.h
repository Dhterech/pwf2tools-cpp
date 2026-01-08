#ifndef PTR2TEX_TYPES_H
#define PTR2TEX_TYPES_H

#include <inttypes.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

enum CLUTClampMode {
    Repeat = 0, // NÃO USAR VALOR DO CLAMP
    Clamp  = 1, // NÃO USAR VALOR DO CLAMP
    Region_Clamp = 2,
    Region_Repeat = 3
};

typedef u8 byte;

#endif  // PTR2TEX_TYPES_H

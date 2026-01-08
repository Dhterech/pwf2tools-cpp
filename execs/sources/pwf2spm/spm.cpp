#include "spm.h"
#include <cstdio>
#include <cstring>

namespace spm {
#define POLYGON_KEY u64(0xEEEEEEEEEEEEEEEE)
#define SPM_MAGIC u32(0x18DF540A)

bool check_install() {
    if (sizeof(polygonheader_t) != 0x70) {
        return false;
    }
    return true;
}

bool checkheader(const void *spm) {
    u32 magic = *(u32 *)(spm);
    if (SPM_MAGIC == magic) {
        return true;
    }
    return false;
}

int getpolygoncount(const void *spm, int len) {
    if (false == checkheader(spm)) return -1;
    const byte *bytes = (const byte *)(spm);
    int parsed = 8;
    int acc = 0;

    while ((parsed + 8) <= len) {
        u64 v = *(u64 *)(bytes + parsed);
        if (v == POLYGON_KEY) {
            acc += 1;
        }
        parsed += 0x10;
    }

    return acc;
}

bool getpolygonbyindex(const void *spm, int len, int index,
                       polygonheader_t **out_polygon) {
    if (!checkheader(spm)) return false;      // if not an spm file, go away
    const byte *bytes = (const byte *)(spm);  // get the spm bytes
    int parsed = 8;
    int acc = 0;

    while ((parsed + 8) <= len) {
        u64 tmpHeader = *(u64 *)(bytes + parsed);
        if (tmpHeader == POLYGON_KEY) {
            if (acc == index) {              // if index matches the current iteration
                *out_polygon = (polygonheader_t *)((bytes + parsed) - 0x68);  // get header from bytes + parsed and subtract from 0x68
                return true;
            }
            acc += 1;
            if (acc > index) {
                break;
            }
        }
        parsed += 0x10;  // increase parsed by 0x10
    }
    *out_polygon = nullptr;  // couldnt find it
    return false;            // nope
}

//u64 tex0frompolygon(polygonheader_t *polygon) { return *(u64 *)(polygon + 1); }
tex0_data_t tex0frompolygon(spm::polygonheader_t *polygon) {
    tex0_data_t data;
    std::memcpy(&data, (polygon + 1), sizeof(tex0_data_t));
    return data;
}

}

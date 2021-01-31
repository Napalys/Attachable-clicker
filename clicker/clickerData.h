#ifndef CLICKERDATA_H
#define CLICKERDATA_H

#include <cstdint>

struct clickerData {
    uint8_t key_code;
    uint32_t delay;
    bool longClick;

    clickerData(uint8_t key_code, uint32_t delay, bool isClickLong)
            : key_code{key_code}, delay{delay}, longClick(isClickLong) {};
};

#endif // CLICKERDATA_H

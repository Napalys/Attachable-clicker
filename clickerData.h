#ifndef CLICKERDATA_H
#define CLICKERDATA_H

#include <cstdint>

struct clickerData{
    uint8_t key_code;
    uint32_t delay;
    clickerData(uint8_t key_code,  uint32_t delay)
        : key_code{key_code}, delay{delay} {};
};

#endif // CLICKERDATA_H

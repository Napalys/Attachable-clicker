#ifndef CLICKERDATA_H
#define CLICKERDATA_H

#include <cstdint>

struct ClickerData {
    enum class Event { Pressed, Released, Unknown };
    uint8_t key_code;
    uint32_t delay;
    Event event;

    ClickerData() : key_code(0), delay(0), event(Event::Unknown) {}

    ClickerData(uint8_t key_code, uint32_t delay, Event e)
            : key_code{key_code}, delay{delay}, event{e} {};
};

#endif // CLICKERDATA_H

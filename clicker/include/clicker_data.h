#ifndef CLICKERDATA_H
#define CLICKERDATA_H

#include <cstdint>

struct ClickerData {
    enum class Event { Pressed, Released, Unknown };
    uint8_t key_code;
    uint32_t delay;
    Event event;
    std::string key_name;

    ClickerData() : key_code(0), delay(0), event(Event::Unknown) {}

    ClickerData(uint8_t key_code, uint32_t delay, Event e, std::string key_name)
            : key_code{key_code}, delay{delay}, event{e}, key_name{std::move(key_name)} {};
};

#endif // CLICKERDATA_H

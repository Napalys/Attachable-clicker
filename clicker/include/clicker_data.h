#ifndef CLICKERDATA_H
#define CLICKERDATA_H

#include <cstdint>
#include <iostream>

struct ClickerData {
    enum class Event { Pressed, Released, Unknown };
    uint8_t key_code;
    uint32_t delay;
    Event event;
    std::string key_name;

    ClickerData() : key_code(0), delay(0), event(Event::Unknown) {}

    ClickerData(uint8_t key_code, uint32_t delay, Event e, std::string key_name)
            : key_code{key_code}, delay{delay}, event{e}, key_name{std::move(key_name)} {};

    friend std::ostream& operator<<(std::ostream& os, const ClickerData& data) {
        os << "Key Name: " << data.key_name << " Key Code: " << static_cast<int>(data.key_code)
           << " Delay: " << data.delay << " Event: ";
        switch (data.event) {
            case Event::Pressed:
                os << "Pressed";
                break;
            case Event::Released:
                os << "Released";
                break;
            default:
                os << "Unknown";
                break;
        }
        os << std::endl;
        return os;
    }
};

#endif // CLICKERDATA_H

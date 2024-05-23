#ifndef CLICKERDATA_H
#define CLICKERDATA_H

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>

struct Delay{
    uint32_t delay;
    Delay() : delay(0) {}
    explicit Delay(uint32_t delay) : delay{delay} {}
    friend std::ostream& operator<<(std::ostream& os, const Delay& d) {
        os << "Delay: " << d.delay << std::endl;
        return os;
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Delay, delay)
};

struct ClickerData {
    enum class Event { Pressed, Released, Unknown };
    uint8_t key_code;
    Event event;
    std::string key_name;

    ClickerData() : key_code(0), event(Event::Unknown) {}

    ClickerData(uint8_t key_code, Event e, std::string key_name)
            : key_code{key_code}, event{e}, key_name{std::move(key_name)} {};

    friend std::ostream& operator<<(std::ostream& os, const ClickerData& data) {
        os << "Key Name: " << data.key_name << " Key Code: " << static_cast<int>(data.key_code)
           << " Event: ";
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

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClickerData, key_code, event, key_name)
};

struct KeyNameVisitor {
    auto inline operator()(const ClickerData& data) const {
        return data.key_name;
    }

    auto inline operator()(const Delay& data) const {
        (void)data;
        return std::string("Delay");
    }
};

#endif // CLICKERDATA_H

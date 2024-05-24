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

    NLOHMANN_JSON_SERIALIZE_ENUM(ClickerData::Event, {
        {ClickerData::Event::Unknown, "Unknown"},
        {ClickerData::Event::Pressed, "Pressed"},
        {ClickerData::Event::Released, "Released"}
    })
};

inline void to_json(nlohmann::json& j, const ClickerData& p) {
    j = nlohmann::json{
            {"type", "ClickerData"},
            {"key_name", p.key_name},
            {"key_code", p.key_code},
            {"event", p.event}
    };
}

inline void to_json(nlohmann::json& j, const Delay& d) {
    j = nlohmann::json{
            {"type", "Delay"},
            {"delay", d.delay}
    };
}

inline void from_json(const nlohmann::json& j, ClickerData& p) {
    j.at("key_name").get_to(p.key_name);
    j.at("key_code").get_to(p.key_code);
    j.at("event").get_to(p.event);
}

inline void from_json(const nlohmann::json& j, Delay& d) {
    j.at("delay").get_to(d.delay);
}

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

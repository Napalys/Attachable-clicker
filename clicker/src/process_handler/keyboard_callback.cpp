//
// Created by Widok on 5/20/24.
//
#include <stdexcept>
#include "process_handler/keyboard_callback.h"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <thread>

#ifdef WIN32
#include <windows.h>
#endif
#ifdef linux

#include <libevdev/libevdev.h>

#endif

namespace ProcessHandler {

    static KeyboardCallback callback = nullptr;
    static bool keyboard_registered = false;

#ifdef linux

    int open_keyboard_device() {
        const char *dir_name = "/dev/input";
        struct dirent *entry;
        DIR *dir = opendir(dir_name);
        if (!dir) return -1;

        while ((entry = readdir(dir)) != nullptr) {
            if (strncmp("event", entry->d_name, 5) != 0)
                continue;

            std::string dev_path = std::string(dir_name) + "/" + std::string(entry->d_name);
            int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
            if (fd < 0) continue;

            struct libevdev *dev;
            if (libevdev_new_from_fd(fd, &dev) < 0) {
                close(fd);
                continue;
            }

            if (libevdev_has_event_type(dev, EV_KEY) &&
                libevdev_has_event_code(dev, EV_KEY, KEY_Q) &&
                libevdev_has_event_code(dev, EV_KEY, KEY_P) &&
                libevdev_has_event_code(dev, EV_KEY, KEY_1) &&
                libevdev_has_event_code(dev, EV_KEY, KEY_0) &&
                libevdev_has_event_code(dev, EV_KEY, KEY_ENTER) &&
                libevdev_has_event_code(dev, EV_KEY, KEY_SPACE) &&
                libevdev_has_event_code(dev, EV_KEY, KEY_F1) &&
                std::string(libevdev_get_name(dev)).find("Keyboard") != std::string::npos &&
                std::string(libevdev_get_name(dev)).find("Mouse") == std::string::npos) {
                libevdev_free(dev);
                closedir(dir);
                return fd;
            }

            libevdev_free(dev);
            close(fd);
        }

        closedir(dir);
        return -1;
    }

    void startListening(int fd, struct libevdev *dev) {
        std::thread t([=]() {
            std::cout << "Listening to device: " << libevdev_get_name(dev) << std::endl;
            auto lastTime = std::chrono::high_resolution_clock::now();

            while (keyboard_registered) {
                struct input_event ev{};
                auto rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL | LIBEVDEV_READ_FLAG_BLOCKING, &ev);
                if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
                    if (ev.type == EV_KEY) {
                        ClickerData::Event event;
                        switch (ev.value) {
                            case 0:
                                event = ClickerData::Event::Released;
                                break;
                            case 1:
                                event = ClickerData::Event::Pressed;
                                break;
                            default:;
                                continue;
                        }
                        auto currentTime = std::chrono::high_resolution_clock::now();
                        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(
                                currentTime - lastTime).count();
                        lastTime = currentTime;
                        const char *key_name = libevdev_event_code_get_name(EV_KEY, ev.code);
                        std::string formatted_key_name = "Unknown";

                        if (key_name) {
                            formatted_key_name = key_name;
                            size_t prefix_length = 4;
                            if (formatted_key_name.find("KEY_") == 0) {
                                formatted_key_name = formatted_key_name.substr(prefix_length);
                            }
                        }
                        ClickerData clickerData(ev.code, static_cast<uint32_t>(delay), event, formatted_key_name);
                        callback(clickerData);
                    }
                }
            }

            libevdev_free(dev);
            close(fd);
            std::cout << "Closed" << std::endl;
        });

        t.detach();
    }

#endif
#ifdef WIN32
    HHOOK hKeyboardHook;
    std::chrono::time_point<std::chrono::steady_clock> lastKeyPressTime;

    std::string GetKeyName(DWORD vkCode) {
        char name[128];
        if (GetKeyNameTextA(MapVirtualKeyA(vkCode, 0) << 16, name, sizeof(name)) > 0) {
            return std::string(name);
        }
        return "Unknown";
    }

    LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN || wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                DWORD vkCode = pKeyBoard->vkCode;
                auto currentKeyPressTime = std::chrono::steady_clock::now();

                auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(currentKeyPressTime - lastKeyPressTime).count();
                const auto keyEvent = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) ? ClickerData::Event::Pressed : ClickerData::Event::Released;

                ClickerData clickerData(vkCode, static_cast<uint32_t>(delay), keyEvent, GetKeyName(vkCode));
                callback(clickerData);
                lastKeyPressTime = currentKeyPressTime;
            }
        }
        return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
    }

    void SetHook() {
        if (!(hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0))) {
           throw std::runtime_error("Failed to find keyboard, software might be missing admin privileges");
        }
    }

    void ReleaseHook() {
        UnhookWindowsHookEx(hKeyboardHook);
    }
#endif

    void registerCallBack(const KeyboardCallback &cb) {
        if (!keyboard_registered) {
#ifdef linux
            auto fd = open_keyboard_device();
            if (fd == -1)
                throw std::runtime_error("Failed to find keyboard, software might be missing admin privileges");
            struct libevdev *dev = nullptr;
            int rc = libevdev_new_from_fd(fd, &dev);
            if (rc < 0) {
                close(fd);
                throw std::runtime_error("Failed to find keyboard, software might be missing admin privileges");
            }
            startListening(fd, dev);
#endif
#ifdef WIN32
            lastKeyPressTime = std::chrono::steady_clock::now();
            SetHook();
            MSG msg;
#endif
            keyboard_registered = true;

        }
        callback = cb;
    }

    void removeCallBack() {
        callback = nullptr;
        keyboard_registered = false;
#ifdef WIN32
        ReleaseHook();
#endif
    }
}
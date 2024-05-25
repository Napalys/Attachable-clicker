//
// Created by Widok on 5/19/24.
//

#ifndef INJECTIONCLICKER_WINDOWS_PROCESS_MANAGER_H
#define INJECTIONCLICKER_WINDOWS_PROCESS_MANAGER_H
#ifdef _WIN32

#include <cstdint>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "process_manager.h"
#include "windows.h"

namespace ProcessHandler {
    class HoldKey{
    public:
        HoldKey(HWND hwnd, uint8_t key_code);

        ~HoldKey() noexcept;

        void startWorker();

        inline void setPressedState(bool state) noexcept;
    private:
        HWND hwnd;
        uint8_t key_code;
        std::thread thread;
        std::atomic<bool> pressed = true;
        std::atomic<bool> running = true;
        std::mutex mtx;
        std::condition_variable cv;
    };

    class WindowsProcessManager : public ProcessManager {
    public:
        WindowsProcessManager(uint32_t dwProcessID, const std::string &processName);

        ~WindowsProcessManager() noexcept override;

        void sendClick(ClickerData clicker_data) override;

        void stopAll();

    private:

        void startKey(uint8_t key_code);

        void stopKey(uint8_t key_code);


    private:
        HWND hwnd;
        std::unordered_map<uint8_t, HoldKey> keys;
    };

} // ProcessHandler
#endif
#endif //INJECTIONCLICKER_WINDOWS_PROCESS_MANAGER_H

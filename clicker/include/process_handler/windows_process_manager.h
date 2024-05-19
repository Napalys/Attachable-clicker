//
// Created by Widok on 5/19/24.
//

#ifndef INJECTIONCLICKER_WINDOWS_PROCESS_MANAGER_H
#define INJECTIONCLICKER_WINDOWS_PROCESS_MANAGER_H
#ifdef _WIN32
#include <cstdint>
#include <string>
#include "process_manager.h"
#include "windows.h"

namespace ProcessHandler {

    class WindowsProcessManager : public ProcessManager {
    public:
        WindowsProcessManager(uint32_t dwProcessID, const std::string& processName);

        void sendClick(ClickerData clicker_data) override;

    private:
        HWND hwnd;
    };

} // ProcessHandler
#endif
#endif //INJECTIONCLICKER_WINDOWS_PROCESS_MANAGER_H

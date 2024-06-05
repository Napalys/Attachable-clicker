//
// Created by Widok on 5/19/24.
//

#ifndef INJECTIONCLICKER_LINUX_PROCESS_MANAGER_H
#define INJECTIONCLICKER_LINUX_PROCESS_MANAGER_H
#ifdef linux
#include <string>
#include "process_manager.h"

namespace ProcessHandler {

    class LinuxProcessManager : public ProcessManager {
    public:
        LinuxProcessManager(uint32_t dwProcessID, const std::string& processName);

        void sendClick(ClickerData clicker_data) override;

        std::string takeScreenshot() override;
    };

} // ProcessHandler
#endif
#endif //INJECTIONCLICKER_LINUX_PROCESS_MANAGER_H

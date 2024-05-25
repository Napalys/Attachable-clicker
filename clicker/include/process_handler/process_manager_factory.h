//
// Created by Widok on 5/19/24.
//

#ifndef INJECTIONCLICKER_PROCESS_MANAGER_FACTORY_H
#define INJECTIONCLICKER_PROCESS_MANAGER_FACTORY_H

#include <memory>
#include "process_manager.h"
#include "linux_process_manager.h"
#include "windows_process_manager.h"

namespace ProcessHandler {
    static std::unique_ptr<ProcessManager> create_process_handler(uint32_t dwProcessID, const std::string& processName) {
#ifdef _WIN32
        return std::make_unique<WindowsProcessManager>(dwProcessID, processName);
#else
        return std::make_unique<LinuxProcessManager>(dwProcessID, processName);
#endif
    }
}
#endif //INJECTIONCLICKER_PROCESS_MANAGER_FACTORY_H

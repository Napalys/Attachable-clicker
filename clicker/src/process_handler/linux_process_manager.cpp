#ifdef linux

#include <stdexcept>
#include "process_handler/linux_process_manager.h"

namespace ProcessHandler {
    LinuxProcessManager::LinuxProcessManager(uint32_t dwProcessID, const std::string &processName) {
        throw std::invalid_argument("No implementation for linux system");
    }

    void LinuxProcessManager::sendClick(ClickerData clicker_data) {

    }
} // ProcessHandler
#endif
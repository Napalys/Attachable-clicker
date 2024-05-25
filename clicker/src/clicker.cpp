#include <mutex>
#include <iostream>
#include "clicker.h"
#include "process_handler/process_manager_factory.h"

static std::mutex m;

Clicker::Clicker(uint32_t dwProcessID, const std::string& processName) : process_manager(
        ProcessHandler::create_process_handler(dwProcessID, processName)) {
}

void Clicker::setClickerStatus(bool status) {
    this->clickerStatus = status;
}

bool Clicker::getClickerStatus() const {
    return clickerStatus;
}

void Clicker::addRoutine(std::vector<std::variant<ClickerData, Delay>> routine) {
    routines.emplace_back(process_manager, std::move(routine));
}

void Clicker::startRoutines() noexcept {
    for(auto& routine : routines) routine.startRoutine();
}

void Clicker::stopRoutines() noexcept {
    routines.clear();
#ifdef WIN32
    if (auto windows_manager = std::dynamic_pointer_cast<ProcessHandler::WindowsProcessManager>(
                process_manager); windows_manager) {
        windows_manager->stopAll();
    }
#endif
}

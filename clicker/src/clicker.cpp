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

void startClicking(const ClickerData &data, Clicker *clicker) {
    using namespace std::chrono_literals;
    while (clicker->getClickerStatus()) {
        m.lock();
        clicker->process_manager->sendClick(data);
        m.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(data.delay));
    }
}

void Clicker::initClickerThreads(std::vector<ClickerData> &data) {
    for (auto &d: data) {
        jobs.emplace_back(startClicking, d, this);
    }
}

void Clicker::destroyClickerThreads() {
    for (auto &thread: jobs) {
        thread.join();
    }
    jobs.clear();
}

void Clicker::addRoutine(std::vector<ClickerData> routine) {
    routines.emplace_back(process_manager, std::move(routine));
}

void Clicker::startRoutines() noexcept {
    for(auto& routine : routines) routine.startRoutine();
}

void Clicker::stopRoutines() noexcept {
    routines.clear();
}

#ifndef CLICKER_H
#define CLICKER_H

#include "clicker_data.h"
#include "process_handler/process_manager.h"
#include "routine.h"
#include <vector>
#include <thread>
#include <variant>

class Clicker {
public:
    std::shared_ptr<ProcessHandler::ProcessManager> process_manager;

    Clicker(uint32_t dwProcessID, const std::string& processName);

    void setClickerStatus(bool status);

    [[nodiscard]] bool getClickerStatus() const;

    void addRoutine(std::vector<std::variant<ClickerData, Delay>> routine);

    void startRoutines() noexcept;

    void stopRoutines() noexcept;

private:
    bool clickerStatus = false;
    std::vector<Routine> routines;
};

#endif // CLICKER_H

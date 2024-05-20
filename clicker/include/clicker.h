#ifndef CLICKER_H
#define CLICKER_H

#include "clicker_data.h"
#include "process_handler/process_manager.h"
#include "routine.h"
#include <vector>
#include <thread>

class Clicker {
private:
    bool clickerStatus = false;
    std::vector<std::thread> jobs{};
    std::vector<Routine> routines;
public:
    std::shared_ptr<ProcessHandler::ProcessManager> process_manager;

    Clicker(uint32_t dwProcessID, const std::string& processName);

    void setClickerStatus(bool status);

    [[nodiscard]] bool getClickerStatus() const;

    //void startClicking(const clickerData& data);

    void addRoutine(std::vector<ClickerData> routine);

    void startRoutines() noexcept;

    void stopRoutines() noexcept;

    void initClickerThreads(std::vector<ClickerData> &data);

    void destroyClickerThreads();
};

#endif // CLICKER_H

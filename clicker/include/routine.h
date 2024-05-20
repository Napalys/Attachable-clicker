//
// Created by Widok on 5/20/24.
//

#ifndef INJECTIONCLICKER_ROUTINE_H
#define INJECTIONCLICKER_ROUTINE_H


#include <thread>
#include <vector>
#include <memory>
#include "clicker_data.h"
#include "process_handler/process_manager.h"

class Routine {
public:
    explicit Routine(std::shared_ptr<ProcessHandler::ProcessManager> process_manager);
    explicit Routine(std::shared_ptr<ProcessHandler::ProcessManager> process_manager, std::vector<ClickerData> clicks);

    Routine(Routine&& other) noexcept = default;
    Routine& operator=(Routine&& other) noexcept = default;

    Routine(const Routine&) = delete;
    Routine& operator=(const Routine&) = delete;

    void startRoutine();

    ~Routine() noexcept;

private:
    std::shared_ptr<ProcessHandler::ProcessManager> process_manager;
    std::vector<ClickerData> clicks{};
    bool running = false;
    std::thread runner;
};


#endif //INJECTIONCLICKER_ROUTINE_H

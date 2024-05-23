//
// Created by Widok on 5/20/24.
//

#include "routine.h"

#include <utility>

Routine::Routine(std::shared_ptr<ProcessHandler::ProcessManager> process_manager) : process_manager(
        std::move(process_manager)) {};

Routine::Routine(std::shared_ptr<ProcessHandler::ProcessManager> process_manager,
                 std::vector<ClickerData> clicks) : process_manager(std::move(process_manager)),
                                                    clicks(std::move(clicks)) {}

Routine::~Routine() {
    running = false;
    if (runner.joinable()) runner.join();
}

void Routine::startRoutine() {
    running = true;
    runner = std::thread([&]() {
        using namespace std::chrono_literals;
        while (running) {
            for (const auto &data: clicks) {
                process_manager->sendClick(data);
                if(!running) break;
//                std::this_thread::sleep_for(std::chrono::milliseconds(data.delay));
            }
        }

    });
}

//
// Created by Widok on 5/20/24.
//

#include "routine.h"

#include <utility>

Routine::Routine(std::shared_ptr<ProcessHandler::ProcessManager> process_manager) : process_manager(
        std::move(process_manager)) {};

Routine::Routine(std::shared_ptr<ProcessHandler::ProcessManager> process_manager,
                 std::vector<std::variant<ClickerData, Delay>> clicks) : process_manager(std::move(process_manager)),
                                                    clicks(std::move(clicks)) {}

Routine::~Routine() noexcept {
    running = false;
    if (runner.joinable()) runner.join();
}

void Routine::startRoutine() {
    running = true;
    runner = std::thread([&]() {
        using namespace std::chrono_literals;
        while (running) {
            for (const auto &data : clicks) {
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, ClickerData>) {
                        process_manager->sendClick(arg);
                    } else if constexpr (std::is_same_v<T, Delay>) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(arg.delay));
                    }
                }, data);

                if (!running) break;
            }
        }
    });
}

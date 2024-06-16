#ifndef INJECTIONCLICKER_ANOMALY_RUNNER_H
#define INJECTIONCLICKER_ANOMALY_RUNNER_H

#include <thread>
#include <atomic>
#include "process_handler/process_manager.h"
#include "discord_bot.h"
#include "anomaly.h"

namespace Runners {



    class AnomalyRunner {
    public:
        AnomalyRunner(const std::shared_ptr<ProcessHandler::ProcessManager> &processManager,
                      const std::shared_ptr<Notification::DiscordBot> &bot, std::string channel_id,
                      std::vector<Anomaly> anomalies);

        void run();

        void stopRunner();

        virtual ~AnomalyRunner();


    private:
        std::shared_ptr<ProcessHandler::ProcessManager> process_manager;
        std::shared_ptr<Notification::DiscordBot> bot;
        std::string channel_id;
        std::vector<Anomaly> anomalies;
        std::thread runner;
        std::atomic<bool> running = false;
    };

} // Runners

#endif //INJECTIONCLICKER_ANOMALY_RUNNER_H

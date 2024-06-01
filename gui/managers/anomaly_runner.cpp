#include "anomaly_runner.h"
#include "image_processing.h"

namespace Runners {
    AnomalyRunner::AnomalyRunner(const std::shared_ptr<ProcessHandler::ProcessManager> &processManager,
                                 const std::shared_ptr<Notification::DiscordBot> &bot, std::string chan,
                                 std::vector<Anomaly> anomalies)
            : process_manager(
            processManager), bot(bot), channel_id(std::move(chan)), anomalies(std::move(anomalies)) {}

    void AnomalyRunner::run() {
        running = true;
        runner = std::thread([this]() {
            while (running) {
                using namespace std::chrono;
                std::this_thread::sleep_for(10s);
                const auto image_path = process_manager->takeScreenshot();
                if (image_path.empty()) continue;

                for(const auto& anomaly : anomalies){
                    double percentage = anomaly.coefficient;
                    const auto path = ImageProcessing::isImageWithinImage(image_path, anomaly.template_image, percentage / 100);
                    if(path.empty()) continue;
                    bot->send_message(channel_id, anomaly.message);
                    bot->send_image(channel_id, path);
                }

            }
        });
    }

    AnomalyRunner::~AnomalyRunner() {
        running = false;
        runner.join();
    }

    void AnomalyRunner::stopRunner() {
        running = false;
    }

} // Runners
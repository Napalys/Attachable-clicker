#include "../include/discord_bot.h"
#include <dpp/cluster.h>

namespace Notification {

    class DiscordBot::Impl {
    public:
        explicit Impl(const std::string& token) : bot(token) {
            bot.on_log(dpp::utility::cout_logger());
            bot.on_message_create([](const dpp::message_create_t& event) {
                if (event.msg.content == "!ping") {
                    event.reply("Pong!");
                }
            });
        }

        void run() {
            bot.start(dpp::st_wait);
        }

    private:
        dpp::cluster bot;
    };

    DiscordBot::DiscordBot(const std::string& token) : pImpl(std::make_unique<Impl>(token)) {}

    DiscordBot::~DiscordBot() noexcept = default;

    void DiscordBot::run() {
        pImpl->run();
    }

} // namespace Notification

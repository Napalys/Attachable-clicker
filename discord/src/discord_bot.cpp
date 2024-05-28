#include "discord_bot.h"
#include <dpp/cluster.h>
#include <fstream>

namespace Notification {

    class DiscordBot::Impl {
    public:
        explicit Impl(const std::string& token, LogCallback log_callback = nullptr) : bot(token), log_callback(std::move(log_callback)) {
            bot.on_log([this](const dpp::log_t& event) {
                if(this->log_callback) this->log_callback(event.message);
            });
            bot.on_message_create([](const dpp::message_create_t& event) {
                try {
                    if (event.msg.content == "!ping") {
                        event.reply("Pong!");
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error handling message: " << e.what() << std::endl;
                    event.reply("An error occurred while processing your request.");
                }
            });
        }

        void run() {
            try {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
                bot.start(&dpp::set_nonblocking);
#pragma GCC diagnostic pop
            } catch (const dpp::exception& e) {
                std::cerr << "Failed to start the Discord bot: " << e.what() << std::endl;
                throw std::runtime_error(e.what());
            } catch (const std::exception& e) {
                std::cerr << "A general exception occurred: " << e.what() << std::endl;
                throw e;
            }
        }

        void send_message(const std::string& channel_id, const std::string& message, const Callback& callback) {
            bot.message_create(dpp::message(channel_id, message), [callback, channel_id, message](const dpp::confirmation_callback_t& cb) {
                if (cb.is_error()) {
                    if (callback) callback(false, cb.http_info.body);
                } else {
                    if (callback) callback(true, "Message sent successfully");
                }
            });
        }

        void send_image(const std::string& channel_id, const std::string& image_path, const Callback& callback) {
            dpp::message msg(channel_id, "");
            std::string image_data = dpp::utility::read_file(image_path);
            if (image_data.empty()) {
                if (callback) callback(false, "Failed to read image file");
                return;
            }
            msg.add_file(image_path, image_data);

            bot.message_create(msg, [callback, channel_id](const dpp::confirmation_callback_t& cb) {
                if (cb.is_error()) {
                    if (callback) callback(false, cb.http_info.body);
                } else {
                    if (callback) callback(true, "Image sent successfully");
                }
            });
        }

    private:
        dpp::cluster bot;
        LogCallback log_callback;
    };

    DiscordBot::DiscordBot(const std::string& token, LogCallback log_callback) : pImpl(std::make_unique<Impl>(token, std::move(log_callback))) {}

    DiscordBot::~DiscordBot() noexcept = default;

    void DiscordBot::run() {
        pImpl->run();
    }

    void DiscordBot::send_message(const std::string& channel_id, const std::string& message, const Callback& callback) const {
        pImpl->send_message(channel_id, message, callback);
    }

    void DiscordBot::send_image(const std::string& channel_id, const std::string& image_path, const Callback& callback) const {
        pImpl->send_image(channel_id, image_path, callback);
    }

} // namespace Notification

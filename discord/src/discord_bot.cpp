#include "discord_bot.h"
#include <dpp/cluster.h>
#include <fstream>

namespace Notification {

    class DiscordBot::Impl {
    public:
        explicit Impl(const std::string& token) : bot(token) {
            bot.on_log([](const dpp::log_t& event) {
                std::cout << event.message << std::endl;
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
                bot.start(&dpp::set_nonblocking);
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
                    std::cerr << "Failed to send message to channel " << channel_id << ": " << cb.http_info.body << std::endl;
                    if (callback) callback(false, cb.http_info.body);
                } else {
                    std::cout << "Message sent successfully to channel " << channel_id << ": " << message << std::endl;
                    if (callback) callback(true, "Message sent successfully");
                }
            });
        }

        void send_image(const std::string& channel_id, const std::string& image_path, const Callback& callback) {
            dpp::message msg(channel_id, "Hey there, I've got a new file!");

            // Read the image file into a string
            std::string image_data = dpp::utility::read_file(image_path);
            std::cerr << "Loaded image data size: " << image_data.size() << " bytes" << std::endl;

            if (image_data.empty()) {
                std::cerr << "Failed to read image file: " << image_path << std::endl;
                if (callback) callback(false, "Failed to read image file");
                return;
            }

            // Attach the image data to the message
            msg.add_file("image.jpg", image_data);

            // Send the message with the image
            bot.message_create(msg, [callback, channel_id](const dpp::confirmation_callback_t& cb) {
                if (cb.is_error()) {
                    std::cerr << "Failed to send image to channel " << channel_id << ": " << cb.http_info.body << std::endl;
                    if (callback) callback(false, cb.http_info.body);
                } else {
                    std::cout << "Image sent successfully to channel " << channel_id << std::endl;
                    if (callback) callback(true, "Image sent successfully");
                }
            });
        }

    private:
        dpp::cluster bot;
    };

    DiscordBot::DiscordBot(const std::string& token) : pImpl(std::make_unique<Impl>(token)) {}

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

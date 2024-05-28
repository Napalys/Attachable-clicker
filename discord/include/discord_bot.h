#ifndef INJECTIONCLICKER_DISCORD_BOT_H
#define INJECTIONCLICKER_DISCORD_BOT_H

#include <string>
#include <memory>
#include <functional>

namespace Notification {

    class DiscordBot {
    public:
        explicit DiscordBot(const std::string& token);
        ~DiscordBot() noexcept;
        void run();

        using Callback = std::function<void(bool, const std::string&)>;

        void send_message(const std::string& channel_id, const std::string& message, const Callback& callback = nullptr) const;
        void send_image(const std::string& channel_id, const std::string& image_path, const Callback& callback = nullptr) const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace Notification

#endif //INJECTIONCLICKER_DISCORD_BOT_H

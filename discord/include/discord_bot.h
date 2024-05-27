//
// Created by Widok on 5/27/24.
//

#ifndef INJECTIONCLICKER_DISCORD_BOT_H
#define INJECTIONCLICKER_DISCORD_BOT_H

#include <string>
#include <memory>

namespace Notification {

    class DiscordBot {
    public:
        explicit DiscordBot(const std::string& token);
        ~DiscordBot() noexcept;
        void run();

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace Notification

#endif //INJECTIONCLICKER_DISCORD_BOT_H

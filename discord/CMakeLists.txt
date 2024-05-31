cmake_minimum_required(VERSION 3.10)
project(Notifier)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(dpp CONFIG REQUIRED)

include_directories(include)

add_library(${PROJECT_NAME}
        src/discord_bot.cpp
#        include/discord_bot.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${DPP_INCLUDE_DIRS}
)

target_include_directories(${PROJECT_NAME} PUBLIC
        include
)


target_link_libraries(${PROJECT_NAME} dpp::dpp)

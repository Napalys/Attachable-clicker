cmake_minimum_required(VERSION 3.10)
project(injectionClicker  VERSION 1.0.0
        DESCRIPTION ""
        HOMEPAGE_URL "https://github.com/Napalys/Attachable-clicker"
        LANGUAGES CXX C)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GIT_SHA "Unknown" CACHE STRING "SHA this build was generated from")

string(SUBSTRING "${GIT_SHA}"
        0
        8
        GIT_SHORT_SHA)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(clicker)
add_subdirectory(discord)
add_subdirectory(configured_files)

add_executable(${PROJECT_NAME} main.cpp
        gui/mainwindow.cpp
        gui/mainwindow.ui
        gui/delegates/non_editable_delegate.hpp
        gui/delegates/numeric_delegate.hpp
        gui/delegates/action_delegate.hpp
        gui/dialogs/clicker_data_dialog.cpp
        gui/dialogs/clicker_data_dialog.h
        gui/managers/table_manager.cpp
        gui/managers/table_manager.h
        gui/dialogs/loading_dialog.cpp
        gui/dialogs/loading_dialog.h
        gui/managers/anomaly_manager.cpp
        gui/managers/anomaly_manager.h
        gui/dialogs/add_anomaly_dialog.cpp
        gui/dialogs/add_anomaly_dialog.h
)

target_include_directories(${PROJECT_NAME} PRIVATE gui)
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Widgets
        clicker_lib
        Notifier
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_EXTENSIONS OFF
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_BINARY_DIR}/configured_files/include/internal_use_only
        gui
)

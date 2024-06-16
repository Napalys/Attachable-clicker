cmake_minimum_required(VERSION 3.10)
project(Imaging)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV CONFIG REQUIRED)

include_directories(include)
include_directories(${OPENCV_INCLUDE_DIRS})

add_library(${PROJECT_NAME}
        src/image_processing.cpp
        include/image_processing.h
)

target_include_directories(${PROJECT_NAME} PUBLIC
        include
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

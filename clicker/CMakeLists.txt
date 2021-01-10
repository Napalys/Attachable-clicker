project(clicker)

set(HEADER_FILES clickerData.h clicker.h)

set(SOURCE_FILES clicker.cpp)

add_library(clicker STATIC ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(clicker PROPERTIES LINKER_LANGUAGE CXX)
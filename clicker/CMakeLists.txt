project(clicker_lib)

set(HEADER_FILES clickerData.h clicker.h)

set(SOURCE_FILES clicker.cpp)

add_library(clicker_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(clicker_lib PROPERTIES LINKER_LANGUAGE CXX)
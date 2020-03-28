cmake_minimum_required(VERSION 3.17)
project(TrainingRaft)

set(CMAKE_CXX_STANDARD 20)
set(MAIN_FILE "src/main.cpp")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/${MAIN_FILE}")

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")

# set(STRICT_OPTIONS -Wall -Wextra -Werror -ansi)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(training-raft-objects OBJECT ${SOURCES} ${HEADERS})
target_link_libraries(training-raft-objects ${CONAN_LIBS})

add_executable(training-raft ${MAIN_FILE} $<TARGET_OBJECTS:training-raft-objects>)
target_compile_options(training-raft PRIVATE ${STRICT_OPTIONS})


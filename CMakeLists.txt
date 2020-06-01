cmake_minimum_required(VERSION 3.17)
project(TrainingRaft)

set(CMAKE_CXX_STANDARD 20)
set(MAIN_FILE "src/main.cpp")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/${MAIN_FILE}")

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")

file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "test/*.cpp")
file(GLOB_RECURSE TESTS_HEADERS CONFIGURE_DEPENDS "test/*.h")

set(STRICT_OPTIONS -Wall -Wextra -Werror -ansi)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Seems like conan can't into pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Enabling coroutines
# TODO: set per target
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts -stdlib=libc++")
# Disabling concepts because of asio bug https://github.com/boostorg/asio/issues/312
add_definitions(-DBOOST_ASIO_DISABLE_CONCEPTS)

# All src object files
add_library(training-raft-objects OBJECT ${SOURCES} ${HEADERS})
target_compile_options(training-raft-objects PRIVATE ${STRICT_OPTIONS})

# Main executable
add_executable(training-raft ${MAIN_FILE} $<TARGET_OBJECTS:training-raft-objects>)
target_compile_options(training-raft PRIVATE ${STRICT_OPTIONS})
# TODO: seems like gmock/gtest still will be linked. Or not?
#message(INFO ${CONAN_LIBS_BOOST})
target_link_libraries(training-raft ${CONAN_LIBS} Threads::Threads)

# Executable with tests
add_executable(training-raft-tests ${TESTS} ${TESTS_HEADERS} $<TARGET_OBJECTS:training-raft-objects>)
target_include_directories(training-raft-tests PRIVATE "src/")
target_compile_options(training-raft-tests PRIVATE ${STRICT_OPTIONS})
set(GTEST_LIBRARIES_LIST
    gtest
    gmock
    gmock_main)
target_link_libraries(training-raft-tests ${CONAN_LIBS} ${GTEST_LIBRARIES_LIST} Threads::Threads)

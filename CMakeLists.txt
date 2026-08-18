cmake_minimum_required(VERSION 3.15)
project(TelemetryCacheStreamer VERSION 1.4.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/telemetry_streamer.cpp
)

# Header files
set(HEADERS
    include/telemetry_streamer.h
)

add_executable(tcs_daemon ${SOURCES} ${HEADERS})

# Link dependencies (e.g. threads)
find_package(Threads REQUIRED)
target_link_libraries(tcs_daemon PRIVATE Threads::Threads)

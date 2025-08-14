cmake_minimum_required(VERSION 3.10)
project(CMEMockServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(CORE_SOURCES
    src/core/order_book.cpp
    src/core/order_book_manager.cpp
    src/core/market_data_generator.cpp
    src/core/reference_data_manager.cpp
)

set(NETWORK_SOURCES
    src/network/udp_publisher.cpp
    src/network/multicast_publisher.cpp
    src/network/feed_publisher.cpp
)

set(MESSAGE_SOURCES
    src/messages/mdp_messages.cpp
    src/messages/sbe_encoder.cpp
    src/messages/message_factory.cpp
)

set(CONFIG_SOURCES
    src/config/configuration.cpp
    src/config/instrument_config.cpp
)

set(UTILS_SOURCES
    src/utils/logger.cpp
    src/utils/time_utils.cpp
)

# Main library
add_library(cme_mock_core STATIC
    ${CORE_SOURCES}
    ${NETWORK_SOURCES}
    ${MESSAGE_SOURCES}
    ${CONFIG_SOURCES}
    ${UTILS_SOURCES}
)

target_link_libraries(cme_mock_core
    Threads::Threads
)

# Main executable
add_executable(cme_mock_server
    src/main.cpp
)

target_link_libraries(cme_mock_server
    cme_mock_core
)

# Test executable
enable_testing()
add_subdirectory(test)

# Installation
install(TARGETS cme_mock_server DESTINATION bin)
install(DIRECTORY config/ DESTINATION etc/cme_mock_server)
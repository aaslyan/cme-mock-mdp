cmake_minimum_required(VERSION 3.10)
project(MarketDataPlatform VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(Threads REQUIRED)

# Include directories for the new structure
set(CORE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/core/include)
set(PROTOCOLS_COMMON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/protocols/common/include)
set(PROTOCOLS_CME_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/protocols/cme/include)

# ===========================
# Core Market Data Library
# ===========================
set(CORE_SOURCES
    core/src/order_book.cpp
    core/src/order_book_manager.cpp
    core/src/market_data_generator.cpp
)

add_library(market_core STATIC ${CORE_SOURCES})
target_include_directories(market_core PUBLIC ${CORE_INCLUDE_DIR})
target_link_libraries(market_core Threads::Threads)

# ===========================
# Protocol Common Library
# ===========================
set(PROTOCOL_COMMON_SOURCES
    protocols/common/src/udp_transport.cpp
)

add_library(protocol_common STATIC ${PROTOCOL_COMMON_SOURCES})
target_include_directories(protocol_common PUBLIC 
    ${PROTOCOLS_COMMON_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
)
target_link_libraries(protocol_common market_core)

# ===========================
# CME Protocol Library
# ===========================
set(CME_SOURCES
    protocols/cme/src/cme_encoder.cpp
    protocols/cme/src/cme_protocol_adapter.cpp
    protocols/cme/src/cme_event_listener.cpp
)

add_library(cme_protocol STATIC ${CME_SOURCES})
target_include_directories(cme_protocol PUBLIC 
    ${PROTOCOLS_CME_INCLUDE_DIR}
    ${PROTOCOLS_COMMON_INCLUDE_DIR}
    ${CORE_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include  # For legacy SBE headers
)
target_link_libraries(cme_protocol 
    market_core 
    protocol_common
)

# ===========================
# CME Server Application
# ===========================
add_executable(cme_server
    apps/cme_server/cme_server_main.cpp
)

target_include_directories(cme_server PRIVATE
    ${CORE_INCLUDE_DIR}
    ${PROTOCOLS_COMMON_INCLUDE_DIR}
    ${PROTOCOLS_CME_INCLUDE_DIR}
)

target_link_libraries(cme_server
    cme_protocol
    protocol_common
    market_core
    Threads::Threads
)

# ===========================
# Legacy CME Server (for comparison)
# ===========================
# Keep the old structure for testing/comparison
set(LEGACY_SOURCES
    src/core/order_book.cpp
    src/core/order_book_manager.cpp
    src/core/market_data_generator.cpp
    src/core/reference_data.cpp
    src/network/udp_publisher.cpp
    src/network/feed_publisher.cpp
    src/network/definition_feed_publisher.cpp
    src/network/udp_receiver.cpp
    src/messages/sbe_encoder.cpp
    src/messages/sbe_decoder.cpp
    src/messages/cme_sbe_encoder.cpp
    src/messages/instrument_encoder.cpp
    src/messages/message_factory.cpp
    src/config/configuration.cpp
    src/scenarios/market_scenario.cpp
    src/utils/logger.cpp
    src/utils/hexdump.cpp
    src/utils/packet_verifier.cpp
)

add_library(cme_legacy STATIC ${LEGACY_SOURCES})
target_include_directories(cme_legacy PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(cme_legacy Threads::Threads)

add_executable(cme_mock_server src/main.cpp)
target_link_libraries(cme_mock_server cme_legacy)

# ===========================
# Test Programs
# ===========================
# Individual test programs using legacy structure for now
set(TEST_PROGRAMS
    test_order_book
    test_mdp_encoding
    test_reference_data
    test_scenarios
    test_udp_publisher
    debug_client
    cme_test_client
    list_instruments
)

foreach(TEST_PROG ${TEST_PROGRAMS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/test/${TEST_PROG}.cpp)
        add_executable(${TEST_PROG} test/${TEST_PROG}.cpp)
        target_link_libraries(${TEST_PROG} cme_legacy)
    endif()
endforeach()

# Additional executables found in root
set(ROOT_EXECUTABLES
    test_sbe_encoding
    test_packet_verification
    test_hexdump
    cme_data_analyzer
    analyze_packet_size
    test_client
    cme_mock_server
)

foreach(EXEC ${ROOT_EXECUTABLES})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${EXEC}.cpp)
        add_executable(${EXEC} ${EXEC}.cpp)
        target_link_libraries(${EXEC} cme_legacy)
    endif()
endforeach()

# ===========================
# Utilities and Tools
# ===========================
# Build existing utility programs
if(EXISTS ${CMAKE_SOURCE_DIR}/udp_receiver.cpp)
    add_executable(udp_receiver ${CMAKE_SOURCE_DIR}/test/udp_receiver.cpp)
    target_link_libraries(udp_receiver cme_legacy)
endif()

# ===========================
# Testing
# ===========================
enable_testing()

# Add tests for new architecture
add_test(NAME core_order_book_test 
         COMMAND test_order_book)
add_test(NAME core_market_generator_test 
         COMMAND test_scenarios)

# Quick integration test
if(EXISTS ${CMAKE_SOURCE_DIR}/quick_test.sh)
    add_test(NAME quick_integration_test
             COMMAND ${CMAKE_SOURCE_DIR}/quick_test.sh)
endif()

# ===========================
# Installation
# ===========================
install(TARGETS cme_server DESTINATION bin)
install(TARGETS cme_mock_server DESTINATION bin)
install(DIRECTORY config/ DESTINATION etc/market_data_platform)

# ===========================
# Documentation
# ===========================
# Install documentation
install(FILES CLAUDE.md README.md DESTINATION doc)

# ===========================
# Debug Information
# ===========================
message(STATUS "Build Configuration:")
message(STATUS "  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "  CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "  CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "  Core Library: market_core")
message(STATUS "  CME Protocol Library: cme_protocol")
message(STATUS "  Main Executable: cme_server")
message(STATUS "  Legacy Executable: cme_mock_server")
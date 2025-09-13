cmake_minimum_required(VERSION 3.10)
project(MDClientSample)

# Make sure to set BOOST_ROOT variable in your build environment.
if(NOT DEFINED ENV{BOOST_ROOT})
    message(FATAL_ERROR "BOOST_ROOT environment variable must be set!")
endif()

find_package(Boost 1.67.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(md-client 
    Main.cpp Client.cpp FIXMsg.cpp EstablishMsg.cpp 
    NegotiateMsg.cpp TerminateMsg.cpp)


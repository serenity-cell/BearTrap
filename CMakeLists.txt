cmake_minimum_required(VERSION 3.10.0)
project(BearTrap VERSION 0.1.0 LANGUAGES C CXX)

add_executable(BearTrap main.cpp src/honeypot.cpp src/logger.cpp)

include(CTest)
enable_testing()


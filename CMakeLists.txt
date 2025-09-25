cmake_minimum_required(VERSION 3.20)
project(Logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDAR_REQUIRED ON)

add_library(Logger INTERFACE)
target_include_directories(Logger INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

option(LOGGER_ENABLE_TESTS "Enable building tests for Logger" OFF)

if (LOGGER_ENABLE_TESTS)
  enable_testing()

  add_executable(logger_tests tests/LoggerTests.cpp)
  target_include_directories(logger_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  add_test(NAME logger_tests COMMAND logger_tests)
endif()


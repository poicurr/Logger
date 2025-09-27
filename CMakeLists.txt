cmake_minimum_required(VERSION 3.20)
project(Logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Logger INTERFACE)
target_include_directories(Logger INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Install support toggled by option that also works when embedded
set(LOGGER_IS_TOP_LEVEL OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(LOGGER_IS_TOP_LEVEL ON)
endif()

set(_logger_default_install OFF)
if(LOGGER_IS_TOP_LEVEL)
  set(_logger_default_install ON)
endif()
option(LOGGER_ENABLE_INSTALL "Enable install/export targets for Logger" ${_logger_default_install})
unset(_logger_default_install)

set(LOGGER_EXPORT_SET "LoggerTargets" CACHE STRING "Export set used when installing Logger")
set(LOGGER_INSTALL_EXPORT_FILE ON CACHE BOOL "Install the Logger export file")
set(LOGGER_EXPORT_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Logger" CACHE PATH "Destination for the Logger export file")

if(LOGGER_ENABLE_INSTALL)
  include(GNUInstallDirs)

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Logger)
  install(TARGETS Logger EXPORT ${LOGGER_EXPORT_SET})

  if(LOGGER_INSTALL_EXPORT_FILE)
    install(EXPORT ${LOGGER_EXPORT_SET} NAMESPACE Logger:: DESTINATION ${LOGGER_EXPORT_DESTINATION})
  endif()
endif()

# Optional tests
option(LOGGER_ENABLE_TESTS "Enable building tests for Logger" OFF)

if(LOGGER_ENABLE_TESTS)
  enable_testing()

  add_executable(logger_tests tests/LoggerTests.cpp)
  target_include_directories(logger_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  add_test(NAME logger_tests COMMAND logger_tests)
endif()



cmake_minimum_required(VERSION 3.10)

project(envmonitor
  DESCRIPTION "Environmental monitoring device"
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wconversion -Wextra -pedantic)

include(GNUInstallDirs)
enable_testing()

add_executable(envmonitor
  main.cpp
  JSONReader.cpp
  SCD4x.cpp
  DataCleaner.cpp
  SDCardWriter.cpp
  MQTTPublisher.cpp
  Sensirion/sensirion_i2c.c
  Sensirion/scd4x_i2c.c
  Sensirion/sensirion_common.c
  Sensirion/sensirion_i2c_hal.c
)

target_include_directories(envmonitor PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Sensirion
)

target_link_libraries(envmonitor
  paho-mqttpp3
  paho-mqtt3as
  pthread
)

# add_subdirectory(test)
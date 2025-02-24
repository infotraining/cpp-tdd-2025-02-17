cmake_minimum_required(VERSION 3.15)

##############
# Vcpkg integration - uncomment if necessery
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

message(STATUS "Vcpkg integration script found: " ${CMAKE_TOOLCHAIN_FILE})

##################
# Project
get_filename_component(PROJECT_NAME_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_MAIN ${PROJECT_NAME_DIR})

set(PROJECT_ID ${PROJECT_MAIN})
project(${PROJECT_ID})
message(STATUS "PROJECT_ID is: " ${PROJECT_ID})

enable_testing()
add_subdirectory(snake_lib)
add_subdirectory(snake_tests)
# add_subdirectory(snake_console)
add_subdirectory(snake_sfml)
# Set VCPKG_ROOT to your vcpkg installation directory or add the following to your cmake call:
# -DCMAKE_TOOLCHAIN=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake

cmake_minimum_required(VERSION 3.16)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")  
endif()
if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

project(cpp-tdd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/bin")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/bin")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/lib")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/lib")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/lib")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/lib")

enable_testing()

add_subdirectory(catch-demo)
add_subdirectory(googlemock-demo)
add_subdirectory(injecting-dependencies)
add_subdirectory(mocks-vs-stubs)
add_subdirectory(mother-vs-builder)
add_subdirectory(document-editor)

add_subdirectory(tdd-katas/catch2/bowling-catch)
add_subdirectory(tdd-katas/catch2/recently-used-list-catch)
add_subdirectory(tdd-katas/catch2/mars-rover-catch)

add_subdirectory(tdd-katas/gtest/bowling)
add_subdirectory(tdd-katas/gtest/recently-used-list)
add_subdirectory(tdd-katas/gtest/mars-rover)

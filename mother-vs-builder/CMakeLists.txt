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
add_subdirectory(src)
add_subdirectory(tests)

####################
# Packages & libs
find_package(Threads REQUIRED)

####################
# Main app
# include_directories(src)
add_executable(${PROJECT_MAIN} main.cpp)
target_link_libraries(${PROJECT_MAIN} PRIVATE ${PROJECT_LIB} ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${PROJECT_MAIN} PUBLIC cxx_std_20)

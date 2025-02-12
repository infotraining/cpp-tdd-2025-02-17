set(PROJECT_TESTS "tests-${PROJECT_ID}")
set(PROJECT_TESTS "tests-${PROJECT_ID}" PARENT_SCOPE)
message(STATUS "PROJECT_TESTS is: " ${PROJECT_TESTS})

####################
# Sources & headers
aux_source_directory(. SRC_LIST)
file(GLOB HEADERS_LIST "*.h" "*.hpp")

find_package(Catch2 3 REQUIRED)

if (NOT Catch2_FOUND)
  Include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0 # or a later release
  )

  FetchContent_MakeAvailable(Catch2)

  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

include(CTest)
include(Catch)

add_executable(${PROJECT_TESTS} ${SRC_LIST} ${HEADERS_LIST})

target_link_libraries(${PROJECT_TESTS} PRIVATE Catch2::Catch2WithMain ${PROJECT_LIB})

catch_discover_tests(${PROJECT_TESTS})
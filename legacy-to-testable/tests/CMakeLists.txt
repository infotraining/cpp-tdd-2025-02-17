set(PROJECT_TESTS "tests-${PROJECT_ID}")
set(PROJECT_TESTS "tests-${PROJECT_ID}" PARENT_SCOPE)
message(STATUS "PROJECT_TESTS is: " ${PROJECT_TESTS})

# project(${PROJECT_TESTS})

find_package(GTest CONFIG REQUIRED)
enable_testing()

####################
# Sources & headers
file(GLOB SRC_FILES *.cpp *.c *.cxx)
file(GLOB SRC_HEADERS *.h *.hpp *.hxx)

add_executable(${PROJECT_TESTS} ${SRC_FILES} ${SRC_HEADERS})

target_link_libraries(${PROJECT_TESTS} ${PROJECT_LIB} GTest::gmock_main)
target_compile_features(${PROJECT_TESTS} PRIVATE cxx_std_20)

include(CTest)
include(GoogleTest)
gtest_discover_tests(${PROJECT_TESTS})

file(COPY data.dat DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
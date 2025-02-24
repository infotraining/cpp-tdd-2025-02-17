set(PROJECT_LIB "${PROJECT_ID}_lib")
set(PROJECT_LIB "${PROJECT_ID}_lib" PARENT_SCOPE)
message(STATUS "PROJECT_LIB is: " ${PROJECT_LIB})

# Include(FetchContent)

# FetchContent_Declare(
#   cpp-terminal
#   GIT_REPOSITORY https://github.com/jupyter-xeus/cpp-terminal.git
# )

# FetchContent_MakeAvailable(cpp-terminal)

# file(GLOB SRC_FILES *.cpp *.c *.cxx)
# file(GLOB SRC_HEADERS *.h *.hpp *.hxx)

add_library(${PROJECT_LIB} STATIC src/snake.cpp)

target_compile_features(${PROJECT_LIB} PUBLIC cxx_std_20)
target_include_directories(${PROJECT_LIB} PUBLIC ./include)

#target_link_libraries(${PROJECT_LIB} PUBLIC cpp-terminal::cpp-terminal Warnings::Warnings)

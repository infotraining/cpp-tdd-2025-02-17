set(SNAKE_CONSOLE "${PROJECT_ID}-console")
set(SNAKE_CONSOLE "${PROJECT_ID}-console" PARENT_SCOPE)
message(STATUS "SNAKE_CONSOLE is: " ${SNAKE_CONSOLE})

Include(FetchContent)

FetchContent_Declare(
  cpp-terminal
  GIT_REPOSITORY https://github.com/jupyter-xeus/cpp-terminal.git
)

FetchContent_MakeAvailable(cpp-terminal)

add_executable(${SNAKE_CONSOLE} main.cpp)

target_link_libraries(${SNAKE_CONSOLE} PUBLIC ${PROJECT_LIB} cpp-terminal::cpp-terminal Warnings::Warnings)
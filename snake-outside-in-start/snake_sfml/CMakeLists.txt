set(SNAKE_SFML "${PROJECT_ID}-sfml")
set(SNAKE_SFML "${PROJECT_ID}-sfml" PARENT_SCOPE)
message(STATUS "SNAKE_SFML is: " ${SNAKE_SFML})

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)

add_executable(${SNAKE_SFML} main.cpp)

target_link_libraries(${SNAKE_SFML} PUBLIC ${PROJECT_LIB} sfml-system sfml-network sfml-graphics sfml-window)

file(COPY fonts/default-font.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY fonts/ibm-vga-9x16.ttf DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
cmake_minimum_required(VERSION 3.10)
project(parser_sample VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wextra -g -O0")

include_directories(~/ypg/include /mingw64/local/include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
link_directories(/mingw64/local/lib)

add_custom_command(OUTPUT lexer.cpp lexer.h
    COMMAND reflex ${CMAKE_CURRENT_SOURCE_DIR}/lex.l
    DEPENDS lex.l
    COMMENT "RE/flex-ing lex.l")
add_custom_target(lexer
    DEPENDS lexer.cpp lexer.h)

add_executable(main lexer.cpp parser.cpp main.cpp)
target_link_libraries(main reflex)

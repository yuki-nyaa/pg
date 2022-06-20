cmake_minimum_required(VERSION 3.10)
project(parser_sample VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-Wextra -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified. Defaulted to 'Release'.")
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(YUKI_PG_DBG TRUE)
    message("Build type: Debug")
else()
    set(YUKI_PG_DBG FALSE)
    message("Build type: Release")
endif()

configure_file(cconfig.in cconfig)

set(YUKI_PG_ROOT "../../../..")

include_directories(${YUKI_PG_ROOT}/include /usr/local/include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
link_directories(/usr/local/lib)


add_custom_command(OUTPUT lexer.cpp lexer.h
    COMMAND reflex ${PROJECT_SOURCE_DIR}/lex.l
    DEPENDS lex.l
    COMMENT "RE/flex-ing lex.l")

add_custom_target(lexer
    DEPENDS lexer.cpp lexer.h)

add_custom_command(OUTPUT parser.cpp parser.h parser.token.hpp
    COMMAND ypg -i ${PROJECT_SOURCE_DIR}/parser.y -o parser.cpp -h parser.h -t parser.token.hpp
    DEPENDS parser.y
    COMMENT "YPG-ing parser.y")

add_custom_target(parser
    DEPENDS parser.cpp parser.h parser.token.hpp)

add_executable(main lexer.cpp parser.cpp main.cpp)
target_link_libraries(main reflex fmt)

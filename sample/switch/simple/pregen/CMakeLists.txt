cmake_minimum_required(VERSION 3.10)
project(parser_sample VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-Wextra -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified. Defaulted to 'Debug'.")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(YUKI_PARSER_SAMPLE_DBG TRUE)
    message("Build type: Debug")
else()
    set(YUKI_PARSER_SAMPLE_DBG FALSE)
    message("Build type: Release")
endif()

configure_file(cconfig.in cconfig)

if(MINGW)
    set(USR "mingw64")
else()
    set(USR "usr")
endif()
set(LOCAL_INCLUDE /${USR}/local/include)
set(LOCAL_LIB /${USR}/local/lib)

set(YUKI_PG_ROOT "../../../..")

include_directories(${YUKI_PG_ROOT}/include ${LOCAL_INCLUDE} ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
link_directories(${LOCAL_LIB})



add_custom_command(OUTPUT lexer.cpp lexer.h
    COMMAND reflex ${PROJECT_SOURCE_DIR}/lex.l
    DEPENDS lex.l
    COMMENT "RE/flex-ing lex.l")

add_custom_target(lexer
    DEPENDS lexer.cpp lexer.h)

add_executable(main lexer.cpp parser.cpp main.cpp)
target_link_libraries(main reflex fmt)

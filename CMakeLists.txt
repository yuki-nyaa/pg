cmake_minimum_required(VERSION 3.10)
project(yuki.pg VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-Wextra -Wno-narrowing -Wno-missing-field-initializers -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-Wno-narrowing -O2")
if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified. Defaulted to 'Debug'.")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(YUKI_PG_DBG TRUE)
    message("Build type: Debug")
else()
    set(YUKI_PG_DBG FALSE)
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

include_directories(${PROJECT_SOURCE_DIR}/include ${LOCAL_INCLUDE} ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
link_directories(${LOCAL_LIB})

add_custom_command(OUTPUT meta_lexer0.cpp meta_lexer0.h
    COMMAND reflex ${PROJECT_SOURCE_DIR}/meta_lexer0.l
    DEPENDS meta_lexer0.l
    COMMENT "RE/flex-ing meta_lexer0.l")
add_custom_target(meta_lexer0
    DEPENDS meta_lexer0.cpp meta_lexer0.h)

add_custom_command(OUTPUT meta_lexer1.cpp meta_lexer1.h
    COMMAND reflex ${PROJECT_SOURCE_DIR}/meta_lexer1.l
    DEPENDS meta_lexer1.l
    COMMENT "RE/flex-ing meta_lexer1.l")
add_custom_target(meta_lexer1
    DEPENDS meta_lexer1.cpp meta_lexer1.h)

add_executable(ypg main.cpp meta_lexer0.cpp meta_lexer1.cpp lr1_writer.cpp)
target_link_libraries(ypg reflex fmt)

install(DIRECTORY include/yuki TYPE INCLUDE)
install(TARGETS ypg RUNTIME)

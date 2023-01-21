cmake_minimum_required(VERSION 3.10)
project(yuki.pg VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "-Wextra -Wno-missing-field-initializers -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified. Defaulted to 'Release'.")
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(YUKI_PG_META_DBG TRUE)
    message("Build type: Debug")
else()
    set(YUKI_PG_META_DBG FALSE)
    message("Build type: Release")
endif()

configure_file(cconfig.in cconfig)

include_directories(${PROJECT_SOURCE_DIR}/include /usr/local/include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
link_directories(/usr/local/lib)

add_custom_command(OUTPUT Meta_Lexer0.cpp Meta_Lexer0.h
    COMMAND reflex ${PROJECT_SOURCE_DIR}/Meta_Lexer0.l
    DEPENDS Meta_Lexer0.l
    COMMENT "RE/flex-ing Meta_Lexer0.l")
add_custom_target(Meta_Lexer0
    DEPENDS Meta_Lexer0.cpp Meta_Lexer0.h)

add_custom_command(OUTPUT Meta_Lexer1.cpp Meta_Lexer1.h
    COMMAND reflex ${PROJECT_SOURCE_DIR}/Meta_Lexer1.l
    DEPENDS Meta_Lexer1.l
    COMMENT "RE/flex-ing Meta_Lexer1.l")
add_custom_target(Meta_Lexer1
    DEPENDS Meta_Lexer1.cpp Meta_Lexer1.h)

add_executable(ypg main.cpp Meta_Lexer0.cpp Meta_Lexer1.cpp LR1_Writer.cpp)
target_link_libraries(ypg reflex fmt)

install(DIRECTORY include/yuki TYPE INCLUDE)
install(TARGETS ypg RUNTIME)

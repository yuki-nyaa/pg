cmake_minimum_required(VERSION 3.20)
project(ypg_sample)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified. Defaulted to 'Release'.")
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(YUKI_PARSER_SAMPLE_DBG TRUE)
    add_compile_options(-Wextra -O0)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wno-missing-field-initializers)
    elseif(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
        message(FATAL_ERROR "Unsupported compiler! Please use GCC or Clang!")
    endif()
    message("Build type: Debug")
else()
    set(YUKI_PARSER_SAMPLE_DBG FALSE)
    add_compile_options(-O2)
    message("Build type: Release")
endif()

configure_file(cconfig.in cconfig)

set(YUKI_PG_ROOT "../..")

include_directories(${YUKI_PG_ROOT}/include /usr/local/include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
link_directories(/usr/local/lib)

if(NOT YUKI_PG_SAMPLE_PREGEN)
    add_custom_target(build-time-make-directory
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/gen)

    add_custom_command(
        OUTPUT ${PROJECT_SOURCE_DIR}/gen/parser.cpp ${PROJECT_SOURCE_DIR}/gen/parser.h ${PROJECT_SOURCE_DIR}/gen/parser.token.hpp
        COMMAND ypg -i parser.y -o gen/parser.cpp -h gen/parser.h -t gen/parser.token.hpp
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS build-time-make-directory parser.y
        COMMENT "YPG-ing parser.y")

    add_custom_target(parser
        DEPENDS ${PROJECT_SOURCE_DIR}/gen/parser.cpp ${PROJECT_SOURCE_DIR}/gen/parser.h ${PROJECT_SOURCE_DIR}/gen/parser.token.hpp)
endif()

add_executable(main ${PROJECT_SOURCE_DIR}/gen/parser.cpp main.cpp)
target_link_libraries(main fmt)

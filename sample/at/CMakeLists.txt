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
    set(YUKI_PG_SAMPLE_DBG TRUE)
    message("Build type: Debug")
else()
    set(YUKI_PG_SAMPLE_DBG FALSE)
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

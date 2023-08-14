cmake_minimum_required(VERSION 3.20)
project(yuki.pg VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    message("No build type specified. Defaulted to 'Release'.")
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(YUKI_PG_META_DBG TRUE)
    add_compile_options(-Wextra -O0)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wno-missing-field-initializers)
    elseif(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
        message(FATAL_ERROR "Unsupported compiler! Please use GCC or Clang!")
    endif()
    message("Build type: Debug")
else()
    set(YUKI_PG_META_DBG FALSE)
    add_compile_options(-O2)
    message("Build type: Release")
endif()

configure_file(cconfig.in cconfig)

include_directories(${PROJECT_SOURCE_DIR}/include /usr/local/include ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
link_directories(/usr/local/lib)

add_executable(ypg main.cpp lr.cpp)
target_link_libraries(ypg fmt)

install(DIRECTORY include/yuki TYPE INCLUDE)
install(TARGETS ypg RUNTIME)

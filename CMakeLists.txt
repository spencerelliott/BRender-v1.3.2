cmake_minimum_required(VERSION 3.20)
project(BRender VERSION 1.3.2 LANGUAGES C)

option(BRENDER_FLOAT "Use 32-bit floating point scalars" ON)
option(BRENDER_USE_ASM "Build legacy x86 assembly paths" OFF)
option(BRENDER_BUILD_TESTS "Build smoke tests" ON)
option(BRENDER_BUILD_SOFTREND "Build software renderer driver (C-only geometry)" ON)

if(NOT BRENDER_FLOAT)
    message(FATAL_ERROR "Only BASED_FLOAT builds are supported by the CMake port (Phase 0).")
endif()

set(BRENDER_SOURCE_DIR "${CMAKE_SOURCE_DIR}" CACHE PATH "BRender source root")

include(cmake/BRenderOptions.cmake)

add_subdirectory(core)

if(BRENDER_BUILD_SOFTREND)
    add_subdirectory(drivers/softrend)
endif()

if(BRENDER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

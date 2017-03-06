cmake_minimum_required(VERSION 3.5)
project(nigiri)

include(pre.cmake)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(LIBRARIES dl)
endif()

set(BUILD_TYPE "shared")

set(SIGAR_MODULE_JVM "yes")

include(post.cmake)
add_subdirectory(modules/jvm)
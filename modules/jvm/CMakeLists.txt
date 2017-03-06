cmake_minimum_required(VERSION 3.5)
project(nigiri_jvm)

include(pre.cmake)

set(LIBRARIES nigiri)

find_package(JNI)
if(NOT JNI_FOUND)
    message(FATAL_ERROR "Cannot find JNI installation")
endif()

set(LIBRARIES ${JNI_LIBRARIES})
include_directories(${JNI_INCLUDE_DIRS})
link_directories(${JAVA_JVM_LIBRARY})


set(BUILD_TYPE "shared")

include(post.cmake)
add_subdirectory(tester)

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

set(BUILD_TYPE "shared")

include(post.cmake)
add_subdirectory(tester)

enable_testing()
file(GLOB_RECURSE TEST_SOURCE_FILES "${PROJECT_SOURCE_DIR}/tests/*.cpp")

foreach(TEST_SOURCE ${TEST_SOURCE_FILES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} nigiri)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

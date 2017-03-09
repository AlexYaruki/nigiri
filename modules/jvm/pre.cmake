set(HEADERS "${PROJECT_SOURCE_DIR}/include/generic")

set(UNIX_HEADERS "${PROJECT_SOURCE_DIR}/include/unix")
set(LINUX_HEADERS "${PROJECT_SOURCE_DIR}/include/linux")
set(DARWIN_HEADERS "${PROJECT_SOURCE_DIR}/include/darwin")

set(WIN32_HEADERS "${PROJECT_SOURCE_DIR}/include/win32")

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/generic/*.cpp")

file(GLOB_RECURSE UNIX_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/unix/*.cpp")
file(GLOB_RECURSE LINUX_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/linux/*.cpp")
file(GLOB_RECURSE DARWIN_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/darwin/*.cpp")

file(GLOB_RECURSE WIN32_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/win32/*.cpp")

set(INPUT_FILES ${SOURCE_FILES})

set(DEBUG "no")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(BUILD_PLATFORM "linux")
    set(INPUT_FILES "${INPUT_FILES};${LINUX_SOURCE_FILES};${UNIX_SOURCE_FILES}")
    set(HEADERS "${HEADERS};${LINUX_HEADERS};${UNIX_HEADERS}")
    set(TARGET_INSTALL_DIR /usr/local/lib)
    set(HEADERS_INSTALL_DIR /usr/local/include)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    set(BUILD_PLATFORM "darwin")
    set(INPUT_FILES "${INPUT_FILES};${DARWIN_SOURCE_FILES};${UNIX_SOURCE_FILES}")
    set(HEADERS "${HEADERS};${DARWIN_HEADERS};${UNIX_HEADERS}")
    set(TARGET_INSTALL_DIR /usr/local/lib)
    set(HEADERS_INSTALL_DIR /usr/local/include)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(BUILD_PLATFORM "win32")
    set(INPUT_FILES "${INPUT_FILES};${WIN32_SOURCE_FILES}")
    set(HEADERS "${HEADERS};${WIN32_HEADERS}")
    set(TARGET_INSTALL_DIR C:/Libraries)
    set(HEADERS_INSTALL_DIR C:/Headers)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()
message(STATUS "Input files: ${INPUT_FILES}")


if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Werror -pedantic")
    add_definitions("-D${PROJECT_SHARED_DEFINITION}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    ##set(INPUT_FILES ${INPUT_FILES} ${HEADERS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    add_definitions("/D${PROJECT_SHARED_DEFINITION}")
else()
    message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

include_directories(${HEADERS})
include_directories(${HEADERS_INSTALL_DIR})
link_directories(${TARGET_INSTALL_DIR})

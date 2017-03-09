
if(DEBUG STREQUAL "yes")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DEBUG")
    else()
        message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endif()

if(BUILD_TYPE STREQUAL "shared")
    string(TOUPPER "${CMAKE_PROJECT_NAME}" PROJECT_SHARED_DEFINITION)
    set(PROJECT_SHARED_DEFINITION "${PROJECT_SHARED_DEFINITION}_SHARED_BUILD")
    if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        add_definitions("-D${PROJECT_SHARED_DEFINITION}")
    elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        add_definitions("/D${PROJECT_SHARED_DEFINITION}")
    else()
        message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
    add_library(${PROJECT_NAME} SHARED ${INPUT_FILES})
elseif(BUILD_TYPE STREQUAL "static")
    add_library(${PROJECT_NAME} STATIC ${INPUT_FILES})
elseif(BUILD_TYPE STREQUAL "executable")
    add_executable(${PROJECT_NAME} ${INPUT_FILES})
else()
    message(FATAL_ERROR "Build type not supported: #{BUILD_TYPE}")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    source_group("src" FILES ${SOURCE_FILES})
    source_group("include" FILES ${HEADERS})
endif()

if(NOT "${LIBRARIES}" STREQUAL "")
    target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${TARGET_INSTALL_DIR})
install(DIRECTORY include/generic/ DESTINATION ${HEADERS_INSTALL_DIR}/${PROJECT_NAME})
# if(EXISTS include/${BUILD_PLATFORM})
#     install(DIRECTORY include/${BUILD_PLATFORM}/ DESTINATION ${HEADERS_INSTALL_DIR}/${PROJECT_NAME})
# endif()

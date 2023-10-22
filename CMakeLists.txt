cmake_minimum_required(VERSION 3.27)

project(ecmascript
    VERSION 1.0
    DESCRIPTION "Implementation of standard ECMA-262 5.1 edition"
    LANGUAGES CXX
)

add_subdirectory(src)

include(CTest)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

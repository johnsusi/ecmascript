cmake_minimum_required(VERSION 3.27)

project(ecmascript
    VERSION 1.0
    DESCRIPTION "Implementation of standard ECMA-262 5.1 edition"
    LANGUAGES CXX
)

add_subdirectory(src)
add_subdirectory(tests)
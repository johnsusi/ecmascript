find_package(Catch2 3 REQUIRED)
add_executable(tests json/null.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

include(CTest)
include(Catch)
catch_discover_tests(tests)
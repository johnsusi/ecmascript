find_package(Catch2 3 REQUIRED)

include(Catch)

add_executable(tests json/null.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

catch_discover_tests(tests)
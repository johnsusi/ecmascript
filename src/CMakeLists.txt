add_library(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        ./ecmascript/ecmascript.h
        ./ecmascript/json/json.cpp ./ecmascript/json/json.hpp
    PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES
            ./ecmascript/ecmascript.h
            ./ecmascript/json/json.hpp            
)



target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-config
    FILE_SET HEADERS)

include(GNUInstallDirs)



# install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}-config DESTINATION share/${PROJECT_NAME}/cmake)

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}-config.cmake)
cmake_minimum_required(VERSION 3.1.0)

project(CryptNote VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(3rd/qblowfish/src)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(CryptNote WIN32
    changepassworddialog.cpp
    enterpassworddialog.cpp
    main.cpp
    mainwindow.cpp
    settings.cpp
    3rd/qblowfish/src/qblowfish.cpp
    resource.qrc
)

target_link_libraries(CryptNote Qt6::Widgets)

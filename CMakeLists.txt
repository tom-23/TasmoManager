cmake_minimum_required(VERSION 3.5)

project(TasmoManager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

add_subdirectory(lib/qmqtt)
include_directories("lib/qmqtt/src/mqtt")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets WebSockets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)

file(GLOB tasmomanager_SOURCES src/main.cpp
    src/app/*.cpp
    src/common/*.cpp
    src/gui/*.cpp)

file(GLOB tasmomanager_HEADERS
    src/app/*.h
    src/common/*.h
    src/gui/*.h)

file(GLOB tasmomanager_FORMS src/gui/*.ui)

set(tasmomanager_RESOURCES resources/resources.qrc)

set(tasmomanager_COMPILE
    ${tasmomanager_COMPILE} ${tasmomanager_SOURCES} ${tasmomanager_HEADERS} ${tasmomanager_FORMS} ${tasmomanager_RESOURCES})


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TasmoManager
        ${tasmomanager_COMPILE}
    )
else()
    if(ANDROID)
        add_library(TasmoManager SHARED
            ${tasmomanager_COMPILE}
        )
    else()
        add_executable(TasmoManager
            ${tasmomanager_COMPILE}
        )
    endif()
endif()

target_link_libraries(TasmoManager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network qmqtt)


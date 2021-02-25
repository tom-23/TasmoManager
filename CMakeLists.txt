cmake_minimum_required(VERSION 3.5)

project(TasmoManager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (APPLE)
    cmake_minimum_required(VERSION 3.1)
    set(TASMOMANAGER_EXECUTABLE_NAME "TasmoManager")
else()
    cmake_minimum_required(VERSION 2.8.12)
    set(TASMOMANAGER_EXECUTABLE_NAME ${PROJECT_NAME})
endif()

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

if(APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME "${TASMOMANAGER_EXECUTABLE_NAME}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.tasmomanager.tasmomanager")
    set(MACOSX_BUNDLE_INFO_STRING "Beginner friendly management UI for Tasmota flashed devices.")
    set(MACOSX_BUNDLE_ICON_FILE "icon.icns")

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(tasmomanager_COMPILE ${tasmomanager_COMPILE} "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")

    add_executable(${TASMOMANAGER_EXECUTABLE_NAME} MACOSX_BUNDLE ${tasmomanager_COMPILE})

    set_target_properties(${TASMOMANAGER_EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/misc/Info.plist")
else()
    add_executable(${TASMOMANAGER_EXECUTABLE_NAME} ${tasmomanager_COMPILE})
endif()

target_link_libraries(TasmoManager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network qmqtt)


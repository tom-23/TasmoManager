project(TasmoManager CXX)

#FINDING QT FILES
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets WebSockets Qmqtt Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network Qmqtt REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DVERSION="${CPACK_PACKAGE_VERSION}")
message( STATUS "VERSION: ${CPACK_PACKAGE_VERSION}" )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#SOURCE FILES
file(GLOB tasmomanager_SOURCES main.cpp
    app/*.cpp
    gui/*.cpp)

#HEADER FILES
file(GLOB tasmomanager_HEADERS
    app/*.h
    gui/*.h)

# UI FILES
file(GLOB tasmomanager_FORMS gui/*.ui)

# RESOURCE FILE
set(tasmomanager_RESOURCES ../resources/resources.qrc)

set(tasmomanager_COMPILE
    ${tasmomanager_COMPILE} ${tasmomanager_SOURCES} ${tasmomanager_HEADERS} ${tasmomanager_FORMS} ${tasmomanager_RESOURCES})

# GENERATE VERSION FILE
add_custom_target(
    generate_version_header
    ALL
    COMMAND ${CMAKE_COMMAND}
        -D INPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
        -D OUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/version.h
        -P ${CMAKE_CURRENT_SOURCE_DIR}/../version.cmake
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/version.h
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
    PROPERTIES GENERATED TRUE
)


set(EXECTYPE "")
if(WIN32 AND MINGW)
    set(EXECTYPE WIN32)
endif()

# ON MAC OS, SPECIFY APP BUNDLE INFO
option(BUILD_MACOSX_BUNDLE "Build macOS App Bundle" ON)
if(APPLE AND BUILD_MACOSX_BUNDLE)
    set( EXECTYPE MACOSX_BUNDLE )
    set(TASMOMANAGER_VERSION_REGEX "^v([0-9]+)\.([0-9]+)\.(.*)$")
    string(REGEX REPLACE "${TASMOMANAGER_VERSION_REGEX}" "\\1" CPACK_PACKAGE_VERSION_MAJOR "${tasmomanager_version}")
    string(REGEX REPLACE "${TASMOMANAGER_VERSION_REGEX}" "\\2" CPACK_PACKAGE_VERSION_MINOR "${tasmomanager_version}")
    string(REGEX REPLACE "${TASMOMANAGER_VERSION_REGEX}" "\\3" CPACK_PACKAGE_VERSION_PATCH "${tasmomanager_version}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.tasmomanager.tasmomanager")
    set(MACOSX_BUNDLE_INFO_STRING "Beginner friendly management UI for Tasmota flashed devices.")
    set(MACOSX_BUNDLE_ICON_FILE "icon.icns")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${horizon_version}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${horizon_version}")

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/../resources/icon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(tasmomanager_COMPILE ${tasmomanager_COMPILE} "${CMAKE_CURRENT_SOURCE_DIR}/../resources/icon.icns")
endif()

add_executable(${PROJECT_NAME} ${EXECTYPE} ${tasmomanager_COMPILE})
add_dependencies(${PROJECT_NAME} generate_version_header)

target_link_libraries(TasmoManager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Qmqtt)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "/wd4251" "/wd4127" "/wd4275" "/wd4200" "/nologo" "/J" "/Zi")
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:/MDd>")
else()
    if( APPLE AND BUILD_MACOSX_BUNDLE )
        set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/TasmoManagerBundleInfo.plist")
    endif()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wno-long-long -pedantic)
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:-DDEBUG>")
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:-O0>")
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:-fno-rtti>")
endif()

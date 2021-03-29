# Project files
file(GLOB tasmomanager_SOURCES
    main.cpp
    app/*.cpp
    misc/*.cpp
    gui/*.cpp
    gui/device_controls/*.cpp
    gui/device_options/*.cpp
    gui/wizards/*.cpp
    )

file(GLOB tasmomanager_FORMS
    gui/*.ui
    gui/device_controls/*.ui
    gui/device_options/*.ui
    gui/wizards/*.ui
    )

file(GLOB semver
    ../lib/semver.c/semver.c)

include_directories("../lib/semver.c")

set(tasmomanager_RESOURCES ../resources/resources.qrc)

set(tasmomanager_WINRC ../resources/win.rc)

# Qt include paths and definitions
include_directories(${CMAKE_CURRENT_BINARY_DIR} .)
include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})

set_source_files_properties(
    ${tasmomanager_SOURCES} PROPERTIES
    COMPILE_OPTIONS "${tasmomanager_pedantic_flags}")

qt5_wrap_ui(tasmomanager_FORMS_HEADERS ${tasmomanager_FORMS})
qt5_add_resources(tasmomanager_RESOURCES_RCC ${tasmomanager_RESOURCES})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(tasmomanager_COMPILE
    ${tasmomanager_COMPILE}
    ${tasmomanager_SOURCES}
    ${tasmomanager_FORMS_HEADERS}
    ${tasmomanager_RESOURCES_RCC}
    ${semver}
    )

if (NOT tasmomanager_version)
    message(FATAL_ERROR "Application version is unset")
endif()

if (WIN32)
    add_executable(${TASMOMANAGER_EXECUTABLE_NAME} WIN32 ${tasmomanager_WINRC} ${tasmomanager_COMPILE})
elseif(APPLE)
    set(TASMOMANAGER_VERSION_REGEX "^v([0-9]+)\.([0-9]+)\.(.*)$")
    string(REGEX REPLACE "${TASMOMANAGER_VERSION_REGEX}"
        "\\1" CPACK_PACKAGE_VERSION_MAJOR
        "${tasmomanager_version}"
        )
    string(REGEX REPLACE "${TASMOMANAGER_VERSION_REGEX}"
        "\\2" CPACK_PACKAGE_VERSION_MINOR
        "${tasmomanager_version}"
        )
    string(REGEX REPLACE "${TASMOMANAGER_VERSION_REGEX}"
        "\\3" CPACK_PACKAGE_VERSION_PATCH
        "${tasmomanager_version}"
        )

    set(MACOSX_BUNDLE_BUNDLE_NAME "${TASMOMANAGER_EXECUTABLE_NAME}")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.tombutcher.tasmomanager")
    set(MACOSX_BUNDLE_INFO_STRING "A beginner friendly desktop UI for Tasmota flashed devices.")
    set(MACOSX_BUNDLE_ICON_FILE "appicon.icns")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "${tasmomanager_version}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${tasmomanager_version}")

    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/../resources/appicon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(tasmomanager_COMPILE ${tasmomanager_COMPILE} "${CMAKE_CURRENT_SOURCE_DIR}/../resources/appicon.icns")

    add_executable(${TASMOMANAGER_EXECUTABLE_NAME} MACOSX_BUNDLE ${tasmomanager_COMPILE})
endif()

add_dependencies(${TASMOMANAGER_EXECUTABLE_NAME} generate_version_header)

# Qt modules
list(APPEND tasmomanager_qt_modules Network Qmqtt)
foreach (tasmomanager_qt_module ${tasmomanager_qt_modules})
    find_package(Qt5${tasmomanager_qt_module} REQUIRED)
    list(APPEND tasmomanager_LIBRARIES Qt5::${tasmomanager_qt_module})
endforeach()

# OpenSSL
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

set(OPENSSL_LIBRARIES ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES} )
list(APPEND tasmomanager_LIBRARIES ${OPENSSL_LIBRARIES})

# macros definitions for preprocessor and moc
set_target_properties(${TASMOMANAGER_EXECUTABLE_NAME} PROPERTIES COMPILE_DEFINITIONS "${tasmomanager_DEFINITIONS}")

# link
set_target_properties(${TASMOMANAGER_EXECUTABLE_NAME} PROPERTIES LINK_FLAGS "${tasmomanager_LINK_FLAGS}")
target_link_libraries(${TASMOMANAGER_EXECUTABLE_NAME} ${tasmomanager_LIBRARIES})

# install
if (MSVC)
    install(TARGETS ${TASMOMANAGER_EXECUTABLE_NAME}
        RUNTIME DESTINATION . COMPONENT Runtime
        LIBRARY DESTINATION . COMPONENT Runtime
        )
else()
    install(TARGETS ${TASMOMANAGER_EXECUTABLE_NAME}
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION bin COMPONENT Runtime
        )
endif()

if (APPLE)

    set_target_properties(${TASMOMANAGER_EXECUTABLE_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/../shared/tasmomanager.plist")

    set(plugin_dest_dir "${TASMOMANAGER_EXECUTABLE_NAME}.app/Contents/PlugIns")
    set(qtconf_dest_dir "${TASMOMANAGER_EXECUTABLE_NAME}.app/Contents/Resources")

    macro(install_qt5_plugin _qt_plugin_name _qt_plugins_var)
        get_target_property(_qt_plugin_path "${_qt_plugin_name}" LOCATION)
        if(EXISTS "${_qt_plugin_path}")
            get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
            get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
            get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)
            set(_qt_plugin_dest "${plugin_dest_dir}/${_qt_plugin_type}")
            install(FILES "${_qt_plugin_path}" DESTINATION "${_qt_plugin_dest}" COMPONENT Runtime)
            set(${_qt_plugins_var}
                "${${_qt_plugins_var}};\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${_qt_plugin_dest}/${_qt_plugin_file}")
        else()
            message(FATAL_ERROR "QT plugin ${_qt_plugin_name} not found")
        endif()
    endmacro()

    # Install needed Qt plugins
    foreach(_tasmomanager_qt_module ${tasmomanager_qt_modules} "Gui" "Widgets" "Qmqtt")
        set(_module_plugins "${Qt5${_tasmomanager_qt_module}_PLUGINS}")
        foreach(_plugin ${_module_plugins})
            install_qt5_plugin("${_plugin}" FIXUP_BUNDLE_QT_PLUGINS)
        endforeach()
    endforeach()

    # install a qt.conf file
    # this inserts some cmake code into the install script to write the file
    install(CODE "
        file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"[Paths]\nPlugins = PlugIns\nImports = Resources/qml\nQml2Imports = Resources/qml\n\")
        "
        COMPONENT Runtime
        )

    set(FIXUP_BUNDLE_APPS "\${CMAKE_INSTALL_PREFIX}/${TASMOMANAGER_EXECUTABLE_NAME}.app")

    get_property(_tasmomanager_installed_plugins GLOBAL PROPERTY TASMOMANAGER_INSTALLED_PLUGINS)

    # Directories to look for dependencies
    set(FIXUP_BUNDLE_DEP_DIRS "${CMAKE_BINARY_DIR};${QT_LIBRARY_DIRS};${Qt5Widgets_DIR}/../..")

    message(STATUS "Fixup app: ${FIXUP_BUNDLE_APPS}")
    message(STATUS "Fixup app plugins: ${_tasmomanager_installed_plugins}")
    message(STATUS "Fixup qt plugins: ${FIXUP_BUNDLE_QT_PLUGINS}")
    message(STATUS "Fixup dirs: ${FIXUP_BUNDLE_DEP_DIRS}")
    install(CODE "
        include(BundleUtilities)
        set(BU_CHMOD_BUNDLE_ITEMS ON)
        fixup_bundle(\"${FIXUP_BUNDLE_APPS}\" \"${FIXUP_BUNDLE_QT_PLUGINS};${_tasmomanager_installed_plugins}\" \"${FIXUP_BUNDLE_DEP_DIRS}\")
        verify_app(\"${FIXUP_BUNDLE_APPS}\")
        "
        COMPONENT Runtime
        )
endif()

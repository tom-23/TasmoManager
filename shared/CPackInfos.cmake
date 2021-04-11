if (UNIX)
    execute_process(
        COMMAND uname -m
        WORKING_DIRECTORY "."
        OUTPUT_VARIABLE CPACK_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
    set(CPACK_GENERATOR TGZ)

    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(CPACK_TARGET "")
        set(CPACK_GENERATOR ${CPACK_GENERATOR} DEB RPM)
    endif()

    # install application starter and icons
    install(
        FILES shared/TasmoManager.desktop
        DESTINATION share/applications
        )
    install(
        FILES resources/appiconx48.png
        DESTINATION share/icons/hicolor/48x48/apps
    )
    install(
        FILES resources/appicon.svg
        DESTINATION share/icons/hicolor/scalable/apps
    )

endif()
message(STATUS "Package: ${CPACK_GENERATOR}")
message(STATUS "Architecture: ${CPACK_ARCH}")

set(CPACK_PACKAGE_NAME "tasmomanager")
string(TOLOWER ${CPACK_PACKAGE_NAME} CPACK_PACKAGE_NAME)
set(CPACK_PACKAGE_VERSION "${DATE_VERSION}-${VCS_REVISION}")
set(CPACK_PACKAGE_VENDOR "Tom Butcher")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A beginner friendly desktop UI for Tasmota flashed devices.")
set(CPACK_PACKAGE_DESCRIPTION "A beginner friendly desktop UI for Tasmota flashed devices for Windows, macOS and Linux.")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
if (EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()

set(CPACK_STRIP_FILES 1)

include(CMakeDetermineSystem)

# Linux DEB (tested on debian stretch and buster)
# Architecture for package and file name are automatically detected
set(CPACK_DEBIAN_PACKAGE_SECTION "electronic")
# do not detect depencencies and versions automatically
# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# use deb stable packages without version explicitely to support also legacy installations
# local build uses Debian stable (currently buster)
# CI build (github actions or appveyor) uses Ubuntu 18.04 LTS until Debian "bullseye" becomes "stable"
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libfftw3-double3, libglu1-mesa")
message( "-- Depends: ${CPACK_DEBIAN_PACKAGE_DEPENDS}" )
# Debian buster Depends: "libc6 (>= 2.14), libfftw3-double3 (>= 3.3.5), libgcc1 (>= 1:3.0), libglu1-mesa | libglu1, libglx0, libopengl0, libqt5core5a (>= 5.11.0~rc1), libqt5gui5 (>= 5.8.0), libqt5opengl5 (>= 5.0.2), libqt5printsupport5 (>= 5.10.0), libqt5widgets5 (>= 5.4.0), libstdc++6 (>= 5), libusb-1.0-0 (>= 2:1.0.16)"
# Debian bullseye Depends: tbd
# Ubuntu 18.04 Depends:  "libc6 (>= 2.14), libfftw3-double3 (>= 3.3.5), libgcc1 (>= 1:3.0), libqt5core5a (>= 5.9.0~beta), libqt5gui5 (>= 5.8.0), libqt5printsupport5 (>= 5.2.0), libqt5widgets5 (>= 5.4.0), libstdc++6 (>= 5), libusb-1.0-0 (>= 2:1.0.16)"
# Ubuntu 20.04 Depends:  "libc6 (>= 2.29), libfftw3-double3 (>= 3.3.5), libgcc-s1 (>= 3.0), libqt5core5a (>= 5.12.2), libqt5gui5 (>= 5.8.0) | libqt5gui5-gles (>= 5.8.0), libqt5printsupport5 (>= 5.10.0), libqt5widgets5 (>= 5.4.0), libstdc++6 (>= 5), libusb-1.0-0 (>= 2:1.0.16)

set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

# Linux RPM (not tested on debian)
# Architecture for package and file name are automatically detected
set(CPACK_RPM_PACKAGE_RELOCATABLE NO)
set(CPACK_RPM_PACKAGE_LICENSE "GPLv2+")
set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
# set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_BINARY_DIR}/changelog")
set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}-1_${CPACK_TARGET}${CPACK_ARCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ".")
SET(CPACK_OUTPUT_FILE_PREFIX packages)

include(CPack)
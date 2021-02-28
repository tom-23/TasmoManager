if(APPLE)
    execute_process(
        COMMAND uname -m
        WORKING_DIRECTORY "."
       OUTPUT_VARIABLE CPACK_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CPACK_GENERATOR TGZ)
    set(CPACK_TARGET "osx_")

elseif(WIN32)
    set(CPACK_TARGET "win_")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

    set(CPACK_GENERATOR ZIP NSIS)
    if ((MSVC AND CMAKE_GENERATOR MATCHES "Win64+") OR (CMAKE_SIZEOF_VOID_P EQUAL 8))
        set(CPACK_ARCH "amd64")
    else()
        set(CPACK_ARCH "x86")
    endif()
endif()

message(STATUS "Package: ${CPACK_GENERATOR}")
message(STATUS "Architecture: ${CPACK_ARCH}")

set(CPACK_PACKAGE_NAME "tasmomanager")
string(TOLOWER ${CPACK_PACKAGE_NAME} CPACK_PACKAGE_NAME)
set(CPACK_PACKAGE_VERSION ${tasmomanager_version})
set(CPACK_PACKAGE_VENDOR "Tom Butcher")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A beginner friendly desktop UI for Tasmota flashed devices.")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/readme.md")
if (EXISTS "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()

set(CPACK_STRIP_FILES 1)

include(CMakeDetermineSystem)

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}-1_${CPACK_TARGET}${CPACK_ARCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY ".")
SET(CPACK_OUTPUT_FILE_PREFIX packages)

include(CPack)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
include(InstallRequiredSystemLibraries)

cpack_add_install_type(Full DISPLAY_NAME "All")

set(VERSION ${CPACK_PACKAGE_VERSION})   
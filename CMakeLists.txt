project(tasmomanager)

if (APPLE)
    cmake_minimum_required(VERSION 3.1)
    set(TASMOMANAGER_EXECUTABLE_NAME "TasmoManager")
else()
    cmake_minimum_required(VERSION 2.8.12)
    set(TASMOMANAGER_EXECUTABLE_NAME ${PROJECT_NAME})
endif()

# C++11
if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)
    set(CMAKE_CXX_STANDARD 11)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(TASMOMANAGER_DEBUG ON)
    add_definitions( -DTASMOMANAGER_DEBUG  )
endif()

OPTION(PEDANTIC "Enable all compiler warnings" OFF)

# Options (cmake -LH)
OPTION(WITH_TESTS "Run test cases from command line" ${TASMOMANAGER_DEBUG})
OPTION(WITH_PLUGINS "Compile plugins" ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets 5.8.0 REQUIRED)
find_package(OpenSSL REQUIRED)
list(APPEND tasmomanager_LIBRARIES Qt5::Widgets OpenSSL::SSL)

# Be more strict while compiling debugging version
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")
    set(CMAKE_CXX_FLAGS_DEBUG
        "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wall -pedantic -Wfloat-equal -Woverloaded-virtual -Wundef -Wno-inconsistent-missing-destructor-override")
endif()

if (PEDANTIC)
    if (CMAKE_COMPILER_IS_GNUCXX)
        list(APPEND tasmomanager_pedantic_flags
            -Wextra -Wall
            -Wsuggest-override
            -Wlogical-op
            -Wnoexcept
            -Wstrict-null-sentinel
            )
    else()
        list(APPEND tasmomanager_pedantic_flags
            -Weverything
            -Winconsistent-missing-override
            )

        list(APPEND tasmomanager_pedantic_flags
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-disabled-macro-expansion
            -Wno-exit-time-destructors
            -Wno-extra-semi-stmt
            -Wno-redundant-parens
            -Wno-shadow-field
            -Wno-shadow-field-in-constructor
            -Wno-shorten-64-to-32
            -Wno-sign-conversion
            -Wno-suggest-destructor-override
            -Wno-unknown-warning-option
            -Wno-used-but-marked-unused
            -Wno-weak-vtables
            -fcomment-block-commands=retval
            )
    endif()

    list(APPEND tasmomanager_pedantic_flags
        -pedantic
        -Wcast-align
        -Wctor-dtor-privacy
        -Wdisabled-optimization
        -Wformat=2
        -Winit-self
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wstrict-overflow=4
        -Wundef
        )

    list(APPEND tasmomanager_pedantic_flags
        -Wno-padded
        -Wno-switch-enum
        )

    # Disable Q_OBJECT macro warnings.
    list(APPEND tasmomanager_pedantic_flags
        -Wno-unused-member-function
        )
endif()

include(version.cmake)
message(STATUS "Building Tasmomanager version ${tasmomanager_version}.")

# Generate version.h from current git commit if available.
add_custom_target(
    generate_version_header
    ALL
    COMMAND ${CMAKE_COMMAND}
        -D INPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
        -D OUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/src/version.h
        -P ${CMAKE_CURRENT_SOURCE_DIR}/version_header.cmake
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/src/version.h
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/src/version.h
    PROPERTIES GENERATED TRUE
)

if (MSVC)
    include(InstallRequiredSystemLibraries)
endif()

add_definitions( -DQT_NO_CAST_TO_ASCII )

set_property(GLOBAL PROPERTY TASMOMANAGER_INSTALLED_PLUGINS)

add_subdirectory(src)

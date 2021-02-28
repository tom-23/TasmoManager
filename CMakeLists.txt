cmake_minimum_required(VERSION 3.5)

project(TasmoManager)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default compile options
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

INCLUDE_DIRECTORIES(".")


include(cmake/CPackInfos.cmake)

if(MSVC)
  add_compile_options(/W4 -D_CRT_SECURE_NO_WARNINGS)
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-Wno-psabi)
endif()

get_directory_property( CompOpts COMPILE_OPTIONS )
message( "-- COMPILE_OPTIONS: ${CompOpts}" )
get_directory_property( CompDefs COMPILE_DEFINITIONS )
message( "-- COMPILE_DEFINITIONS: ${CompDefs}" )

add_subdirectory(src)

if (WIN32)
    install(
	FILES CHANGELOG.md LICENSE README.md
	DESTINATION "."
    )
endif()

#ADDING QMQTT TO PROJECT
add_subdirectory(lib/qmqtt)
include_directories("lib/qmqtt/src/mqtt")

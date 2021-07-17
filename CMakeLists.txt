cmake_minimum_required(VERSION 3.0)

set(VER 1.0.1)

project(RunHiddenConsole VERSION ${VER})

if (NOT WIN32)
    message(FATAL_ERROR "This program only works on WIN32 platform.")
endif()

include(CheckIncludeFiles)
include(GNUInstallDirs)
include(CheckSymbolExists)

CHECK_INCLUDE_FILES(getopt.h HAVE_GETOPT_H)
check_symbol_exists(printf_s stdio.h HAVE_PRINTF_S)
check_symbol_exists(GetCommandLineW windows.h HAVE_GETCOMMANDLINEW)
check_symbol_exists(CommandLineToArgvW windows.h HAVE_COMMANDLINETOARGVW)
check_symbol_exists(fprintf_s stdio.h HAVE_FPRINTF_S)

if ("${HAVE_GETOPT_H}" STREQUAL "")
    add_library(getopt STATIC getopt/getopt_core.h getopt/getopt_ext.h getopt/getopt_int.h
    getopt/getopt.c getopt/getopt.h getopt/getopt1.c)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/getopt")
endif()

add_compile_definitions(HAVE_RHC_CONFIG_H)
if (MSVC)
    add_compile_options(/utf-8)
endif()
configure_file(config.h.in config.h)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(RunHiddenConsole "${CMAKE_CURRENT_BINARY_DIR}/config.h" src/main.cpp
src/util.h src/util.cpp)

if (TARGET getopt)
    target_link_libraries(RunHiddenConsole getopt)
endif()

install(TARGETS RunHiddenConsole)

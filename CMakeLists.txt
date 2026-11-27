#
# GRANULAR SYNTH — Milestone A0: framework-free build and headless test skeleton.
#
# Authorized scope: owner authorization comment on PR #6 (2026-11-27), "Milestone A0 only".
# This build intentionally contains NO audio framework, NO plugin/host targets and NO DSP.
#
# Deliberately NOT decided here (see docs/BUILD_AND_TOOLCHAIN_RECORD.md):
#   * Q-T01 framework (JUCE or otherwise)      -> no framework dependency exists
#   * Q-T03 macOS deployment target / Universal 2 -> left unset and configurable
#   * Q-L05 third-party dependencies           -> zero third-party dependencies
#
cmake_minimum_required(VERSION 3.22)

# ---------------------------------------------------------------------------
# Apple architecture / deployment target: UNSET BY DEFAULT ON PURPOSE.
#
# CMake initializes the Apple toolchain during project()/enable_language(), so these
# caller-configurable cache inputs must be applied BEFORE project(). Q-T03 remains open:
# empty values make no Universal 2 or minimum-macOS commitment.
# ---------------------------------------------------------------------------
set(GS_OSX_ARCHITECTURES "" CACHE STRING
    "Apple architectures to build (e.g. 'arm64;x86_64'). EMPTY = host only; no Universal 2 claim. Q-T03 open.")
set(GS_OSX_DEPLOYMENT_TARGET "" CACHE STRING
    "macOS deployment target (e.g. '12.0'). EMPTY = host default; no supported-OS claim. Q-T03 open.")

if(GS_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "${GS_OSX_ARCHITECTURES}" CACHE STRING "" FORCE)
endif()
if(GS_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "${GS_OSX_DEPLOYMENT_TARGET}" CACHE STRING "" FORCE)
endif()

project(GranularSynth
    VERSION 0.0.1
    DESCRIPTION "GRANULAR SYNTH — framework-free core (Milestone A0 skeleton)"
    LANGUAGES CXX)

if(NOT APPLE)
    message(WARNING
        "GRANULAR SYNTH targets macOS (REQ-PLAT-001). Configuring on a non-Apple host is "
        "unsupported and only the framework-free core/test targets may work.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ---------------------------------------------------------------------------
# Language level
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(GS_OSX_ARCHITECTURES)
    message(STATUS "GS: CMAKE_OSX_ARCHITECTURES set by caller to '${GS_OSX_ARCHITECTURES}' "
                   "(caller-supplied; not an approved Q-T03 decision)")
else()
    message(STATUS "GS: architectures unset -> host architecture only (Q-T03 open; no Universal 2 claim)")
endif()

if(GS_OSX_DEPLOYMENT_TARGET)
    message(STATUS "GS: CMAKE_OSX_DEPLOYMENT_TARGET set by caller to '${GS_OSX_DEPLOYMENT_TARGET}' "
                   "(caller-supplied; not an approved Q-T03 decision)")
else()
    message(STATUS "GS: deployment target unset -> host default (Q-T03 open; no supported-OS claim)")
endif()

# ---------------------------------------------------------------------------
# Shared compile settings
# ---------------------------------------------------------------------------
option(GS_WERROR "Treat compiler warnings as errors (enabled in CI presets)" OFF)
option(GS_BUILD_TESTS "Build the headless core test target" ON)

include(GsWarnings)
include(GsDeterminism)

add_subdirectory(src)

if(GS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "GS: configured ${PROJECT_NAME} ${PROJECT_VERSION} "
               "(C++${CMAKE_CXX_STANDARD}, ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})")

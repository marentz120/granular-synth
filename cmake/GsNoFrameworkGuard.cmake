# Enforces Q-T02 / DEC-011 (proposed): the core domain must stay framework-independent.
#
# Run in script mode as a CTest test:
#   cmake -D GS_GUARD_DIR=<dir> -P cmake/GsNoFrameworkGuard.cmake
#
# This is a mechanical include-text guard, not a linker-level proof. It exists so that a
# framework dependency cannot drift into the core silently between milestones. It does not
# approve any framework decision; Q-T01 remains open.

if(NOT DEFINED GS_GUARD_DIR)
    message(FATAL_ERROR "GsNoFrameworkGuard: GS_GUARD_DIR must be defined")
endif()

if(NOT IS_DIRECTORY "${GS_GUARD_DIR}")
    message(FATAL_ERROR "GsNoFrameworkGuard: '${GS_GUARD_DIR}' is not a directory")
endif()

# Forbidden include fragments: audio frameworks, plug-in SDKs and host/OS audio APIs.
set(GS_FORBIDDEN_INCLUDES
    "JuceHeader.h"
    "juce_"
    "<juce"
    "AudioToolbox/"
    "AudioUnit/"
    "CoreAudio/"
    "AVFoundation/"
    "pluginterfaces/"
    "public.sdk/"
    "clap/"
    "IPlug")

file(GLOB_RECURSE _gs_sources "${GS_GUARD_DIR}/*.h" "${GS_GUARD_DIR}/*.hpp" "${GS_GUARD_DIR}/*.cpp")

set(_gs_violations "")
foreach(_file IN LISTS _gs_sources)
    file(STRINGS "${_file}" _include_lines REGEX "^[ \t]*#[ \t]*include")
    foreach(_line IN LISTS _include_lines)
        foreach(_bad IN LISTS GS_FORBIDDEN_INCLUDES)
            string(FIND "${_line}" "${_bad}" _found)
            if(NOT _found EQUAL -1)
                list(APPEND _gs_violations "${_file}: ${_line}  [matched '${_bad}']")
            endif()
        endforeach()
    endforeach()
endforeach()

list(LENGTH _gs_sources _gs_file_count)

if(_gs_violations)
    message("GsNoFrameworkGuard: FRAMEWORK DEPENDENCY DETECTED IN CORE")
    foreach(_v IN LISTS _gs_violations)
        message("  ${_v}")
    endforeach()
    message(FATAL_ERROR
        "GsNoFrameworkGuard: core must remain framework-independent (Q-T02 / DEC-011). "
        "Framework code belongs in the host adapter layer, which is not authorized yet.")
endif()

message(STATUS "GsNoFrameworkGuard: OK — ${_gs_file_count} core source file(s) scanned, no framework includes")

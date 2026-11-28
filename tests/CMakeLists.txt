# Headless core test target.
#
# "Headless" means: no DAW host, no plug-in wrapper, no audio device, no UI, no framework.
# The executable links only gs_core and the internal harness, so it can run in CI on a
# machine with no audio hardware.

add_library(gs_test_support STATIC
    support/src/Harness.cpp)

target_include_directories(gs_test_support
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/support/include>)

target_link_libraries(gs_test_support
    PRIVATE
        gs_warnings
    PUBLIC
        gs_determinism)

# RtGuard.cpp is compiled into the executable rather than the static library: it replaces
# global operator new/delete, and such a replacement must be in the final link unit to be
# reliably picked up.
add_executable(gs_core_tests
    support/src/TestMain.cpp
    support/src/RtGuard.cpp
    unit/SmokeTest.cpp
    unit/HarnessTest.cpp
    unit/RtGuardTest.cpp)

target_link_libraries(gs_core_tests
    PRIVATE
        gs_core
        gs_test_support
        gs_warnings)

# ---------------------------------------------------------------------------
# CTest registration
# ---------------------------------------------------------------------------

# Whole-suite run: the authoritative pass/fail for the milestone.
add_test(NAME suite.all COMMAND gs_core_tests)
set_tests_properties(suite.all PROPERTIES LABELS "unit")

# Per-suite entries, so a failure points at an area instead of the whole binary.
foreach(_suite IN ITEMS core harness rtGuard)
    add_test(NAME suite.${_suite} COMMAND gs_core_tests --filter ${_suite}.)
    set_tests_properties(suite.${_suite} PROPERTIES LABELS "unit")
endforeach()

# Test discovery must not silently become empty.
add_test(NAME suite.list_is_not_empty COMMAND gs_core_tests --list)
set_tests_properties(suite.list_is_not_empty PROPERTIES
    LABELS "unit"
    PASS_REGULAR_EXPRESSION "core\\.")

# Mechanical guard: the core must contain no framework/plug-in SDK includes (Q-T02/DEC-011).
add_test(NAME guard.no_framework_in_core
    COMMAND ${CMAKE_COMMAND}
        -D "GS_GUARD_DIR=${CMAKE_SOURCE_DIR}/src/core"
        -P "${CMAKE_SOURCE_DIR}/cmake/GsNoFrameworkGuard.cmake")
set_tests_properties(guard.no_framework_in_core PROPERTIES LABELS "guard")

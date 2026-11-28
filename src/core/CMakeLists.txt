# gs_core — framework-independent core (Q-T02 / DEC-011, both still Proposed).
#
# Hard rule for this target: no audio framework, no plug-in SDK, no host/OS audio API. The
# rule is enforced mechanically by the guard test registered in tests/CMakeLists.txt.
#
# Milestone A0 content is deliberately limited to build identity. The rt/, math/, dsp/ and
# rng/ submodules named in DSP_ARCHITECTURE.md §3 are not implemented yet.

add_library(gs_core STATIC
    src/Version.cpp)

target_include_directories(gs_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(gs_core
    PRIVATE
        gs_warnings
    PUBLIC
        gs_determinism)

target_compile_definitions(gs_core
    PRIVATE
        GS_CORE_VERSION_STRING="${PROJECT_VERSION}")

set_target_properties(gs_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON)

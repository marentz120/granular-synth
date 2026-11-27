# Floating-point determinism settings.
#
# GS-NFR-005 requires that an offline render with a fixed seed be bit-reproducible, and
# GS-PLT-004 requires standalone/plug-in output equality. Fast-math style optimisations break
# both by allowing the compiler to reassociate and contract float operations, so they are
# refused at configure time rather than discovered later in a golden-render diff.
#
# Scope note: this only constrains what the compiler may do. It is not a claim that the engine
# is deterministic, and it says nothing about cross-architecture bit-equality, which remains an
# open interpretation question against GS-PLT-004.

add_library(gs_determinism INTERFACE)

# Disable floating-point contraction (e.g. fusing a*b+c into fma) so results do not depend on
# whether the compiler happened to find a contraction opportunity.
target_compile_options(gs_determinism INTERFACE -ffp-contract=off)

set(_gs_forbidden_fp_flags "-ffast-math" "-Ofast" "-funsafe-math-optimizations"
                           "-freciprocal-math" "-fassociative-math" "-ffinite-math-only")

foreach(_scope IN ITEMS "" "_DEBUG" "_RELEASE" "_RELWITHDEBINFO" "_MINSIZEREL")
    set(_flags "${CMAKE_CXX_FLAGS${_scope}}")
    foreach(_bad IN LISTS _gs_forbidden_fp_flags)
        string(FIND "${_flags}" "${_bad}" _found)
        if(NOT _found EQUAL -1)
            message(FATAL_ERROR
                "GS: '${_bad}' found in CMAKE_CXX_FLAGS${_scope}. Fast-math style flags are refused "
                "because they break reproducible offline renders (GS-NFR-005) and standalone/plug-in "
                "output equality (GS-PLT-004).")
        endif()
    endforeach()
endforeach()

message(STATUS "GS: determinism flags applied (-ffp-contract=off); fast-math flags refused")

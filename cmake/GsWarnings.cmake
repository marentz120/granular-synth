# Shared warning configuration, exposed as an INTERFACE target so every GS target opts in
# explicitly rather than relying on directory-scope flag inheritance.

add_library(gs_warnings INTERFACE)

target_compile_options(gs_warnings INTERFACE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wdouble-promotion
    -Wformat=2)

if(GS_WERROR)
    target_compile_options(gs_warnings INTERFACE -Werror)
    message(STATUS "GS: warnings-as-errors ENABLED")
else()
    message(STATUS "GS: warnings-as-errors disabled (enable with -DGS_WERROR=ON; CI presets enable it)")
endif()

// Milestone A0 smoke test: proves the headless pipeline configure -> build -> link -> run
// works against the framework-free core.
//
// Traceability note: A0 implements no product requirement, so these tests do not claim any
// GS-* requirement as satisfied. They support QA-002 (independently testable components) and
// QA-004 (reproducible builds and automated tests) at infrastructure level only.

#include "gs/core/Version.h"
#include "gs/test/Harness.h"

#include <cstring>

GS_TEST(core, versionStringIsPopulated) {
    const char* version = gs::core::versionString();
    GS_REQUIRE(version != nullptr);
    GS_CHECK_MSG(std::strlen(version) > 0, "version string must not be empty");
    GS_CHECK_MSG(std::strcmp(version, "0.0.0-unconfigured") != 0,
                 "version should come from the build system, not the fallback");
}

GS_TEST(core, frameworkIndependenceFlagIsSet) {
    // Mirrors the Q-T02 / DEC-011 core-boundary intent. The mechanical include guard in
    // cmake/GsNoFrameworkGuard.cmake is the actual enforcement; this only makes the
    // invariant visible in test output.
    GS_CHECK(gs::core::isFrameworkIndependentBuild());
}

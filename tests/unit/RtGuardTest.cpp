// Tests for the test-only realtime allocation guard.
//
// These verify the instrumentation itself, not the engine: there is no audio callback in
// Milestone A0. They must not be read as evidence of realtime safety (see RtGuard.h for the
// documented limitations).

#include "gs/test/Harness.h"
#include "gs/test/RtGuard.h"

#include <cstddef>

namespace {

// Defeats optimisation of the allocation under test without pulling in <memory> machinery.
volatile void* g_sink = nullptr;

} // namespace

GS_TEST(rtGuard, inactiveByDefault) {
    GS_CHECK(!gs::test::realtimeSectionActive());
}

GS_TEST(rtGuard, activeInsideScopeAndClearedAfter) {
    {
        const gs::test::RealtimeSectionGuard guard;
        GS_CHECK(gs::test::realtimeSectionActive());
    }
    GS_CHECK(!gs::test::realtimeSectionActive());
}

GS_TEST(rtGuard, nestedScopesRemainActiveUntilOutermostExits) {
    {
        const gs::test::RealtimeSectionGuard outer;
        {
            const gs::test::RealtimeSectionGuard inner;
            GS_CHECK(gs::test::realtimeSectionActive());
        }
        GS_CHECK_MSG(gs::test::realtimeSectionActive(),
                     "inner scope exit must not clear the outer realtime scope");
    }
    GS_CHECK(!gs::test::realtimeSectionActive());
}

GS_TEST(rtGuard, cleanScopeRecordsNoViolation) {
    gs::test::resetRealtimeViolations();
    {
        const gs::test::RealtimeSectionGuard guard;
        // Arithmetic only: no allocation, no deallocation.
        double accumulator = 0.0;
        for (int i = 0; i < 64; ++i) {
            accumulator += static_cast<double>(i) * 0.5;
        }
        GS_CHECK(accumulator > 0.0);
    }
    GS_CHECK_MSG(gs::test::realtimeViolationCount() == 0,
                 "an allocation-free guarded scope must report zero violations");
}

GS_TEST(rtGuard, allocationInsideScopeIsDetected) {
    gs::test::resetRealtimeViolations();
    {
        const gs::test::RealtimeSectionGuard guard;
        auto* buffer = new int[16];
        g_sink = buffer;
        delete[] buffer;
    }
    // One allocation plus one deallocation are both observed.
    GS_CHECK_MSG(gs::test::realtimeViolationCount() >= 2,
                 "new[] and delete[] inside a guarded scope must both be recorded");
    gs::test::resetRealtimeViolations();
}

GS_TEST(rtGuard, allocationOutsideScopeIsIgnored) {
    gs::test::resetRealtimeViolations();
    auto* buffer = new int[16];
    g_sink = buffer;
    delete[] buffer;
    GS_CHECK_MSG(gs::test::realtimeViolationCount() == 0,
                 "allocation outside a guarded scope must not be recorded");
}

GS_TEST(rtGuard, resetClearsCounter) {
    {
        const gs::test::RealtimeSectionGuard guard;
        auto* buffer = new int[4];
        g_sink = buffer;
        delete[] buffer;
    }
    GS_REQUIRE(gs::test::realtimeViolationCount() > 0);
    gs::test::resetRealtimeViolations();
    GS_CHECK(gs::test::realtimeViolationCount() == 0);
}

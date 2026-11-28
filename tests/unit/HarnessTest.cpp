// Self-tests for the internal harness. The harness has no third-party dependency, so its own
// correctness has to be demonstrated rather than assumed.

#include "gs/test/Harness.h"

GS_TEST(harness, registrationIsVisible) {
    // At least the tests in this file plus the smoke tests must be registered.
    GS_CHECK(gs::test::testCount() >= 3);
}

GS_TEST(harness, registeredCasesHaveNamesAndFunctions) {
    const std::size_t total = gs::test::testCount();
    GS_REQUIRE(total > 0);

    for (std::size_t i = 0; i < total; ++i) {
        const gs::test::TestCase& testCase = gs::test::testAt(i);
        GS_CHECK(testCase.suite != nullptr);
        GS_CHECK(testCase.name != nullptr);
        GS_CHECK(testCase.function != nullptr);
    }
}

GS_TEST(harness, passingChecksDoNotMarkFailure) {
    GS_CHECK(true);
    GS_CHECK_MSG(1 + 1 == 2, "arithmetic sanity");
    GS_CHECK(!gs::test::currentTestFailed());
}

// Minimal headless test harness.
//
// The Milestone A0 authorization requires an internal harness with zero third-party
// dependencies (Q-L05 is open, so no external test framework may be introduced). This is
// intentionally small: registration, assertions, filtering and an exit code. It is not a
// replacement for the Phase 5 test plan.
//
// Registration uses a fixed-capacity table so that no heap allocation happens during static
// initialisation, which keeps the realtime allocation guard in RtGuard.h meaningful.

#ifndef GS_TEST_HARNESS_H
#define GS_TEST_HARNESS_H

#include <cstddef>

namespace gs::test {

using TestFunction = void (*)();

struct TestCase {
    const char* suite;
    const char* name;
    TestFunction function;
};

/// Maximum number of registered tests. Static capacity avoids allocation at static-init time.
constexpr std::size_t maxTestCount = 256;

/// Registers one test case at static-initialisation time.
class TestRegistrar {
public:
    TestRegistrar(const char* suite, const char* name, TestFunction function) noexcept;
};

std::size_t testCount() noexcept;
const TestCase& testAt(std::size_t index) noexcept;

/// Records a failed assertion against the currently running test.
void recordAssertionFailure(const char* file, int line, const char* expression, const char* detail) noexcept;

bool currentTestFailed() noexcept;

/// Runs registered tests. Supports `--list` and `--filter <substring>` (matched against
/// "suite.name"). Returns 0 when every selected test passed, 1 otherwise.
int runAll(int argc, char** argv);

} // namespace gs::test

#define GS_TEST(suiteIdentifier, testIdentifier)                                                             \
    static void gs_test_fn_##suiteIdentifier##_##testIdentifier();                                           \
    static const ::gs::test::TestRegistrar gs_test_reg_##suiteIdentifier##_##testIdentifier(                 \
        #suiteIdentifier, #testIdentifier, &gs_test_fn_##suiteIdentifier##_##testIdentifier);                \
    static void gs_test_fn_##suiteIdentifier##_##testIdentifier()

/// Non-fatal check: records a failure and continues the test body.
#define GS_CHECK(expression)                                                                                 \
    do {                                                                                                     \
        if (!(expression)) {                                                                                 \
            ::gs::test::recordAssertionFailure(__FILE__, __LINE__, #expression, nullptr);                    \
        }                                                                                                    \
    } while (false)

/// Non-fatal check with an explanatory message.
#define GS_CHECK_MSG(expression, detail)                                                                     \
    do {                                                                                                     \
        if (!(expression)) {                                                                                 \
            ::gs::test::recordAssertionFailure(__FILE__, __LINE__, #expression, (detail));                   \
        }                                                                                                    \
    } while (false)

/// Fatal check: records a failure and abandons the rest of the test body.
#define GS_REQUIRE(expression)                                                                               \
    do {                                                                                                     \
        if (!(expression)) {                                                                                 \
            ::gs::test::recordAssertionFailure(__FILE__, __LINE__, #expression,                              \
                                               "required condition failed");                                 \
            return;                                                                                          \
        }                                                                                                    \
    } while (false)

#endif // GS_TEST_HARNESS_H

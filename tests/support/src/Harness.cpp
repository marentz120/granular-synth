#include "gs/test/Harness.h"

#include <cstdio>
#include <cstring>

namespace gs::test {
namespace {

struct Registry {
    TestCase cases[maxTestCount] = {};
    std::size_t count = 0;
    std::size_t overflow = 0;
};

Registry& registry() noexcept {
    static Registry instance;
    return instance;
}

bool& failureFlag() noexcept {
    static bool failed = false;
    return failed;
}

std::size_t& failureCount() noexcept {
    static std::size_t failures = 0;
    return failures;
}

bool matchesFilter(const TestCase& testCase, const char* filter) noexcept {
    if (filter == nullptr) {
        return true;
    }

    char qualified[256];
    std::snprintf(qualified, sizeof(qualified), "%s.%s", testCase.suite, testCase.name);
    return std::strstr(qualified, filter) != nullptr;
}

} // namespace

TestRegistrar::TestRegistrar(const char* suite, const char* name, TestFunction function) noexcept {
    Registry& reg = registry();
    if (reg.count >= maxTestCount) {
        ++reg.overflow;
        return;
    }
    reg.cases[reg.count] = TestCase{suite, name, function};
    ++reg.count;
}

std::size_t testCount() noexcept {
    return registry().count;
}

const TestCase& testAt(std::size_t index) noexcept {
    return registry().cases[index];
}

void recordAssertionFailure(const char* file, int line, const char* expression,
                            const char* detail) noexcept {
    failureFlag() = true;
    ++failureCount();
    std::fprintf(stderr, "    FAIL %s:%d: %s%s%s\n", file, line, expression,
                 detail != nullptr ? " — " : "", detail != nullptr ? detail : "");
}

bool currentTestFailed() noexcept {
    return failureFlag();
}

int runAll(int argc, char** argv) {
    const char* filter = nullptr;
    bool listOnly = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--list") == 0) {
            listOnly = true;
        } else if (std::strcmp(argv[i], "--filter") == 0 && i + 1 < argc) {
            filter = argv[++i];
        } else {
            std::fprintf(stderr, "usage: %s [--list] [--filter <substring>]\n", argv[0]);
            return 2;
        }
    }

    Registry& reg = registry();
    if (reg.overflow > 0) {
        std::fprintf(stderr, "harness: %zu test(s) not registered — raise gs::test::maxTestCount\n",
                     reg.overflow);
        return 2;
    }

    if (listOnly) {
        for (std::size_t i = 0; i < reg.count; ++i) {
            std::printf("%s.%s\n", reg.cases[i].suite, reg.cases[i].name);
        }
        return 0;
    }

    std::size_t selected = 0;
    std::size_t passed = 0;
    std::size_t failed = 0;

    for (std::size_t i = 0; i < reg.count; ++i) {
        const TestCase& testCase = reg.cases[i];
        if (!matchesFilter(testCase, filter)) {
            continue;
        }

        ++selected;
        std::printf("[ RUN  ] %s.%s\n", testCase.suite, testCase.name);

        failureFlag() = false;
        const std::size_t failuresBefore = failureCount();
        testCase.function();
        const bool testFailed = failureCount() != failuresBefore;

        if (testFailed) {
            ++failed;
            std::printf("[ FAIL ] %s.%s\n", testCase.suite, testCase.name);
        } else {
            ++passed;
            std::printf("[  OK  ] %s.%s\n", testCase.suite, testCase.name);
        }
    }

    if (selected == 0) {
        std::fprintf(stderr, "harness: no tests matched filter '%s'\n",
                     filter != nullptr ? filter : "(none)");
        return 2;
    }

    std::printf("\n%zu selected, %zu passed, %zu failed (%zu registered)\n", selected, passed, failed,
                reg.count);
    return failed == 0 ? 0 : 1;
}

} // namespace gs::test

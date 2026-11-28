// Test-only realtime allocation guard — skeleton.
//
// Purpose: give the test target a way to assert that a scope performed no heap allocation,
// supporting REQ-AUD-002 / GS-NFR-001 ("the audio callback must not perform routine heap
// allocation"). Milestone A0 provides the instrumentation only; there is no audio callback
// and no DSP to guard yet.
//
// DELIBERATE LIMITATIONS — this is NOT a realtime-safety proof, and the handoff forbids
// claiming one:
//   * Detects only global operator new / operator delete in this executable. Direct malloc,
//     calloc, realloc, strdup, mmap and allocations inside already-compiled system libraries
//     are NOT detected.
//   * Does not detect locks, blocking syscalls, file I/O, logging or unbounded work. Those
//     need separate instrumentation (Q-T09: allocation interposer and/or Clang RTSan).
//   * Counting is per-thread. A violation on another thread is not observed here.
//
// Q-T09 (RT-safety tooling) remains PROPOSED; adopting RTSan or a malloc interposer is a
// later decision and is not made by this file.

#ifndef GS_TEST_RTGUARD_H
#define GS_TEST_RTGUARD_H

#include <cstddef>

namespace gs::test {

/// Marks the enclosing scope as realtime for the current thread. Nesting is supported.
/// Entering a guarded scope does not reset the violation counter.
class RealtimeSectionGuard {
public:
    RealtimeSectionGuard() noexcept;
    ~RealtimeSectionGuard() noexcept;

    RealtimeSectionGuard(const RealtimeSectionGuard&) = delete;
    RealtimeSectionGuard& operator=(const RealtimeSectionGuard&) = delete;
    RealtimeSectionGuard(RealtimeSectionGuard&&) = delete;
    RealtimeSectionGuard& operator=(RealtimeSectionGuard&&) = delete;
};

/// True while the current thread is inside at least one RealtimeSectionGuard scope.
bool realtimeSectionActive() noexcept;

/// Number of allocation/deallocation events observed on this thread while a guarded scope
/// was active, since the last resetRealtimeViolations().
std::size_t realtimeViolationCount() noexcept;

void resetRealtimeViolations() noexcept;

/// Records a violation explicitly. Used by the operator new/delete replacements and
/// available to future instrumentation.
void recordRealtimeViolation() noexcept;

} // namespace gs::test

#endif // GS_TEST_RTGUARD_H

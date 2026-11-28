// Compiled directly into the test executable (not into the support static library) so that
// the global operator new / operator delete replacements are guaranteed to be part of the
// final link unit. A replacement living in a static library can be omitted by the linker if
// no symbol from its object file happens to be referenced.

#include "gs/test/RtGuard.h"

#include <cstdlib>
#include <new>

namespace gs::test {
namespace {

thread_local std::size_t g_guardDepth = 0;
thread_local std::size_t g_violations = 0;

} // namespace

RealtimeSectionGuard::RealtimeSectionGuard() noexcept { ++g_guardDepth; }

RealtimeSectionGuard::~RealtimeSectionGuard() noexcept {
    if (g_guardDepth > 0) {
        --g_guardDepth;
    }
}

bool realtimeSectionActive() noexcept { return g_guardDepth > 0; }

std::size_t realtimeViolationCount() noexcept { return g_violations; }

void resetRealtimeViolations() noexcept { g_violations = 0; }

void recordRealtimeViolation() noexcept { ++g_violations; }

} // namespace gs::test

namespace {

// Count, never throw and never abort: a guarded scope must still complete so the test can
// assert on the counter rather than crashing the whole run.
void noteIfGuarded() noexcept {
    if (gs::test::realtimeSectionActive()) {
        gs::test::recordRealtimeViolation();
    }
}

void* allocate(std::size_t size) {
    noteIfGuarded();
    void* memory = std::malloc(size == 0 ? 1 : size);
    if (memory == nullptr) {
        throw std::bad_alloc();
    }
    return memory;
}

void* allocateNoThrow(std::size_t size) noexcept {
    noteIfGuarded();
    return std::malloc(size == 0 ? 1 : size);
}

void* allocateAligned(std::size_t size, std::size_t alignment) noexcept {
    noteIfGuarded();
    void* memory = nullptr;
    const std::size_t minimumAlignment = sizeof(void*);
    const std::size_t effectiveAlignment = alignment < minimumAlignment ? minimumAlignment : alignment;
    if (posix_memalign(&memory, effectiveAlignment, size == 0 ? 1 : size) != 0) {
        return nullptr;
    }
    return memory;
}

void release(void* memory) noexcept {
    noteIfGuarded();
    std::free(memory);
}

} // namespace

void* operator new(std::size_t size) { return allocate(size); }

void* operator new[](std::size_t size) { return allocate(size); }

void* operator new(std::size_t size, const std::nothrow_t&) noexcept { return allocateNoThrow(size); }

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept { return allocateNoThrow(size); }

void* operator new(std::size_t size, std::align_val_t alignment) {
    void* memory = allocateAligned(size, static_cast<std::size_t>(alignment));
    if (memory == nullptr) {
        throw std::bad_alloc();
    }
    return memory;
}

void* operator new[](std::size_t size, std::align_val_t alignment) {
    void* memory = allocateAligned(size, static_cast<std::size_t>(alignment));
    if (memory == nullptr) {
        throw std::bad_alloc();
    }
    return memory;
}

void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    return allocateAligned(size, static_cast<std::size_t>(alignment));
}

void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept {
    return allocateAligned(size, static_cast<std::size_t>(alignment));
}

void operator delete(void* memory) noexcept { release(memory); }

void operator delete[](void* memory) noexcept { release(memory); }

void operator delete(void* memory, std::size_t) noexcept { release(memory); }

void operator delete[](void* memory, std::size_t) noexcept { release(memory); }

void operator delete(void* memory, const std::nothrow_t&) noexcept { release(memory); }

void operator delete[](void* memory, const std::nothrow_t&) noexcept { release(memory); }

void operator delete(void* memory, std::align_val_t) noexcept { release(memory); }

void operator delete[](void* memory, std::align_val_t) noexcept { release(memory); }

void operator delete(void* memory, std::size_t, std::align_val_t) noexcept { release(memory); }

void operator delete[](void* memory, std::size_t, std::align_val_t) noexcept { release(memory); }

void operator delete(void* memory, std::align_val_t, const std::nothrow_t&) noexcept { release(memory); }

void operator delete[](void* memory, std::align_val_t, const std::nothrow_t&) noexcept { release(memory); }

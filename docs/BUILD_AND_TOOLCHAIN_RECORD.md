# Build and Toolchain Record

**Version:** 0.1 — Milestone A0
**Status:** Record of what the A0 build actually is and what it deliberately does not decide. This document approves nothing.
**Authority:** Owner authorization comment on PR #6, 2026-11-27 — "Milestone A0 only: framework-free build & headless test skeleton".
**Branch base:** `phase3/architecture-completion-audit` @ `b0371db59ae6fc4b913142b9af503f1ae45b61d6`.

## 1. What A0 contains

| Component | Target | Notes |
|---|---|---|
| Framework-free core | `gs_core` (STATIC) | Build identity only; no DSP, no parameters, no audio |
| Internal test harness | `gs_test_support` (STATIC) | Zero third-party dependencies, per A0 authorization |
| Headless test executable | `gs_core_tests` | No host, no audio device, no UI, no framework |
| Build system | CMake ≥ 3.22 + `CMakePresets.json` | Presets: `macos-debug`, `macos-release`, `ci-debug`, `ci-release` |
| Determinism guard | `cmake/GsDeterminism.cmake` | `-ffp-contract=off`; refuses fast-math style flags at configure time |
| Core-boundary guard | `cmake/GsNoFrameworkGuard.cmake` | CTest check that `src/core` contains no framework/plug-in SDK includes |
| CI | `.github/workflows/ci.yml` | macos-13 (x86_64) + macos-14 (arm64), Debug + Release |

## 2. What A0 deliberately does NOT decide

| Open item | Status | How A0 avoids committing |
|---|---|---|
| Q-T01 framework (JUCE 8 or other) | PROPOSED (DEC-005) | No framework dependency of any kind exists in the build |
| Q-T02 core independence | PROPOSED (DEC-011) | Honoured as an engineering rule and enforced by a guard test, but not recorded as an approved decision |
| Q-T03 macOS minimum / Universal 2 | OPEN | `CMAKE_OSX_ARCHITECTURES` and `CMAKE_OSX_DEPLOYMENT_TARGET` are **left unset**. `GS_OSX_ARCHITECTURES` / `GS_OSX_DEPLOYMENT_TARGET` cache variables exist so a future decision is configurable without editing the build |
| Q-L04 JUCE EULA, Q-P03 product model | OPEN | No framework is fetched, vendored or linked |
| Q-L05 third-party licences | OPEN | Zero third-party dependencies — see `DEPENDENCY_AND_LICENCE_MANIFEST.md` |
| Q-T09 RT-safety tooling | PROPOSED | A test-only allocation guard is provided as instrumentation; no RTSan/interposer adoption is decided |
| Q-T12 parameter IDs, AR-01, AR-02, DEC-042 details | OPEN / unresolved | No parameter registry, event ordering or Freeze code exists |

**No product decision, requirement or tier was changed by A0.**

## 3. Recorded deviation: `src/` prefix

`DSP_ARCHITECTURE.md` §3 shows the module tree at repository root (`core/`, `sample/`, `engine/`, `state/`, `host/`, `ui/`). A0 places the same module names under **`src/`**, and the test tree under `tests/`. The A0 authorization explicitly approved this "proposed `src/` and `tests/` directory skeleton, documenting the deviation from the current architecture directory proposal".

Module names, their responsibilities and the dependency rule `ui → host → state/engine → sample → core` are unchanged. Only the path prefix differs. `DSP_ARCHITECTURE.md` §3 has not been edited, because that document is part of the still-open Phase 3 review; whoever closes Phase 3 should reconcile the prefix there.

Modules not created by A0 — `sample/`, `engine/`, `state/`, `host/`, `ui/` — are listed as comments in `src/CMakeLists.txt` with the milestone or gate that governs each.

## 4. Toolchain actually used for the A0 evidence

Observed on the machine that produced the results in §5. **This is not a pinned, approved toolchain** — pinning is part of the blocked A1 work, because it depends on Q-T01/Q-T03.

| Item | Value |
|---|---|
| Host | Intel **x86_64**, macOS **12.7.6** (build 21H1320) |
| Compiler | **AppleClang 14.0.0.14000029** (`/usr/bin/c++`) |
| Xcode | **Not installed** — Command Line Tools only (`/Library/Developer/CommandLineTools`) |
| macOS SDK | 13.1 (CommandLineTools SDK) |
| CMake | 4.3.2 |
| Generator | Unix Makefiles |
| Architectures | host only (`x86_64`); **not** Universal 2 |
| Deployment target | unset (host default) |

### Known toolchain gaps for later milestones

These block A1 and beyond and are recorded here so they are not rediscovered later:

1. **No full Xcode.** AU/VST3 bundle builds, `auval`, codesign and notarization all require it.
2. **AppleClang 14 is a weak C++20 baseline.** libc++ 14 lacks `std::format`, `std::atomic_ref`, `std::jthread`/`stop_token` and has incomplete `<ranges>`. `std::atomic_ref` and `jthread` are directly relevant to the threading model in `THREADING_AND_OWNERSHIP_SPECIFICATION.md`. A newer Xcode/clang is recommended before Milestone C.
3. **Intel-only host, macOS 12.** arm64 slices cannot be *executed* locally, Apple Silicon audio-workgroup APIs cannot be validated here, and newer macOS behaviour cannot be observed.
4. **CMake 4.x.** Fine for A0, but JUCE 8's own CMake code should be verified against CMake 4.x before adopting it, since CMake 4 removed compatibility with very old `cmake_minimum_required` values.

## 5. Evidence — commands run and results

All four commands below were executed on the machine described in §4. Output is summarised faithfully; nothing here is inferred.

```
cmake --preset macos-debug          -> Configuring done / Generating done, exit 0
cmake --build --preset macos-debug  -> gs_core, gs_test_support, gs_core_tests built; 0 warnings
ctest --preset macos-debug          -> 100% tests passed, 0 failed out of 6
cmake --preset macos-release        -> Configuring done / Generating done, exit 0
cmake --build --preset macos-release-> built; 0 warnings
ctest --preset macos-release        -> 100% tests passed, 0 failed out of 6
cmake --preset ci-debug             -> warnings-as-errors ENABLED
cmake --build --preset ci-debug     -> 0 warning/error lines
ctest --preset ci-debug             -> all tests passed
```

Harness detail (Debug): **12 test cases registered, 12 passed, 0 failed.** CTest aggregates them into 6 entries: `suite.all`, `suite.core`, `suite.harness`, `suite.rtGuard`, `suite.list_is_not_empty`, `guard.no_framework_in_core`.

### Guards were verified by provoking them (negative tests)

| Guard | Provocation | Result |
|---|---|---|
| Determinism | configure with `-DCMAKE_CXX_FLAGS="-ffast-math"` | configure **failed** with the intended fatal error |
| Core boundary | ran the guard against a directory containing `#include <juce_core/juce_core.h>` | guard **failed** with "FRAMEWORK DEPENDENCY DETECTED IN CORE" |

These were throwaway checks outside the source tree; no such file is committed.

## 6. Explicitly NOT validated

Stated plainly so no reader over-reads the green test output:

- **CI has not been observed.** The workflow is committed but no run has been seen by the author of this record. Treat CI status as unknown until a run is inspected on the PR.
- **No Apple Silicon validation.** The arm64 CI leg exists; its result has not been observed. Nothing was executed on arm64 locally.
- **No Universal 2 binary** was produced, and no `lipo` verification was performed — architectures are intentionally unset.
- **No macOS version support claim.** The deployment target is unset; only macOS 12.7.6 was exercised.
- **No AU/VST3/Standalone validation**, no `auval`, no `pluginval`, no host scan.
- **No realtime-safety proof.** The allocation guard is test instrumentation with documented blind spots (see `tests/support/include/gs/test/RtGuard.h`): it sees only global `operator new`/`delete` in this binary, not `malloc`, locks, blocking calls or I/O.
- **No performance data.** No benchmark was built or run; Q-T07 numeric budgets remain unmeasured and unauthorized.
- **No clang-tidy enforcement.** A configuration exists; the CI job that would run formatting checks is marked advisory because its runner toolchain has not been confirmed.

## 7. Next gated step

Milestone **A1** pins the compiler, C++ standard, framework/SDK versions, deployment target and Universal 2 configuration, then adds plug-in shells and host validation. It cannot start until **Q-T01, Q-T03, Q-L04** (and Q-P03 for licence tier) are resolved and full Xcode is available.

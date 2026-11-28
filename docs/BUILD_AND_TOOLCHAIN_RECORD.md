# Build and Toolchain Record

**Version:** 0.1 — Milestone A0
**Status:** Record of what the A0 build actually is and what it deliberately does not decide. This document approves nothing.
**Authority:** Owner authorization comment on PR #6, 2026-11-27 — "Milestone A0 only: framework-free build & headless test skeleton".
**Branch base (after synchronization):** `phase3/architecture-completion-audit` @ `f4c5285a9e51b9c855a099ce7d4ac0131a4472ea`.

## 1. What A0 contains

| Component | Target | Notes |
|---|---|---|
| Framework-free core | `gs_core` (STATIC) | Build identity only; no DSP, no parameters, no audio |
| Internal test harness | `gs_test_support` (STATIC) | Zero third-party dependencies, per A0 authorization |
| Headless test executable | `gs_core_tests` | No host, no audio device, no UI, no framework |
| Build system | CMake ≥ 3.22 + `CMakePresets.json` | Presets: `macos-debug`, `macos-release`, `ci-debug`, `ci-release` |
| Determinism guard | `cmake/GsDeterminism.cmake` | `-ffp-contract=off`; refuses fast-math style flags at configure time |
| Core-boundary guard | `cmake/GsNoFrameworkGuard.cmake` | CTest check that `src/core` contains no framework/plug-in SDK includes |
| CI | `.github/workflows/ci.yml` | macos-15-intel (x86_64) + macos-15 (arm64), Debug + Release |

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

## 6. CI evidence and explicitly NOT validated

### Observed CI evidence — PR #7, commit `7a5d3705c7790b67e3368fb2ee05ef94cf8bead8`

GitHub Actions run [#46056952540](https://github.com/marentz120/granular-synth/actions/runs/46056952540) completed on 2026-11-27. All four build/test jobs succeeded:

| Runner | Observed OS | Architecture | Compiler | CMake | Result |
|---|---|---|---|---|---|
| `macos-15-intel` Debug | macOS 15.7.9 | x86_64 | AppleClang 17.0.0 | 4.4.2 | Build passed; 6/6 CTest passed |
| `macos-15-intel` Release | macOS 15.7.9 | x86_64 | AppleClang 17.0.0 | 4.4.2 | Build passed; 6/6 CTest passed |
| `macos-15` Debug | macOS 15.8.2 | arm64 | AppleClang 17.0.0 | 4.5.1 | Build passed; 6/6 CTest passed |
| `macos-15` Release | macOS 15.8.2 | arm64 | AppleClang 17.0.0 | 4.5.1 | Build passed; 6/6 CTest passed |

The advisory `clang-format` job failed in that run: clang-format 23.1.6 reported formatting violations across tracked C++ files. Because the job has `continue-on-error: true`, the workflow conclusion was success; that must not be represented as a clean formatting check.

### Formatting remediation — resolved locally, CI confirmation pending

The violations were fixed by running the repository's committed `.clang-format` with the same clang-format version the CI job installs. Measured with the exact CI command (`files=$(git ls-files '*.h' '*.cpp'); clang-format --dry-run --Werror $files`, run under bash):

| | Violations | Exit code |
|---|---|---|
| Before | **82** across 6 files | 1 |
| After | **0** | 0 |

Per-file before: `tests/support/src/RtGuard.cpp` 42, `tests/support/include/gs/test/Harness.h` 22, `tests/support/src/Harness.cpp` 9, `src/core/src/Version.cpp` 4, `tests/unit/RtGuardTest.cpp` 3, `tests/support/src/TestMain.cpp` 2.

The change is formatting only, verified mechanically: with all whitespace stripped, five of the six files are byte-identical to their previous contents. The sixth, `tests/unit/RtGuardTest.cpp`, differs only by two swapped `#include` lines — `SortIncludes` treats `RtGuard.h` as that file's main header (LLVM's `IncludeIsMainRegex`) and promotes it; its non-include content is byte-identical ignoring whitespace. Local verification after formatting: `macos-debug` and `macos-release` both build with 0 warnings and pass 6/6 CTest entries (harness 12/12), and both `-Werror` CI presets are clean.

**Confirmed by CI.** GitHub Actions run [#46118795550](https://github.com/marentz120/granular-synth/actions/runs/46118795550) on commit `87970fe` reports every job green, including the formatting job that previously failed:

| Job | Run on `5fe2dfb` (pre-fix) | Run on `87970fe` (post-fix) |
|---|---|---|
| `clang-format (advisory)` | **failure** | **success** |
| `macos-15-intel / ci-debug` | success | success |
| `macos-15-intel / ci-release` | success | success |
| `macos-15 / ci-debug` | success | success |
| `macos-15 / ci-release` | success | success |

Toolchain note: the local clang-format 23.1.6 was built from source on macOS 12/x86_64, because no Homebrew bottle exists for that configuration. It matches the CI job's version but not its host; the CI run above is the authoritative confirmation.

### Explicitly NOT validated

Stated plainly so no reader over-reads the green build/test result:

- **No Universal 2 binary** was produced, and no `lipo` verification was performed — architectures are intentionally unset.
- **No macOS version support claim.** The deployment target is unset; CI ran on the observed macOS 15 versions, and the local A0 evidence was collected on macOS 12.7.6.
- **No AU/VST3/Standalone validation**, no `auval`, no `pluginval`, no host scan.
- **No realtime-safety proof.** The allocation guard is test instrumentation with documented blind spots (see `tests/support/include/gs/test/RtGuard.h`): it sees only global `operator new`/`delete` in this binary, not `malloc`, locks, blocking calls or I/O.
- **No performance data.** No benchmark was built or run; Q-T07 numeric budgets remain unmeasured and unauthorized.
- **No clang-tidy enforcement.** A configuration exists but clang-tidy is not run by CI.
- **The clang-format job is still advisory (`continue-on-error: true`)** and has not been observed passing on a runner since the formatting fix. Making it blocking is a reasonable follow-up once a green run exists, but that is a CI policy change and is not part of this remediation.

## 7. Next gated step

Milestone **A1** pins the compiler, C++ standard, framework/SDK versions, deployment target and Universal 2 configuration, then adds plug-in shells and host validation. It cannot start until **Q-T01, Q-T03, Q-L04** (and Q-P03 for licence tier) are resolved and full Xcode is available.

# Testing Strategy

**Status:** Initial principles.

## DSP unit tests
- Grain window/envelope output and edge cases.
- Sample interpolation and playback boundaries.
- Scheduler rate, duration and bounded-capacity behavior.
- Pitch/time mapping and parameter scaling.
- Modulation routing, smoothing and clamping.
- Voice allocation, release and stealing.

## Real-time safety
- Detect audio-thread allocations and blocking where feasible.
- Stress dense polyphony and maximum configured grain count.
- Test CPU overload and deterministic resource reduction.
- Test sample-rate and buffer-size changes.
- Run long-duration playback and state-change tests.

## Integration
- Parameter persistence and stable parameter IDs.
- Preset save/load and missing-sample behavior.
- Standalone audio/MIDI device changes.
- AU/VST3 scanning, instantiation, automation and state restoration.
- Session save/reopen in representative hosts.

## Release validation
Define supported macOS versions, hardware, hosts and measurable thresholds before release. Record build configuration, commands, results, environment and known limitations for every milestone.

---

## Milestone A0 — test infrastructure that actually exists

Added 2026-11-27 under the Milestone A0 authorization. This section records **implemented** infrastructure only. The principles above remain the Phase 5 plan, and the mandatory test families in `CLAUDE_CODE_IMPLEMENTATION_HANDOFF.md` §7 are **not** satisfied by A0.

**What exists**

| Item | Where | Notes |
|---|---|---|
| Headless test executable | `gs_core_tests` | No host, audio device, UI or framework; runnable in CI |
| Internal test harness | `tests/support` | Zero third-party dependencies (Q-L05 open). Registration, `GS_CHECK`/`GS_CHECK_MSG`/`GS_REQUIRE`, `--list`, `--filter` |
| Test-only realtime allocation guard | `tests/support/.../RtGuard.h` | `RealtimeSectionGuard` scope + violation counter |
| Core-boundary guard | `cmake/GsNoFrameworkGuard.cmake` | CTest check that `src/core` has no framework/SDK includes |
| Determinism guard | `cmake/GsDeterminism.cmake` | Refuses fast-math style flags at configure time (protects GS-NFR-005 / GS-PLT-004) |
| CTest entries | `tests/CMakeLists.txt` | `suite.all`, `suite.core`, `suite.harness`, `suite.rtGuard`, `suite.list_is_not_empty`, `guard.no_framework_in_core` |

**Current state:** 12 harness test cases, 6 CTest entries, passing in Debug and Release on the toolchain recorded in `BUILD_AND_TOOLCHAIN_RECORD.md` §4.

**Deliberate limits — do not over-read these tests**

- A0 implements no product requirement, so no `GS-*` requirement may be claimed as tested. The tests support QA-002 and QA-004 at infrastructure level only.
- The allocation guard is **not** a realtime-safety proof. It observes only global `operator new`/`delete` within the test binary: not `malloc`/`realloc`, not allocations inside prebuilt system libraries, and not locks, blocking calls, file I/O or unbounded work. Q-T09 (interposer and/or Clang RTSan) is still an open decision.
- No golden-render harness, no deterministic block renderer, no seeded-RNG equivalence testing — those belong to Milestone B (IF-12) and need AR-01/Q-T12 first.
- No benchmark, no timing or memory measurement. Numeric thresholds remain unmeasured and unauthorized (Q-T07).

**Requirement-linked test naming**, as required from Milestone B onwards: once real requirements are implemented, test names and comments must cite the `GS-*` / `AC-*` / `IF-*` IDs they exercise. A0 establishes the convention; it has no requirements to cite yet.

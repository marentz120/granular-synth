# Dependency and Licence Manifest

**Version:** 0.1 — Milestone A0
**Status:** Living record. Every dependency that ever enters the build must be listed here with its licence before it is used.
**Authority:** Milestone A0 authorization (PR #6 comment, 2026-11-27) — "No third-party dependency introduction unless separately approved". Q-L05 (third-party licences) is OPEN.

## 1. Current dependencies

**Shipping/runtime dependencies: none.**
**Build dependencies: none beyond the platform toolchain.**
**Test dependencies: none — the test harness is internal, written for this repository.**

| Dependency | Version | Licence | Used by | Ships in product? | Approval |
|---|---|---|---|---|---|
| — | — | — | — | — | — |

The only external requirements are the platform toolchain itself (a C++20 compiler, CMake and CTest), which is not a vendored dependency.

### Why the test framework is internal

Q-L05 is open, so no external test framework (Catch2, GoogleTest, doctest) may be introduced. The A0 authorization requires an "internal, zero-third-party-dependency test harness with CTest", and that is what `tests/support` provides. If the owner later approves an external framework, it must be added to the table above with its licence before use.

## 2. Candidate dependencies — NOT approved, NOT present

Recorded only so the eventual licence review has a complete picture. Each is blocked behind Q-L05 and, where relevant, Q-T01/Q-L04.

| Candidate | Purpose | Licence position (unverified) | Gate |
|---|---|---|---|
| JUCE 8 | Standalone/AU/VST3 shells, UI | Dual AGPLv3 / commercial; tier depends on revenue and legal entity | Q-T01, Q-L04, Q-P03 |
| Steinberg VST3 SDK | VST3 format | Dual GPLv3 / proprietary; MIT for parts of 3.8.x per vendor docs | Q-T01, Q-L04 |
| Apple AudioToolbox / Accelerate | Decoding, FFT, vector math | Apple platform SDK | Q-T16; no separate licence file |
| libsamplerate / r8brain | Offline sample-rate conversion | Permissive (to verify) | Q-T04, Q-L05 |
| dr_libs / libsndfile | Portable audio decoding fallback | dr_libs public-domain/MIT; libsndfile LGPL — LGPL needs legal review | Q-T16, Q-L05 |
| PFFFT / KissFFT | Portable FFT fallback | Permissive (to verify) | Q-L05, Spectral L3 |
| Catch2 / GoogleTest | Test framework | BSL-1.0 / BSD-3 | Q-L05 |

**FFTW is explicitly excluded** from consideration for a closed-source product (GPL), consistent with `DSP_ARCHITECTURE.md`.

## 3. Rules

1. No dependency is added without an entry in §1 **and** owner approval.
2. GPL/AGPL/LGPL code must not enter the shipped binary without explicit legal review (Q-L04/Q-L05).
3. Test-only dependencies still require an entry, even though they do not ship.
4. Each entry must pin an exact version or tag; "latest" is not acceptable for a reproducible build.
5. The `src/core` tree must contain no framework or plug-in SDK include at all — enforced mechanically by the `guard.no_framework_in_core` CTest check.

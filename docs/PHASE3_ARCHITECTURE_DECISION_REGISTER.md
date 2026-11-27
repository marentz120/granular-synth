# Phase 3 Architecture Decision Register

**Version:** 0.1 — Review proposal  
**Status:** Recommendations for owner review. None of the proposed technical decisions below is approved by this document.  
**Authority:** `OPEN_QUESTIONS.md` v0.6, `DSP_ARCHITECTURE.md` v0.5 candidate, and the Phase 3 supporting specifications.

## 1. Recommended decisions

| ID | Topic | Recommendation | Evidence / validation needed | Proposed disposition |
|---|---|---|---|---|
| Q-T01 | Framework | C++20 + JUCE 8 for Standalone/AU/VST3 shells | Confirm licence tier and legal entity; build minimal shell; compare required host workflows | Approve conditionally after Q-L04 and host prototype |
| Q-T02 | Core independence | Framework-free DSP, engine and state domain APIs | Headless build and unit-test target; no JUCE types in core headers | Approve |
| Q-T03 | macOS minimum / architectures | macOS 12+; Universal 2 (arm64 + x86_64) | Verify SDK/toolchain and target-host support; test clean OS installations | Approve conditionally after validation |
| Q-T04 | Sample-rate handling | Keep source-rate audio; compensate playback increments by sourceRate/hostRate | Measure aliasing/interpolation across source/host rates, reverse and extreme pitch | Approve after DSP tests |
| Q-T05 | Long samples | R1 uses resident RAM buffers; defer disk streaming for long Sampler assets to L2 | Measure six-minute-per-layer memory and concurrent replacement/recording overhead | Approve R1 policy; budget remains open |
| Q-T06 | Grain windows | Versioned window table bank; each grain retains a stable table/version snapshot | Window shape/sum tests, modulation transitions and retirement lifetime tests | Approve after test design |
| Q-T07 | Global grain budget | Deterministic fixed global budget for R1; no adaptive CPU-feedback scheduling initially | Authorized benchmark spike on the reference machine; stress patch and overload behavior | Defer numeric value until measured |
| Q-T08 | Multithreaded rendering | Single audio render thread for initial R1; preserve a future worker partition seam | Profile worst-case four-layer load; only add workers if measured necessary | Defer worker activation until post-R1 profiling |
| Q-T09 | RT-safety tooling | Allocation/lock/blocking guards in debug/CI; evaluate Clang RTSan where supported | Verify supported toolchain and test coverage | Approve tooling direction |
| Q-T10 | Spectral latency reporting | Defer to Spectral L3 architecture; R1 Granular/Sampler add zero latency | Verify R1 path and host latency behavior | Defer Q-T10 to L3 |
| Q-T11 | Phase-vocoder window/overlap | Defer final choice to Spectral L3; retain Hann 4× only as research baseline | Spectral-specific quality/latency/CPU tests | Defer Q-T11 to L3 |
| Q-T12 | Parameter IDs/host exposure | Stable canonical string IDs plus explicit append-only numeric host-ID manifest; expose all explicitly automatable R1 parameters; exclude structural actions | Review `PARAMETER_ID_SPECIFICATION.md`; generate full inventory and validate AU/VST3 mapping | Owner approval required before any public build |
| Q-T13 | Modulation control rate | 16-sample control sub-block for control-rate sources; interpolate audio-rate destinations | Fast modulation, aliasing/zipper and CPU tests | Approve after tests |
| Q-T14 | Granular interpolation | Cubic Hermite default; retain Linear selectable; Sampler has its required polyphase/windowed-sinc option | Frequency response, aliasing and full-polyphony CPU comparison | Approve after tests |
| Q-T15 | Preset/state format | Versioned JSON manifest plus binary audio chunks; external references for library samples; embedded unsaved recordings | Host state prototype, save interruption, size/corruption and migration tests | Approve container after host validation |
| Q-T16 | Audio decoding | AudioToolbox primary for R1 WAV/AIFF/FLAC; add a portable decoder only if required variants fail validation | Test exact required bit depths, float PCM, malformed files and Intel/Apple Silicon parity | Approve conditionally after format tests |
| Q-T17 | Rates/buffers/latency | Retain proposed 44.1–192 kHz, 16–4096 frames and variable blocks as target matrix; zero added R1 latency | Validate on reference machine and every target host; revise only with evidence | Approve after host/engine validation |

## 2. Related architecture decisions

| Topic | Recommendation | Gate |
|---|---|---|
| VST3 live input | Instrument with optional stereo input bus; master stereo output plus four optional stereo layer outputs | Host prototype and Q-P09 matrix |
| AU live input | Start with AU instrument `aumu`; test `aufm` music-effect alternative. Do not commit to dual components without demonstrated host need | Logic and additional host prototype |
| Shared engine | One framework-independent engine/state implementation for Standalone, AU and VST3 | Q-T02 |
| RT workers | Do not enable in R1 absent measured need | Q-T08 |
| Sample data | Immutable decoded float32 buffers; off-thread decode/analysis and safe swap | Q-T04/Q-T05/Q-T16 |
| Plugin state | Embed unsaved recordings; external library references with relink/collect | DEC-025; host state prototype |
| Freeze | Source Snapshot Freeze per `FREEZE_BEHAVIOR_SPECIFICATION.md` v1.0 | **Approved — DEC-042**; engineering limits remain open |
| R1 DSP latency | Granular and Sampler add no algorithmic latency | GS-NFR-004 validation |
| Resource ceilings | Measured safe limits, deterministic overload and user-visible failure; no guessed numeric caps | Q-T07 and Phase 5 |

## 3. Explicit deferrals

- Spectral phase-vocoder implementation and latency policy (Q-T10/Q-T11): L3 engine gate.
- Multithreaded render workers (Q-T08): only after profiling demonstrates a need.
- Long-file Sampler disk streaming (Q-T05): later tier unless R1 feasibility requires a new scope decision.
- CLAP: optional, not R1 (DEC-026).
- Factory content, product naming and commercial distribution: later release gates.

A deferral must be recorded with the target phase and rationale. It does not mean the item is silently considered solved.

## 4. Required owner approvals

The following are architecture/product decisions that should not be silently committed:
1. Framework and core boundary (Q-T01/Q-T02), subject to licence and prototype evidence.
2. Minimum macOS and Universal 2 support target (Q-T03).
3. Parameter ID and host-exposure contract (Q-T12), irreversible once public automation is shipped.
4. Freeze semantics are approved as Source Snapshot (DEC-042); numeric memory limits, capture duration and crossfade remain engineering gates.
5. AU component strategy and host-visible bus topology (Q-P06), subject to host validation.

Other numeric values remain conditional on measurement and must not be approved by document wording alone.

**Status:** This register consolidates proposed technical decisions for a single owner review after evidence and prototypes are complete.

# Phase 3 Architecture Decision Register

**Version:** 0.4 — state callback and snapshot freshness evidence gates refined  
**Status:** Recommendations for owner review. None of the proposed technical decisions below is approved by this document.  
**Authority:** `OPEN_QUESTIONS.md` v0.8, `DSP_ARCHITECTURE.md` v0.6 candidate, and the Phase 3 supporting specifications.

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
| Q-T15 | Preset/state container and migration | Versioned JSON manifest plus binary audio chunks; external library references; AU/VST3 adapters transport the same canonical state through host-specific APIs. DAW document state embeds unsaved recordings and Freeze snapshots. | Validate AU dictionary/Data representation and nil failure semantics; VST3 IBStream short-transfer and `tresult` propagation; realtime/offline callback context; coherent multi-source capture-boundary publication; stale-snapshot rejection; retention leases; concurrent save/restore admission; cancellation; interrupted save; size/corruption; schema migration and peak memory. User-preset payload scope remains open. | Approve the container only after host validation; exact framing/codec/limits remain unapproved |
| Q-T16 | Audio decoding | AudioToolbox primary for R1 WAV/AIFF/FLAC; add a portable decoder only if required variants fail validation | Test exact required bit depths, float PCM, malformed files and Intel/Apple Silicon parity | Approve conditionally after format tests |
| Q-T17 | Rates/buffers/latency | Retain proposed 44.1–192 kHz, 16–4096 frames and variable blocks as target matrix; zero added R1 latency | Validate on reference machine and every target host; revise only with evidence | Approve after host/engine validation |

## 2. Related architecture decisions

| Topic | Recommendation | Gate |
|---|---|---|
| VST3 live input | Instrument with optional stereo input bus; master stereo output plus four optional stereo layer outputs | Host prototype and Q-P09 matrix |
| VST3 instrument identity and event routing | One instrument-classified component; candidate `Instrument|Synth|Sampler`; one 16-channel event bus with internal layer fan-out; full-band audio input bus type remains open | Verify category discovery, raw audio source routing, MPE Note Expression, five outputs, bus activation and save/reopen in each Q-P09 VST3 host; see `PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md` |
| AU component strategy | Retain `aumu` as first validation candidate; test full-band input on the instrument component. If it fails DEC-020, test `aufm` against the complete MIDI/MPE/instrument workflow; dual only for distinct proven workflows | Logic plus at least one additional target AU host; full-band source path, Note On/Off, MPE, five outputs, automation and state/reopen evidence; see `PHASE3_AU_COMPONENT_STRATEGY_AUDIT.md` |
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
6. VST3 instrument category, live-input bus type, simple-mode support and host-visible Unit/routing metadata, subject to host validation; see `PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md`.

Other numeric values remain conditional on measurement and must not be approved by document wording alone.

**Status:** This register consolidates proposed technical decisions for a single owner review after evidence and prototypes are complete.


## 5. Additional cross-cutting architecture questions (open)

These questions were exposed by the Phase 3 event-time and overload audit. They are not new product-scope decisions and must be resolved before the relevant implementation contracts are frozen.

| ID | Question | Candidate direction | Evidence required | Status |
|---|---|---|---|---|
| AR-01 | What deterministic tie-break and writer precedence apply when host automation, MIDI mapping, sequencer parameter locks and UI writes target the same parameter at the same sample offset? | Preserve source-local order; use an explicit stable cross-source ordering key; define precedence by parameter/update class rather than producer arrival time. Keep base parameter writes separate from modulation contributions under DEC-031. | Written per-parameter writer matrix; exact-offset tests for simultaneous sources; host automation/MIDI/sequence-lock tests across variable block sizes. | OPEN — no precedence selected |
| AR-02 | What is the exact deterministic reclamation policy when voice or global grain capacity is exhausted? | Keep voice stealing and grain-budget reclamation separate. Candidate ordering: released/non-sustained/oldest voices for GS-VOI-001; release-voice grains then oldest spawn sequence with stable layer/voice/grain tie-break for GS-ARC-004. | Resolve eligibility/fallback when all voices are held; define spawn rejection accounting, fade envelope and thresholds; seeded overload tests and listening validation. Numeric budget requires separately authorized benchmark (Q-T07). | OPEN — candidate only |

The current DSP proposal's former `threshold·k` expression is not a usable formula (`k` is undefined); “oldest” and “longest-remaining” are distinct policies. The updated DSP specification marks this ambiguity and records the candidate comparator without approving it. Neither AR-01 nor AR-02 authorizes implementation or benchmark work.


## 6. MPE identity and lifecycle questions (open)

| ID | Question | Candidate direction | Evidence required | Status |
|---|---|---|---|---|
| AR-03 | AU MPE same-pitch Note Off matching, shared member-channel behavior, sustain/release tails and zone reconfiguration | Separate source-note identity from member-channel expression scope; report ambiguous matches; snapshot/detach released voices before channel reuse | MPE 1.1 protocol review; same-pitch retrigger, shared-channel, sustain and channel-reuse fixtures; owner review of user-visible ambiguity behavior | OPEN — no FIFO/LIFO policy selected |
| AR-04 | VST3 Note Expression mapping for product Pressure and Timbre, including host delivery/recording behavior | Tuning is the standard pitch candidate; Pressure/Timbre use explicitly declared standard or custom types; do not silently alias semantics | SDK type review; Note Expression discovery and event tests in each Q-P09 host | OPEN — type mapping not selected |
| AR-05 | Public AU/VST3 metadata and exact conversion for DEC-038's 48-semitone per-note range | Preserve the existing project interpretation; convert through explicit signed semitone units and host-specific normalized domains | Confirm DEC-038 wording against MPE 1.1 default and test minimum, center and maximum in target hosts | OPEN — metadata/conversion not frozen |

These questions refine GS-MID-002 and DEC-038 without changing product scope. See PHASE3_MPE_NOTE_IDENTITY_AND_LIFECYCLE_AUDIT.md. No implementation or benchmark is authorized by these proposals.

## 7. State-restore voice lifecycle question

| ID | Question | Candidate direction | Evidence required | Status |
|---|---|---|---|---|
| AR-06 | When state restore commits while notes/voices are active, terminate old voices at commit or let them finish as non-addressable tails under the old immutable runtime generation? | Never persist or rebind old host-note identities; make audible old-voice treatment an explicit owner choice | Owner decision, then fixtures for held notes, release tails, reused VST3 noteId/member channel and controller synchronization | OPEN — owner decision required |

AR-06 changes audible behavior and cannot be inferred from DEC-025. No behavior is selected by this register.


## 8. State callback and freshness evidence gate

| Gate | Candidate requirement | Evidence needed | Status |
|---|---|---|---|
| AU callback context | Do not assume a UI/non-RT caller or render suspension | Callback thread and render-overlap trace in each target AU host | OPEN — Q-P09 |
| AU failure signal | Never return stale state as successful; do not assume nil aborts save | Host-visible handling of nil/failed document-state getter | OPEN — Q-T15/Q-P09 |
| VST3 concurrency | State calls may occur while Processing; process-owned state remains immutable to callback readers | Concurrent get/set-state and process trace; offline context included | OPEN — Q-T15/Q-P09 |
| VST3 failure propagation | A non-success result/short stream must not produce a successful truncated project | Host-visible save/restore result and rollback behavior | OPEN — Q-T15/Q-P09 |
| Capture-boundary coherence | One state epoch and one coherent watermark vector across all required sources | Multi-source staggered-commit fixture; torn-vector rejection | OPEN — Q-T15 |
| Freshness | A descriptor must match the capture token; stale descriptors cannot satisfy DEC-025 | Commit-before/after-boundary and stale-descriptor fixtures | OPEN — Q-T15 |

This section adds evidence requirements only. It does not select an AU failure policy, callback primitive, capture ABI, callback deadline or owner-visible product behavior.

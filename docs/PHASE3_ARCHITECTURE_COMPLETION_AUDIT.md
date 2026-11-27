# Phase 3 Architecture Completion Audit

**Version:** 0.3  
**Status:** Investigation and gap audit only. No architecture decision, product-scope change, benchmark code, DSP implementation or release authorization is approved by this document.  
**Baseline:** `main` after Phase 2 merge commit `8229c15d457c05eb85f2d93093ad9ee418b9842a`.  
**Reviewed:** `DEVELOPMENT_ROADMAP.md` v0.2, `DSP_ARCHITECTURE.md` v0.4, `OPEN_QUESTIONS.md` v0.6, `R1_PRODUCT_SPECIFICATION.md` v1.1, `GRANULAR_SYNTH_REQUIREMENTS.md` v1.0, and the current Sampler, MIDI and Audio/I/O domain specifications.

---

## 1. Executive assessment

Phase 2 establishes a coherent R1 product baseline: four layers; Granular and Sampler engines; Standalone, AU and VST3; central sequencer; live input, recording and Freeze controls; generic MIDI-controller operation; and approved R1 release tiers.

The repository already contains a substantial **candidate** architecture in `DSP_ARCHITECTURE.md`, but it is not yet an approved, implementation-ready architecture. The document itself labels the framework and most technical choices as proposals. Several important interfaces are underspecified, some Phase 2 decisions have not been propagated into the architecture document, and several exit criteria require measured evidence that does not yet exist.

**Conclusion:** Phase 3 should proceed as an architecture decision and specification phase, not as implementation. The existing architecture is a useful starting proposal, not an approved design.

### Phase 3 exit blockers

1. Framework, supported macOS baseline and binary architecture are not approved (Q-T01…Q-T03).
2. Audio/plugin component and bus design is not host-validated, especially AU live input (Q-P06 / DEC-020 / Q-P09).
3. Stable parameter IDs, host exposure, parameter grouping and migration rules are not frozen (Q-T12).
4. Sample and recording-buffer lifecycle, including embedded plugin-state serialization, is incomplete (Q-T04…Q-T05, Q-T15…Q-T16, DEC-025).
5. Freeze product behavior is approved as Source Snapshot (DEC-042); the rolling-buffer retention lease and single global transaction are now specified as candidate contracts, while exact mechanisms, spare capacity, memory budget, crossfade and RT proof remain engineering work.
6. Real-time ownership, object retirement, command-queue capacity and overload behavior need a complete bounded model.
7. CPU, memory, latency and grain-budget values are not measured. The current memory figures are explicitly raw-payload arithmetic, not measured RSS or an approved budget.
8. The R1 architecture has not been traced to all 124 R1 requirements (100% module/interface coverage is a roadmap exit criterion).

---

## 2. Baseline that architecture must preserve

The following are approved product constraints, not open architecture choices:

- Exactly four independent layers, each selecting Granular or Sampler (DEC-017/018).
- R1 targets macOS Standalone, AU and VST3; CLAP is optional, not R1 (DEC-026).
- One central sequencer and one shared transport domain for all four layers (DEC-019 and product specification).
- Live input per layer in Standalone and plugin contexts; per-layer and global Freeze; per-layer recording and stereo master-mix recording (DEC-020).
- Essential Standalone live operations, including sequencer editing, must be accessible from a generic MIDI controller (DEC-016/021).
- Unsaved recording buffers are embedded in plugin state; existing library samples remain external references with relink/collect support (DEC-025).
- Granular target: at least six minutes of stereo audio per layer at 48 kHz, subject to safe measured feasibility (DEC-028).
- R1 grain-size range is 0.1–5000 ms (DEC-035); ten macros per patch (DEC-036); MPE per-note pitch bend is ±48 semitones range (DEC-038).
- No arbitrary product-level cap on pattern-library size, chain length or performance-set entries; technical limits must be explicit, safe and measured.
- R1 effects: Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator and Compressor (DEC-023).

Architecture must not silently change these decisions. In particular, technical resource ceilings must not be confused with product-level library/pattern limits.

---

## 3. Existing architecture assessment

### 3.1 Strong foundations to retain as candidates

- A framework-independent DSP/core library and a shared engine across Standalone/AU/VST3.
- No allocation, deallocation, blocking locks, file I/O or logging on the audio thread.
- Off-thread sample decoding and structural object construction.
- Bounded command handoff to the audio thread and deferred destruction away from RT.
- Immutable sample data and explicit sample-handle lifetime.
- A headless deterministic renderer and testable DSP modules.
- Parameter smoothing classes and separation of UI snapshots from live DSP objects.
- One central transport/sequencer across four layers.

These are sound architectural directions, but the concrete algorithms, queue bounds, ownership rules and host APIs still need specification and validation.

### 3.2 Required corrections and reconciliation

| Location | Finding | Required action |
|---|---|---|
| `DSP_ARCHITECTURE.md` §12 | State says “sample references + optional embedded audio (Q-P08)”. | Replace with DEC-025 policy: unsaved recording buffers are embedded in plugin state; library samples are references with relink/collect. Define serialization mechanics separately. |
| `DSP_ARCHITECTURE.md` §15, risk R-03 | Says “embed opt-in” and recommends reference + collect by default. | Rewrite risk around mandatory unsaved-recording embedding, large-state behavior, host save/restore constraints, interruption and recovery. |
| `DSP_ARCHITECTURE.md` §16, Q-P08 | Repeats the superseded “Reference + collect; embed opt-in” recommendation. | Mark product policy resolved by DEC-025 and specify the remaining engineering questions only. |
| `DSP_ARCHITECTURE.md` §3 | Candidate tree lists Slice, Tape and Spectral engine modules without tier qualifiers. | Label Granular and Sampler as R1; Slice/Tape as later-tier extension points and Spectral as L3. Do not imply R1 implementation scope. |
| `DSP_ARCHITECTURE.md` §9.3 | The prior revision said 10 macros, “scalable to 20”. | **Reconciled in this branch:** exactly ten macros in R1 under DEC-036; no R1 scalability implication. |
| `DSP_ARCHITECTURE.md` §12 | Suggests MIDI out “where format/host support it” and standalone MIDI clock in/out. | Reconcile with R1: MIDI clock input is required in Standalone; MIDI clock/feedback/sequencer MIDI output are not R1 commitments. |
| `DSP_ARCHITECTURE.md` §5.6 | Proposes AU `aumu` + `aumf` variants and VST3 input bus. | Treat as options only. Validate real host insertion/routing/automation behavior before selecting the component set. |
| `DSP_ARCHITECTURE.md` §5.3 | Memory table is raw sample payload arithmetic only. | Keep explicit estimate label; do not promote these figures to a memory budget. Add measured RSS and non-audio overhead later. |

### 3.3 Requirements needing explicit architectural interpretation

- `GS-ARC-002` specifies 1–20 voices per layer and `GS-ARC-003` specifies up to 128 grains per voice, while the product specification rejects arbitrary product-level caps. Document these as explicit R1 engine/voice behavior constraints (where applicable), distinct from the measured global grain budget and safe system limits.
- Four layers at six minutes stereo float32/48 kHz represent approximately 553 MB of raw audio payload if all four buffers are resident. This excludes live-recording buffers, analysis products, copies, FX/voice state, allocator overhead and UI data. The system needs a transparent admission/eviction/replacement policy and safe behavior when memory is insufficient.
- The current design says “identical” Standalone/plugin render and also allows host-specific I/O and transport. Define the deterministic equivalence boundary precisely: same engine state, input samples, MIDI/events, sample rate and render mode; document intentional host/device differences.
- Define whether all four stereo layer outputs are mandatory or optional per host, and what happens when a host cannot expose the requested bus topology.
- Define the exact master and per-layer recording tap points, including pre/post FX and mute/solo behavior, without changing the approved product scope.

---

## 4. Technical decision register — Q-T01…Q-T17

The following is an audit recommendation, not an approval. “Proposed” means the current candidate should be evaluated; “defer” means a later gate is more appropriate. All final decisions must be recorded in `DECISIONS_LOG.md`.

| ID | Topic | Current repository state | Audit recommendation / required evidence |
|---|---|---|---|
| Q-T01 | Language/framework | C++20 + JUCE 8 proposed | Compare a minimal AU/VST3/Standalone build, deployment targets, developer workflow and full JUCE commercial/Starter obligations. Select only after licence review Q-L04. |
| Q-T02 | Core independence | Framework-free core proposed | Retain as the default architecture: no JUCE types in DSP/state domain APIs. Verify with a headless build boundary. |
| Q-T03 | macOS minimum / architectures | macOS 12+ Universal 2 proposed | Verify current JUCE/SDK/host support and required Intel coverage. Record minimum OS, arm64/x86_64 build and test matrix explicitly. |
| Q-T04 | Sample-rate handling | Source-rate playback with compensated increments proposed | Validate interpolation and aliasing across 44.1–192 kHz, source-rate mismatch, reverse playback and extreme pitch. Do not decide from memory estimates alone. |
| Q-T05 | Long-sample strategy | RAM for granular; streaming for 60-minute Sampler L2 proposed | For R1, define resident-sample policy, allocation failure, simultaneous sample replacement and peak memory. Streaming remains a later-tier option unless R1 evidence requires it. |
| Q-T06 | Grain windows | Versioned table bank + per-grain snapshot proposed | Specify table generation, precision, interpolation, version lifetime and click-free window changes; verify against window-sum and discontinuity tests. |
| Q-T07 | Global grain budget | Measurement required; value open | Define a deterministic global scheduler/admission policy first. Numeric budget requires an authorized benchmark spike on the reference machine; no number may be guessed. |
| Q-T08 | Multithreaded rendering | Architect for it; enable after R1 profiling proposed | Keep R1 single audio-thread rendering initially unless measurements prove infeasible. Define partition boundaries so later workgroup rendering does not alter DSP semantics. |
| Q-T09 | RT-safety tooling | Allocation hook + RTSan proposed | Verify toolchain availability and CI stability; use allocation/lock/I/O guards as minimum evidence even if RTSan cannot run in all CI environments. |
| Q-T10 | Spectral latency reporting | Open | Spectral is L3. Defer the product-facing latency policy to the Spectral architecture gate; R1 Granular/Sampler must remain zero-added-latency unless a verified dependency says otherwise. |
| Q-T11 | Phase-vocoder window/overlap | Hann 4× baseline proposed | Spectral is L3; defer final algorithm decision to its own design gate. Preserve the current proposal as non-binding research only. |
| Q-T12 | Parameter IDs/host exposure | Stable string IDs, expose all proposed; irreversible after release | Freeze before implementation. Define canonical string IDs, numeric host-ID mapping, reserved ranges, grouping, units/curves/defaults, automatable vs structural state, duplicate handling and migration guarantees. Generate and test a complete R1 registry inventory. |
| Q-T13 | Modulation control rate | 16–32 samples in architecture; 16 proposed in Q list | Choose one R1 control-rate policy or a documented hybrid. Validate audio-rate destinations, fast modulation, aliasing/zipper behavior and CPU cost. |
| Q-T14 | Granular interpolation | Cubic Hermite default proposed | Compare linear, Hermite and windowed-sinc under pitch/rate stress, CPU and aliasing metrics. DEC-027 favors quality but does not itself select an algorithm. |
| Q-T15 | Preset/state format | JSON + external/embedded WAV proposed | Specify versioned manifest, binary audio chunk representation, checksums, atomic save, crash recovery, migration and host-state size behavior. Must implement DEC-025 exactly. |
| Q-T16 | Audio decoding | AudioToolbox primary; portable fallback proposed | Verify required WAV/AIFF/FLAC variants, malformed files, metadata, channel layouts and licence obligations. Avoid unnecessary decoder duplication. |
| Q-T17 | Sample rates / buffers / latency | 44.1–192 kHz; 16–4096 frames; variable blocks proposed | Verify actual supported host callback patterns and all supported sample rates. Specify minimum/maximum, variable block behavior, zero-added latency and failure handling; do not claim unsupported combinations. |

### 4.1 Related questions outside Q-T01…Q-T17

- **Q-P06 / DEC-020:** AU/VST3 live input. Must settle the exact plugin component(s), input bus arrangement, MIDI behavior, host routing and whether AU needs separate instrument/music-effect components.
- **Q-P09:** target host matrix. The current proposal names Logic Pro, Ableton Live, Bitwig and Reaper; establish exact versions and which host/format combinations are release-blocking.
- **Q-P10:** reference machine. The proposed base M1 target needs an actual machine/OS/host specification and reproducible profiling setup.
- **Q-P08 / DEC-025:** product policy is resolved; serialization size, host state callbacks, chunking, cancellation, missing-file and restore behavior are not.
- **Q-L04/Q-L05:** JUCE and third-party licence reviews are prerequisites to framework/dependency commitment.
- **Q-P03:** commercial/distribution model remains a later Phase 6/8 gate, but framework licence cost must be considered now.

---

## 5. Architecture domains that need complete specifications

The current `DSP_ARCHITECTURE.md` has useful outlines, but the following design contracts are missing or too implicit for implementation.

### 5.1 Host and application boundary
- Standalone audio/MIDI device lifecycle and device loss/reconnect.
- AU component type(s), VST3 class/component model, bus names/configurations, channel layouts and optional/required bus negotiation.
- MIDI event delivery, MPE, Program Change and host transport capabilities per format.
- Host parameter identity, automation gestures, parameter flush, state save/restore and plugin lifecycle callbacks.
- Ableton Link integration boundary and arbitration between host transport, internal clock and external MIDI Clock.

### 5.2 Parameter and state model
- Canonical parameter schema and stable ID-generation rules.
- Mapping from string IDs to AU/VST3 numeric IDs, including collision prevention and reserved IDs.
- Parameter metadata: range, units, normalized mapping, taper, default, smoothing class, automatable/modulatable/host-visible flags and version.
- Structural vs continuous changes; safe-point semantics; host automation and modulation precedence.
- Patch, layer, multi, pattern, performance set, controller mapping and global settings ownership.
- Versioned serialization, schema migrations, sample references, embedded unsaved recordings and atomic save/restore.

### 5.3 Real-time execution and ownership
- Exact thread inventory, ownership and permitted operations.
- Queue topology, SPSC producer/consumer assumptions, capacities, overflow policy and back-pressure.
- Immutable runtime snapshots and safe reclamation (including audio-thread last-reference prevention).
- Bounded work per callback, worst-case event count, sequencer scheduling and deterministic overload policy.
- Parameter and structural-command ordering at block boundaries.
- Device/host stop, reset, sample-rate change and buffer-size change behavior.

### 5.4 Audio/DSP signal flow
- Normative signal-flow diagram for voice, layer, FX, layer outputs, dry live input and master.
- Exact per-layer and master recording tap points.
- Engineering definition of the approved Freeze capture boundary, recording tap, live-input source handoff, snapshot publication/retirement and failure reporting; product interactions are fixed by DEC-042.
- Channel mapping for mono/stereo live input and multi-channel device/host buses.
- Sampler and granular interpolation, loop boundary and sample-swap behavior.

### 5.5 Resource management
- Memory categories and accounting: resident sample payloads, recording buffers, analysis data, grain/voice pools, FX delay lines, state serialization staging and UI waveforms.
- Safe memory admission/rejection and replacement behavior.
- Global grain budget policy, voice/grain overload policy and sequencer event capacity.
- CPU measurement methodology, reference patch set and quality degradation ordering.
- Latency accounting and host reporting.

---

## 6. Freeze product behavior is approved; engineering details remain

The owner approved Source Snapshot Freeze as DEC-042 on 2026-11-27. Product semantics are not open and must not be reopened as an architecture choice. The implementation architecture and validation plan must preserve the approved contract in `FREEZE_BEHAVIOR_SPECIFICATION.md` v1.0 and acceptance criteria AC-AUD-005, AC-AUD-015…026:

1. Capture the current valid source region, respecting crop/loop bounds and excluding uncommitted frames; prepare the immutable snapshot off the audio thread.
2. Existing grains/voices continue; new MIDI notes and sequencer events use the snapshot.
3. Transport, envelopes, LFOs, modulation, MIDI, sequencer and layer FX/tails continue.
4. Independent recording continues at its defined tap; it does not mutate the frozen snapshot.
5. Thaw crossfades to the previous live/rolling source.
6. Global Freeze transitions all four layers at one logical boundary; partial failure is visible.
7. Immutable library samples report “already static” or disable redundant capture.
8. Missing/disconnected source or memory exhaustion fails safely, preserving the prior valid source and reporting status.
9. Freeze state and snapshot persist under DEC-025; All Sound Off does not thaw.

Still open as engineering work: owner review and host/UX validation of the proposed recording taps; selection and proof of the rolling-source retention mechanism and spare capacity; snapshot memory accounting/admission; bounded transition work; crossfade duration; object retirement; and host-state serialization limits. The key concurrency invariant is now explicit: committed frames may be overwritten by a rolling writer, so an off-thread Freeze copy must hold a bounded retention lease or fail before switching. Global Freeze is specified as one transaction payload so successful layers cannot land on different render boundaries. The proposed successful-subset behavior for mixed preparation results is not explicitly settled by DEC-042; owner review must choose it or all-or-nothing semantics before implementation. These details do not reopen the approved product behavior.

---

## 7. Required Phase 3 deliverables

| Deliverable | Required content | Exit evidence |
|---|---|---|
| `DSP_ARCHITECTURE.md` v1.0 | Approved module boundaries, signal flow, RT model, resource policies, host integration, sample/state lifecycle and explicit deferrals | No contradictory Phase 2 decisions; all architecture decisions cross-referenced |
| `PARAMETER_ID_SPECIFICATION.md` v1.0 | Stable IDs, host numeric mapping, metadata schema, grouping, automation semantics and migration policy | Full R1 parameter inventory generated; uniqueness and stability tests specified |
| `THREADING_AND_OWNERSHIP_SPECIFICATION.md` v1.0 | Threads, queues, capacities, ownership, reclamation, RT prohibitions, rolling-source retention leases and global Freeze transaction behavior | Every cross-thread object has a defined owner and bounded transfer/reclamation path; rolling-source reads cannot race ring overwrite |
| `SAMPLE_AND_STATE_LIFECYCLE_SPECIFICATION.md` v1.0 | Loading, recording, sample handles, embedded unsaved buffers, external references, save/restore and recovery | DEC-025 fully reflected; failure cases and size handling specified |
| `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md` v1.0 | Standalone/AU/VST3 components, input/output buses, MIDI, host capabilities and fallback behavior | Host/format matrix and unresolved compatibility evidence explicit |
| `FREEZE_BEHAVIOR_SPECIFICATION.md` v1.0 | Capture boundary, freeze/thaw state machine, interactions and failure behavior | Product behavior approved and mapped to GS-SMG-021 acceptance |
| `PHASE3_ARCHITECTURE_TRACEABILITY.md` + `PHASE3_INTERFACE_CONTRACTS.md` | Every R1 requirement → module/interface → validation obligation; cross-domain contracts define ownership, timing, failure and validation | 124/124 domain mapping plus IF-01…IF-12 contract set; remaining per-requirement class/API traceability explicitly closed or deferred |
| Updated `OPEN_QUESTIONS.md` and `DECISIONS_LOG.md` | Final status of Q-T01…Q-T17 and related decisions | Each question Approved, Rejected or explicitly deferred with a phase and rationale |

---

## 8. Recommended work sequence

1. Reconcile architecture document against Phase 2 decisions (DEC-022…DEC-041) and remove stale policy statements.
2. Decide framework and platform target after current SDK/host/licence verification.
3. Specify plugin components, buses and live-input strategy; validate with the selected host matrix.
4. Define and cross-check every cross-module contract (host process, parameter registry, commands, snapshots, sample assets, recording, Freeze, transport, MIDI, audio routing, state and deterministic render).
5. Freeze parameter schema and stable host ID strategy before any public automation IDs exist.
6. Specify sample, recording and plugin-state lifecycle, implementing DEC-025 without changing its policy.
7. Define threading, queue capacities, ownership and overload behavior.
8. Verify that the architecture and validation plan fully preserve the already-approved Freeze semantics (DEC-042); close only the remaining engineering details (recording tap, snapshot memory/admission, publication/retirement, transition work and serialization). Do not reopen product behavior.
9. Define resource accounting and deterministic overload policy; separately plan the benchmark needed to set numeric budgets.
10. Complete requirement-to-architecture traceability and independent review.
11. Submit Phase 3 completion PR for owner review. Do not begin production implementation until Phase 3 is approved and Phase 4/5 gates are satisfied.

---

## 9. Explicitly outside this audit

- No production DSP, plugin shell, UI, sample decoder or benchmark code is authorized.
- No numeric CPU, RAM, latency, grain-budget or recording-duration ceiling is approved by this audit.
- No framework, minimum macOS version, AU component type or bus topology is approved.
- No proprietary GR-MEGA firmware, source, presets, artwork or undocumented file format may be copied or reverse-engineered.
- Host compatibility, legal review and distribution remain separate evidence/gates; this audit does not certify them.

**Audit outcome:** Phase 3 is correctly started, but not complete. The repository needs the deliverables in §7 before the architecture can be considered implementation-ready. The traceability table now maps all 124 R1 rows individually to proposed modules, interfaces and validation families; it does not yet provide concrete class/API signatures or passing validation evidence.


## API and signal-flow refinement (2026-11-27)

`PHASE3_API_AND_SIGNAL_FLOW_SPECIFICATION.md` adds candidate service boundaries and typed interface sketches for IF-01…IF-12, plus render ordering, signal-flow topology, recording-tap alternatives and validation obligations. It does not approve framework, ABI, numeric capacities, plugin topology or implementation. The recommended recording/routing semantics are now documented consistently across the API, Lifecycle, DSP and Plugin Bus specifications; they remain proposals pending host and UX validation.


## Recommended signal-flow defaults added (2026-11-27)

The API/signal-flow proposal now recommends one coherent routing contract across engine, recorder and host adapters: layer recording and layer output are post-FX/pre-fader; layer stem capture is independent of mute/solo; master recording is post-layer-mix/pre-host-output and reflects fader/mute/solo; live input feeds each layer before FX and follows the required parallel dry-to-master path (GS-ARC-007), with DEC-040 mono centering; voice chain preserves engine + sub-oscillator → LPF → HPF, with amp-envelope/pan placement pending validation. These are engineering recommendations pending host and UX validation, not new product-level decisions.


## Cross-document consistency audit — 2026-11-27

A deeper concurrency review found an additional Freeze gap: a committed-frame count does not keep a rolling-ring region alive while a non-RT snapshot copy is in progress. The Threading, Lifecycle, API, Interface and Acceptance documents now require a bounded retention lease (or equivalent segment rotation), independent recorder write space, and safe pre-switch failure when retention cannot be guaranteed. Global Freeze is represented as one bounded transaction with per-layer outcomes, preventing four independent commands from applying at different boundaries. The exact retention implementation and capacity remain open; no code or measurement was run.

A deeper comparison against the approved requirement inventory found and corrected a material routing contradiction:

- The initial Phase 3 recommendation to omit a parallel dry live-input path contradicted **GS-ARC-007**, which explicitly requires dry live input to be summed before the master. That recommendation has been removed from the API, DSP, Lifecycle, Plugin Bus, Interface Contracts and Roadmap documents.
- The reconciled proposal now routes live input both into the per-layer wet/FX path and through the required parallel dry path to the master. The master recording tap includes both paths. Per-layer stem/output taps remain post-FX/pre-layer-fader and represent the wet layer path. Host adapters must prevent accidental double summing of layer outputs.
- The voice-chain description now preserves the explicit GS-ARC-007 order: engine + sub-oscillator → LPF → HPF. Amp-envelope and pan placement remain engineering details requiring validation.
- The four serial FX slots per layer (R1 acceptance criteria) are distinct from the seven available effect types approved by DEC-023; there is no count conflict. Exact slot behavior/order and per-effect parameters remain to be reconciled with the Effects Specification.

This is a documentation correction to preserve approved product scope, not a new product decision. The revised routing remains an architecture proposal pending owner review and host/UX validation. No implementation, benchmark or merge was performed.

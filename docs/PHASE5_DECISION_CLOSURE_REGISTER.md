# GRANULAR SYNTH — Phase 5 Decision Closure Register

**Version:** 0.5  
**Date:** 2026-11-28  
**Status:** Consolidated decision register; no unresolved decision is silently approved by this document.  
**Authority:** `DECISIONS_LOG.md`, `OPEN_QUESTIONS.md`, `R1_PRODUCT_SPECIFICATION.md`, `PHASE5_ALL_REQUIREMENTS_AUDIT.md`.

## 1. Purpose

This register turns the all-requirements audit into a finite closure list. It separates:
1. Product behavior decisions.
2. Technical architecture decisions.
3. Cross-cutting owner/host decisions.
4. UI/UX approval.
5. Legal and release decisions.
6. Items explicitly deferred outside R1.

A proposed answer is not an approved answer. Product-level questions may be resolved under the owner's documented product-management delegation only when they do not conflict with an approved decision. That delegation does not authorize framework lock-in, host certification claims, numerical performance commitments, legal conclusions or implementation.

## 2. R1 product behavior gates

These questions affect existing R1 requirements. They must be resolved or explicitly narrowed before final acceptance criteria are frozen.

| Question | Requirement(s) | Current proposal / known baseline | Closure needed | Blocks |
|---|---|---|---|---|
| Q-M03 | GS-GRN-011 | **Approved — DEC-043:** retain last valid Rate/Size inside the near-zero Scan region; resume normal calculation outside it | Specify and validate epsilon, initialization and reset conditions; no numeric threshold approved | Granular engine acceptance details (not product semantics) |
| Q-M04 | GS-GRN-009 | Mode count approved — DEC-045: exactly Bounce, Wrap and Kill; One-Shot is a separate behavior per DEC-044 | Define exact Kill behavior and distinguish it from explicit Stop/Panic; test One-Shot boundary with active grains | Scan behavior tests |
| Q-M09 | GS-GRN-018, GS-ARC-008 | **Approved — DEC-046:** per-grain gain follows 1/√N relative to the single-grain reference | Define N counting domain, transition smoothing and ON/OFF reference details; validate four-layer peak/headroom oracle. Benchmark level matching requires authorized measurement | Grain normalization and mix/headroom tests |
| Q-M14 | GS-GRN-014 | Proposed trigger values: Off / On / Legato | Define each trigger's start/retrigger/legato behavior | Grain-trigger tests |
| Q-M16 | GS-GRN-014 | Manual label changed from Clock Retrigger to Voice Sync in a later release; equivalence remains unverified | Define GRANULAR SYNTH behavior without assuming undocumented benchmark equivalence | Voice/grain clock tests |
| Q-M17 | GS-GRN-015 | Four modes listed: Gradual, Direct, Direct Reset, Recycle | Define each mode's spawn schedule, reset point, capacity response and recycle victim behavior | Spawn-mode tests |
| Q-M18 | GS-VOI-006 | R1 requirement states 0.1 ms minimum | Confirm that 0.1 ms is the product minimum and align OPEN_QUESTIONS with the requirement | Envelope boundary tests |
| Q-I07 | GS-VOI-008 | LPF/HPF, slopes, routing and bypass are in R1; full SVF mode list is unknown | Approve a finite R1 mode list or explicitly constrain R1 to the already stated LPF/HPF behavior | Filter response tests |
| Q-I08 | GS-GRN-006 | Uniform and centre-weighted pan-spray shapes are required | Define the centre-weighted probability curve, normalization and seeded statistical tolerance | Pan-spray tests |
| Q-I12 | GS-VOI-010 | Sub-oscillator is R1; waveform/default is unspecified | Select waveform/default or narrow the requirement explicitly | Sub-oscillator tests |
| FX parameter contract | GS-FX-002/004/006 | Effect inventory is approved; many units/ranges/defaults remain TBD | Define Delay units/range/sync, Chorus/Compressor/Saturator/Reverb ranges/defaults, EQ3 crossover behavior, send range and automation scaling | FX acceptance and host parameter registry |
| MTX-05 | GS-MOD-005 | R1 destination coverage must exclude deferred-only targets | Reconcile the eligible R1 destination inventory against the canonical parameter catalogue | Modulation and parameter registry tests |
| Sampler interpolation | GS-SMP-003 | High-quality interpolation is required; exact modes/default/filter length remain open | Select modes/default and define transition behavior; CPU budget is a separate technical gate | Sampler quality tests |
| Patch comments | GS-PRS-009 | R1 tier is approved; feature behavior/UI remain proposed | Confirm whether patch comments are required in R1 and define editing/search/persistence behavior; do not inherit the benchmark's 100-character limit automatically | Preset/library acceptance |

## 3. Technical architecture decisions

All values below are proposals in `OPEN_QUESTIONS.md`, not approved implementation commitments.

| Gate | Current candidate | Required disposition | Blocks |
|---|---|---|---|
| Q-T01 — Framework | C++20 + JUCE 8 | Approve/reject/defer framework and licensing implications | Build system, AU/VST3/Standalone shell |
| Q-T02 — Core boundary | Framework-independent DSP/core | Approve core/framework boundary and public interfaces | Testability, portability, architecture |
| Q-T03 — Platform | macOS 12+, Universal 2 | Approve minimum macOS and Intel/Apple Silicon support | Packaging, signing, UI/host matrix |
| Q-T04 — Sample-rate strategy | Source-rate playback with compensated increment | Approve conversion/resampling behavior | Pitch, timing and sampler/granular tests |
| Q-T05 — Long samples | RAM for Granular; streaming for long Sampler assets in L2 | Confirm R1 asset residency and defer streaming if not R1 | Memory admission and sample lifecycle |
| Q-T06 — Window implementation | Table bank with per-grain snapshot | Approve implementation approach and numerical oracle | Window correctness and CPU |
| Q-T07 — Global grain budget | Measure on reference machine; fixed budget first | Select reference hardware, budget, overload policy and authorized benchmark thresholds | Global admission, CPU/memory claims |
| Q-T08 — Multithreading | Architect for it; enable after R1 profiling | Explicitly approve R1 single-thread baseline or worker strategy | Render architecture and host integration |
| Q-T09 — RT-safety tooling | Allocation interposer + RTSan in CI | Approve toolchain and CI enforcement | Real-time safety acceptance |
| Q-T10 — Spectral latency | Report only when processing live input | L3-only; defer if Spectral remains L3 | Spectral release, not R1 |
| Q-T11 — Spectral analysis | Hann 4× baseline; 8× quality mode | L3-only; defer if Spectral remains L3 | Spectral release, not R1 |
| Q-T12 — Parameter IDs | Stable string IDs, grouped per layer; expose-all proposal | Approve canonical ID model, host numeric mapping, grouping, scaling and migration | Automation, MIDI Learn, preset compatibility |
| Q-T13 — Modulation control rate | 16 samples | Approve update rate and interpolation/latency contract | Modulation DSP and CPU |
| Q-T14 — Granular interpolation | Cubic Hermite default, selectable | Approve default and numerical/performance contract | Granular audio quality |
| Q-T15 — State format | JSON metadata + external/embedded WAV | Approve schema, versioning, atomic save, migration and size/admission policy | Presets, recordings, Freeze restore |
| Q-T16 — Decoder | AudioToolbox primary + portable fallback | Approve decoder dependencies, format support and failure behavior | Sample import and portability |
| Q-T17 — Audio matrix | 44.1–192 kHz, 16–4096 frames, variable blocks; zero added latency except Spectral | Approve supported sample rates/block sizes and define measurable latency contract | Host/device integration and DSP tests |
| Q-P06 — Live-input host topology | AU/VST3 component and bus details remain open | Choose/validate format-specific live-input routing and component strategy | Plugin live input and output buses |
| Q-P09 — Host matrix | Logic, Live, Bitwig, REAPER proposed | Approve exact host versions, formats and certification matrix | Host acceptance and compatibility claims |
| Q-P10 — Reference machine | No approved reference Mac | Select machine/configuration before numeric performance thresholds | CPU, memory, load-time and latency tests |

**Disposition rule:** every Q-T01…Q-T17 item must be approved, rejected or explicitly deferred with a target phase. A proposed value must not become a release promise by appearing in a specification.

## 4. Cross-cutting architecture and host decisions

| Gate | Required decision | Main impact |
|---|---|---|
| AR-01 | Same-sample-offset precedence and deterministic ordering for host automation, MIDI, sequencer locks and UI writes | Parameter/event correctness and sequencer automation |
| AR-02 | Voice/grain overload, voice stealing, global grain reclamation, thresholds and safe fade behavior | Allocator, DSP safety and overload tests |
| AR-03 | AU MPE same-pitch Note Off matching, shared member-channel behavior, sustain and zone reconfiguration | AU MPE correctness |
| AR-04 | VST3 Note Expression Pressure/Timbre discovery and event mapping | VST3 MPE correctness |
| AR-05 | AU/VST3 metadata and conversion for DEC-038's 48-semitone per-note range | Pitch-expression parity |
| AR-06 | State restore while old voices/notes are active: terminate immediately or allow non-addressable old-generation tails | State restore and note-identity safety |

### Freeze transaction sub-decision

DEC-042 Source Snapshot behavior is approved. One owner decision remains: when a global Freeze preparation succeeds for some layers and fails for others, choose between:
- successful layers switch together at one boundary while failed layers remain unchanged; or
- the whole four-layer operation aborts.

`AC-AUD-028` is proposed and must not be treated as approved until this is decided. Memory admission, retention leases, crossfade and retirement remain technical validation work.

## 5. Phase 4 UI/UX owner review

These items are documented proposals, not approved UI behavior:

- Granular and Sampler A–H encoder assignments.
- Chord Record Arm/Capture/Commit/Cancel workflow and overlap/duplicate semantics.
- Recording target selection, arming, start/stop, status and concurrency for layer/master capture.
- Performance Bank editor, active-bank persistence and Program Change presentation.
- Six workspace layouts, global frame, performance row, overlays and navigation.
- Final controller-only essential-operation checklist and usability sign-off.
- Patch-comment workflow for `GS-PRS-009`.

**Exit condition:** owner accepts or rejects each proposal, and the accepted workflows receive executable controller/UI acceptance cases.

## 6. Commercial, legal and release gates

| Gate | Timing / impact | Required disposition |
|---|---|---|
| Q-P03 | Phase 6 distribution | Decide internal/commercial distribution, installer and copy-protection policy |
| Q-P01 | Phase 8 identity | Final product name and visual identity |
| Q-P14 | Phase 8 content | Factory preset/content scope and licensing |
| Q-L01…Q-L07 | Before relevant feature/distribution | Resolve import/IP, trademark, framework and third-party licensing questions with appropriate legal review |
| Q-P13 | Deferred benchmark research | Physical GR-MEGA access is not an R1 requirement |
| Q-I15 | Separate research task | Firmware roadmap research; not an R1 acceptance blocker |

## 7. Explicitly deferred from R1

These questions must not silently expand R1:
- Q-M15: per-layer time-signature emulation.
- Q-I16: keyboard-position voice-stealing heuristic; R1 priority remains `GS-VOI-001`.
- Q-I17: separate benchmark “Pos Y” source; R1 uses the DEC-032 two-axis XY pad.
- Q-I18: Elektron-style sequencer preset switching; R1 uses the approved Pattern/Presets model.
- Q-T10/Q-T11: Spectral latency and PV analysis choices while Spectral remains L3.
- L2/L3/OPT features remain outside R1 unless a new scope decision is approved.

## 8. Closure order

1. Resolve R1 product behavior and domain-spec gates in §2.
2. Resolve AR-01/02 and the MPE/state-restore owner gates in §4.
3. Approve the technical architecture disposition list in §3.
4. Complete Phase 4 UI/UX owner review in §5.
5. Finalize all 124 R1 acceptance links/cases, including the 71 currently unlinked requirements.
6. Approve test harness, fixtures, CI enforcement and numeric thresholds.
7. Only then request explicit authorization for a named implementation milestone.

## 9. Current state

This register is a planning artifact. It does not close any open question, approve proposed values, authorize implementation or imply that tests have passed. See `PHASE5_ALL_REQUIREMENTS_AUDIT.md` for the complete requirement and traceability baseline.

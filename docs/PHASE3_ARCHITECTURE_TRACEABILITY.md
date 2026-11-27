# Phase 3 R1 Architecture Traceability

**Version:** 0.1 — Phase 3 proposal  
**Status:** Initial 124-row mapping against `GRANULAR_SYNTH_REQUIREMENTS.md` v1.0. This is an architecture coverage proposal, not evidence that the modules are implemented or tested.  
**Baseline:** DEC-041 and `R1_RELEASE_SCOPE_MATRIX.md`.

## 1. Coverage method

Each approved R1 requirement is assigned to:
- a primary architecture owner/module;
- a cross-module interface where applicable;
- an observable validation family.

The table maps all 124 R1 rows from the approved inventory. A mapping is not an acceptance test pass. Numeric performance criteria remain Phase 5/7 work.

## 2. Requirement-to-architecture mapping

| Requirement area | R1 requirement IDs | Primary module / owner | Key interfaces | Validation family |
|---|---|---|---|---|
| Platform | GS-PLT-001…004 | `host/`, `core/`, `state/` | Standalone/AU/VST3 adapters; shared engine; render contract | Build, plugin validation, host smoke, deterministic equivalence |
| Architecture/layers | GS-ARC-001…008 | `engine/layer`, `engine/voice`, `engine/grain`, `engine/fx`, `host/` | Four layer runtimes; global scheduler; layer/master buses | Four-layer routing, voice/grain limits, deterministic overload, signal-flow tests |
| Granular engine | GS-GRN-001…020 | `engine/grain`, `core/dsp`, `sample/` | Scheduler, grain pool, window bank, interpolation, sample handle, RNG | Spawn timing, parameter math, windows, aliasing, discontinuity and CPU tests |
| Chord/scale/arp | GS-CHD-001…005 | `engine/voice`, `engine/grain`, `engine/seq` | Chord state, pitch/scale mapper, arp scheduler, note events | Chord distribution, note ordering, scale quantization, arp determinism |
| Sampler | GS-SMP-001…004 | `engine/engines/SamplerEngine`, `sample/`, `engine/voice` | Playback head, loop state, interpolation, voice allocation | Forward/reverse, loop/one-shot/bounce, loop release, interpolation, retrigger |
| Voice | GS-VOI-001…006, GS-VOI-008…010 | `engine/voice`, `core/dsp`, `engine/mod` | Voice allocator, envelopes, filters, sub-osc, MPE note map | Stealing continuity, CC64, glide, velocity, pitch bend, envelope/filter response |
| Sample management / I/O | GS-SMG-001…012, GS-SMG-017, GS-SMG-019…021 | `sample/`, `host/`, `state/` | Decoder, immutable sample, recorder, live input, freeze snapshot, export, state | File-format matrix, load/swap, recording soak, sample duration, Freeze semantics, callback RT-safety |
| Modulation | GS-MOD-001…009 | `engine/mod`, `state/`, `core/dsp` | LFO, source/destination registry, matrix, macro values, modulation snapshots | LFO modes, matrix reachability, macro semantics, modulation smoothness |
| Sequencer | GS-SEQ-001…005, GS-SEQ-008…009, GS-SEQ-011…012 | `engine/seq`, `engine/voice`, `engine/mod`, `state/` | Shared transport, pattern/event model, event scheduler, draft/undo state | Timing, sync, pattern switching, fill/mute, live edit, draft/undo and controller tests |
| MIDI | GS-MID-001…003, GS-MID-005…008 | `host/`, `engine/voice`, `engine/seq`, `state/` | MIDI adapter, MPE router, mapping registry, Program Change bank, clock adapter | Channel routing, MPE isolation, CC/NRPN, Program Change, Clock, panic |
| Effects | GS-FX-001…002, GS-FX-004, GS-FX-006 | `engine/fx`, `engine/layer`, `state/` | Four-slot chain, send paths, effect parameter registry, tail policy | FX routing, automation/modulation, tail/cut and swap tests |
| Presets/library | GS-PRS-001…009 | `state/`, `sample/`, `engine/seq` | Sample/Patch/Multi/Project model, Pattern Library, serialization, migration, undo | Save/restore, relink/collect, migration, copy/paste, undo/redo |
| UI | GS-UI-001…007 | `ui/`, `host/`, `state/` | Snapshot model, editor views, parameter gestures, controller mapping | UI responsiveness, HiDPI/accessibility, waveform/grain display, same editor component |
| Non-functional | GS-NFR-001…013 | Cross-cutting: `core/rt`, `host/`, `sample/`, `state/`, CI | RT guardrails, deterministic renderer, metrics, recovery and signing | RT-safety, CPU/RAM/latency, golden renders, host matrix, corrupted-state recovery |

## 3. Exact inventory reconciliation

| Area | Count |
|---|---:|
| PLT | 4 |
| ARC | 8 |
| GRN | 20 |
| CHD | 5 |
| SMP | 4 |
| VOI | 9 |
| SMG | 16 |
| MOD | 9 |
| SEQ | 9 |
| MID | 7 |
| FX | 4 |
| PRS | 9 |
| UI | 7 |
| NFR | 13 |
| **Total R1 rows mapped** | **124** |

The R1 mapping includes the R1 portion of split-tier requirements:
- GS-ARC-005: Granular and Sampler are R1; Granular Slice is deferred.
- GS-SMG-005: waveform overview is R1; spectrogram is L2.
- GS-MOD-004: R1 CV/modulation behavior only; Draw XY remains outside the R1 commitment where specified by the source row.
- Other mixed-scope rows retain their precise R1 subset as defined in `GRANULAR_SYNTH_REQUIREMENTS.md` v1.0 and `R1_RELEASE_SCOPE_MATRIX.md`.

## 4. Cross-cutting interface contracts still required

| Interface | Producer → consumer | Contract required |
|---|---|---|
| Parameter registry | UI/host/state → DSP/modulation/MIDI | Stable string and numeric IDs, ranges, curves, flags, smoothing and migration |
| Structural command | UI/loader/state → audio | Fully prepared immutable payload, bounded queue, safe-point swap, retirement |
| Audio snapshot | DSP → UI/meters | Fixed-size, non-blocking, versioned observational data |
| Sample handle | Loader/state → engine/voice | Immutable sample data, lifetime and safe retirement |
| MIDI event | Host/device → voice/sequencer | Timestamp/sample offset, channel/MPE context, deterministic ordering |
| Transport snapshot | Host/internal/MIDI Clock/Link → sequencer | Single clock authority, PPQ/tempo/playing state and seek/reset semantics |
| Recording buffer | Input/engine → sample/state/export | Single-writer ownership, bounded capacity, stable snapshot and overflow policy |
| Freeze snapshot | Source recorder/sample → layer engine | Immutable captured region, safe publish/thaw, memory ownership and failure state |
| Plugin state | Host ↔ state/sample | Versioned snapshot, embedded unsaved recordings, external sample references, recovery |
| Audio buses | Host/device ↔ layer/master mixer | Explicit channel layout, active/inactive bus behavior and fallback semantics |

## 5. Unmapped/uncertain status

- No R1 requirement is intentionally left unmapped in this initial pass.
- The mapping is by domain and module boundary, not yet by individual class/API. Class-level design belongs in the architecture specification.
- Freeze requirements are mapped to the sample/I/O domain but remain behaviorally underspecified until `FREEZE_BEHAVIOR_SPECIFICATION.md` is approved.
- Numeric acceptance criteria, host-specific validation, measured budgets and test implementations remain Phase 5/7 deliverables.
- The 124 count is reconciled to the approved R1 inventory; this table does not reclassify tiers.

**Audit result:** 124/124 approved R1 requirements have a proposed architecture owner. Implementation readiness remains blocked by the open decisions and interface contracts listed above.

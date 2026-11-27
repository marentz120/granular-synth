# Phase 3 R1 Architecture Traceability

**Version:** 0.3 — Phase 3 proposal  
**Status:** Initial 124-row mapping against `GRANULAR_SYNTH_REQUIREMENTS.md` v1.0. This is an architecture coverage proposal, not evidence that the modules are implemented or tested.  
**Baseline:** DEC-041 and `R1_RELEASE_SCOPE_MATRIX.md`.

## 1. Coverage method

Each approved R1 requirement is assigned to:
- a primary architecture owner/module;
- a cross-module interface where applicable;
- an observable validation family.

The domain-level mapping below is refined by `PHASE3_INTERFACE_CONTRACTS.md`, which defines 12 cross-domain interface contracts, ownership, timing, failure behavior and minimum validation obligations. That refinement is still a proposal and does not constitute passing test evidence.

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

## 5. Interface-level refinement

See `PHASE3_INTERFACE_CONTRACTS.md` for IF-01…IF-12. It defines the cross-domain contract obligations for host processing, parameter registry, structural commands, UI snapshots, sample assets, recording, Freeze, transport, MIDI, audio routing, plugin state and deterministic rendering. The area-to-interface index in that document refines the broad interface column above.

The 124 requirement rows now have individual module/interface/validation-family assignments. Exact C++ class/API signatures, method contracts, data layouts and per-test pass conditions remain an engineering specification task before implementation approval.

## 6. Per-requirement interface mapping (124 R1 rows)

The following expansion maps each approved R1 row individually to its primary module owner, applicable IF contracts and planned validation family. These are proposed design/validation assignments, not implementation evidence or passing tests. Requirement descriptions and tier authority remain in `GRANULAR_SYNTH_REQUIREMENTS.md` v1.0 and `R1_RELEASE_SCOPE_MATRIX.md`.

| Requirement | Primary module owner | Interfaces | Validation family (planned) |
|---|---|---|---|
| GS-PLT-001 | host/ | IF-01, IF-11, IF-12 | host callback, plugin state, deterministic render |
| GS-PLT-002 | host/ | IF-01, IF-11, IF-12 | host callback, plugin state, deterministic render |
| GS-PLT-003 | host/ | IF-01, IF-11, IF-12 | host callback, plugin state, deterministic render |
| GS-PLT-004 | host/ | IF-01, IF-11, IF-12 | host callback, plugin state, deterministic render |
| GS-ARC-001 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-ARC-002 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-ARC-003 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-ARC-004 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-ARC-005 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-ARC-006 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-ARC-007 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-ARC-008 | engine/layer, engine/voice, engine/grain, engine/fx | IF-01, IF-03, IF-09, IF-10, IF-12 | layer isolation, routing, voice/grain scheduling, overload |
| GS-GRN-001 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-002 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-003 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-004 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-005 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-006 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-007 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-008 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-009 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-010 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-011 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-012 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-013 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-014 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-015 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-016 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-017 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-018 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-019 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-GRN-020 | engine/grain, core/dsp, sample/ | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | grain timing, parameter math, window/interpolation, aliasing, seeded render |
| GS-CHD-001 | engine/voice, engine/grain, engine/seq | IF-08, IF-09, IF-12 | chord distribution, note ordering, scale/arp determinism |
| GS-CHD-002 | engine/voice, engine/grain, engine/seq | IF-08, IF-09, IF-12 | chord distribution, note ordering, scale/arp determinism |
| GS-CHD-003 | engine/voice, engine/grain, engine/seq | IF-08, IF-09, IF-12 | chord distribution, note ordering, scale/arp determinism |
| GS-CHD-004 | engine/voice, engine/grain, engine/seq | IF-08, IF-09, IF-12 | chord distribution, note ordering, scale/arp determinism |
| GS-CHD-005 | engine/voice, engine/grain, engine/seq | IF-08, IF-09, IF-12 | chord distribution, note ordering, scale/arp determinism |
| GS-SMP-001 | engine/engines/SamplerEngine, sample/, engine/voice | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | playback/loop boundaries, note events, sample lifetime, interpolation |
| GS-SMP-002 | engine/engines/SamplerEngine, sample/, engine/voice | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | playback/loop boundaries, note events, sample lifetime, interpolation |
| GS-SMP-003 | engine/engines/SamplerEngine, sample/, engine/voice | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | playback/loop boundaries, note events, sample lifetime, interpolation |
| GS-SMP-004 | engine/engines/SamplerEngine, sample/, engine/voice | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | playback/loop boundaries, note events, sample lifetime, interpolation |
| GS-VOI-001 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-002 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-003 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-004 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-005 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-006 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-008 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-009 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-VOI-010 | engine/voice, core/dsp, engine/mod | IF-02, IF-03, IF-09, IF-10, IF-12 | voice allocation/stealing, envelopes, filters, pitch/MPE |
| GS-SMG-001 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-002 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-003 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-004 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-005 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-006 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-007 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-008 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-009 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-010 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-011 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-012 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-017 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-019 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-020 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-SMG-021 | sample/, host/, state/ | IF-01, IF-05, IF-06, IF-07, IF-11 | decode/load, record soak/taps, Freeze, state/relink |
| GS-MOD-001 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-002 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-003 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-004 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-005 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-006 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-007 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-008 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-MOD-009 | engine/mod, state/, core/dsp | IF-02, IF-03, IF-04, IF-09 | registry reachability, modulation scope/order, smoothness |
| GS-SEQ-001 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-002 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-003 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-004 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-005 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-008 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-009 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-011 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-SEQ-012 | engine/seq, engine/voice, engine/mod, state/ | IF-03, IF-08, IF-09 | transport timing, event order, live edits, controller path |
| GS-MID-001 | host/, engine/voice, engine/seq, state/ | IF-01, IF-08, IF-09 | MIDI routing, MPE, CC, Program Change, clock/panic |
| GS-MID-002 | host/, engine/voice, engine/seq, state/ | IF-01, IF-08, IF-09 | MIDI routing, MPE, CC, Program Change, clock/panic |
| GS-MID-003 | host/, engine/voice, engine/seq, state/ | IF-01, IF-08, IF-09 | MIDI routing, MPE, CC, Program Change, clock/panic |
| GS-MID-005 | host/, engine/voice, engine/seq, state/ | IF-01, IF-08, IF-09 | MIDI routing, MPE, CC, Program Change, clock/panic |
| GS-MID-006 | host/, engine/voice, engine/seq, state/ | IF-01, IF-08, IF-09 | MIDI routing, MPE, CC, Program Change, clock/panic |
| GS-MID-007 | host/, engine/voice, engine/seq, state/ | IF-01, IF-08, IF-09 | MIDI routing, MPE, CC, Program Change, clock/panic |
| GS-MID-008 | host/, engine/voice, engine/seq, state/ | IF-01, IF-08, IF-09 | MIDI routing, MPE, CC, Program Change, clock/panic |
| GS-FX-001 | engine/fx, engine/layer, state/ | IF-03, IF-10 | FX chain routing, automation, tails and swap |
| GS-FX-002 | engine/fx, engine/layer, state/ | IF-03, IF-10 | FX chain routing, automation, tails and swap |
| GS-FX-004 | engine/fx, engine/layer, state/ | IF-03, IF-10 | FX chain routing, automation, tails and swap |
| GS-FX-006 | engine/fx, engine/layer, state/ | IF-03, IF-10 | FX chain routing, automation, tails and swap |
| GS-PRS-001 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-002 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-003 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-004 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-005 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-006 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-007 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-008 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-PRS-009 | state/, sample/, engine/seq | IF-02, IF-05, IF-11 | save/restore, migration, relink/collect, undo |
| GS-UI-001 | ui/, host/, state/ | IF-02, IF-04, IF-09 | snapshot safety, interaction, controller mapping |
| GS-UI-002 | ui/, host/, state/ | IF-02, IF-04, IF-09 | snapshot safety, interaction, controller mapping |
| GS-UI-003 | ui/, host/, state/ | IF-02, IF-04, IF-09 | snapshot safety, interaction, controller mapping |
| GS-UI-004 | ui/, host/, state/ | IF-02, IF-04, IF-09 | snapshot safety, interaction, controller mapping |
| GS-UI-005 | ui/, host/, state/ | IF-02, IF-04, IF-09 | snapshot safety, interaction, controller mapping |
| GS-UI-006 | ui/, host/, state/ | IF-02, IF-04, IF-09 | snapshot safety, interaction, controller mapping |
| GS-UI-007 | ui/, host/, state/ | IF-02, IF-04, IF-09 | snapshot safety, interaction, controller mapping |
| GS-NFR-001 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-002 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-003 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-004 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-005 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-006 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-007 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-008 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-009 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-010 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-011 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-012 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |
| GS-NFR-013 | core/rt, host/, sample/, state/, CI | IF-01…IF-12 | RT safety, determinism, CPU/RAM/latency, recovery, host matrix |

**Count check:** 124 rows selected from the source inventory by an R1 tier marker. Mixed-tier rows are included only for their R1 commitment. No L2/L3/OPT-only row is promoted by this table.

## 7. Unmapped/uncertain status

- No R1 requirement is intentionally left without a proposed module/interface/validation-family assignment.
- Exact C++ class/API signatures, method contracts, data layouts and per-test pass conditions remain an engineering specification task before implementation approval.
- Freeze product behavior is approved by DEC-042 and specified in `FREEZE_BEHAVIOR_SPECIFICATION.md` v1.0; engineering validation still requires sample/I/O, RT ownership, state persistence and host integration evidence.
- Numeric acceptance criteria, host-specific validation, measured budgets and test implementations remain Phase 5/7 deliverables.
- The 124 count is reconciled to the approved R1 inventory; this table does not reclassify tiers.

**Audit result:** 124/124 approved R1 requirements have individual proposed module/interface/validation-family assignments, and the cross-domain contract set IF-01…IF-12 is documented. This is not yet concrete C++ class/API design and does not show that tests pass. Implementation readiness remains blocked by open technical decisions, host evidence, signal-flow details and measured resource limits.


## 8. R1 acceptance-criterion cross-check — 2026-11-27

A mechanical cross-check compared the 124 requirement IDs in §6 against the requirement references present in `R1_ACCEPTANCE_CRITERIA.md`. This is a **trace-link coverage check**, not a judgment that an unreferenced requirement is unimportant or that a referenced criterion has passed.

| Area | R1 requirements | Explicitly referenced by current AC | No explicit AC reference |
|---|---:|---:|---:|
| PLT | 4 | 2 | 2 |
| ARC | 8 | 1 | 7 |
| GRN | 20 | 0 | 20 |
| CHD | 5 | 0 | 5 |
| SMP | 4 | 4 | 0 |
| VOI | 9 | 0 | 9 |
| SMG | 16 | 7 | 9 |
| MOD | 9 | 6 | 3 |
| SEQ | 9 | 1 | 8 |
| MID | 7 | 7 | 0 |
| FX | 4 | 4 | 0 |
| PRS | 9 | 7 | 2 |
| UI | 7 | 0 | 7 |
| NFR | 13 | 8 | 5 |
| **Total** | **124** | **47** | **77** |

The 77 IDs without an explicit acceptance-criterion reference are:

- **PLT:** GS-PLT-002, GS-PLT-003
- **ARC:** GS-ARC-001, GS-ARC-002, GS-ARC-003, GS-ARC-004, GS-ARC-006, GS-ARC-007, GS-ARC-008
- **GRN:** GS-GRN-001…020
- **CHD:** GS-CHD-001…005
- **VOI:** GS-VOI-001…006, GS-VOI-008…010
- **SMG:** GS-SMG-002…007, GS-SMG-010, GS-SMG-011, GS-SMG-019
- **MOD:** GS-MOD-002, GS-MOD-005, GS-MOD-007
- **SEQ:** GS-SEQ-001…004, GS-SEQ-008, GS-SEQ-009, GS-SEQ-011, GS-SEQ-012
- **PRS:** GS-PRS-008, GS-PRS-009
- **UI:** GS-UI-001…007
- **NFR:** GS-NFR-004, GS-NFR-005, GS-NFR-009, GS-NFR-011, GS-NFR-013

**Interpretation and next gate:** the current acceptance document is a partial product-level criteria draft, not a complete 124-row acceptance matrix. The source document itself assigns detailed acceptance criteria and test design to Phase 5. Therefore this finding is recorded as a Phase 5 traceability deliverable, not silently filled with invented pass conditions in Phase 3. Before Phase 5 test implementation, every one of the 124 R1 IDs must link to either (a) one or more explicit acceptance/test IDs, or (b) a documented rationale for being validated through a cross-cutting criterion. Existing criteria are not evidence of passing tests.

**Mechanical result:** 124 unique R1 traceability rows; 47 have an explicit requirement reference somewhere in the current acceptance document; 77 do not. No requirement tier or approved product behavior is changed by this audit.


## 9. Candidate API-symbol traceability — 2026-11-27

The 124 individual requirement rows now have a second-level candidate symbol/operation assignment in PHASE3_API_SYMBOL_TRACEABILITY.md, linked to the existing IF-01…IF-12 contracts and the current acceptance-criteria references. This is a proposed symbol vocabulary only; no final C++ signatures, ABI, implementation or public parameter IDs are approved. The current acceptance cross-check remains 47/124 explicit links and 77 gaps; the new symbol mapping does not fabricate test IDs or close Phase 5 acceptance work.

The candidate symbol catalogue is in PHASE3_API_AND_SIGNAL_FLOW_SPECIFICATION.md §14. Framework, AU component strategy, host bus layout, AR-01/AR-02, Q-T03/Q-T12, numeric resource limits and DEC-042 mixed-failure semantics remain open as recorded in the decision register.

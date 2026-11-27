# MIDI Specification

**Version:** 0.1 — Phase 2 working specification  
**Status:** Proposed; subject to owner review. No MIDI implementation authorized.  
**Authority:** GRANULAR_SYNTH_REQUIREMENTS.md GS-MID-001…008; R1_PRODUCT_SPECIFICATION.md §5; LIVE_PERFORMANCE_SPECIFICATION.md; DEC-016, DEC-019, DEC-021.  
**Benchmark evidence:** GR_MEGA_FEATURE_MAP.md §11.1.

## 1. R1 scope and governing decisions

R1 supports generic class-compliant MIDI controllers and MIDI input for note/performance control. Each layer has a MIDI channel. MPE is required. MIDI Learn and manual mapping are supported for CC, channel and Note; 14-bit CC and NRPN input and optional soft-takeover are required by the current requirement. Program Change recalls presets where supported. MIDI Clock input is required in Standalone. Channel-mode messages include All Notes Off, All Sound Off and Reset Controllers.

Standalone essential live-performance operations, including all sequencer functions and editing, must be reachable through a generic MIDI controller without keyboard/mouse (DEC-016 and DEC-019). R1 does not require controller feedback, named-controller templates or controller-specific auto-mapping (DEC-021).

## 2. MIDI note and channel behavior

- Each layer has an independently configurable MIDI receive channel (GS-MID-001).
- Channel assignment must be unambiguous when multiple layers use the same channel; the product must define whether same-channel notes trigger all matching enabled layers or a selected/focused layer. This behavior is unresolved and must be decided before implementation.
- Note-on triggers the selected layer voice according to the layer's engine and voice configuration.
- Note-off releases the corresponding voice; sustain/hold CC64 follows the shared voice requirements.
- Velocity, pitch bend, channel/poly aftertouch and MPE timbre are routed to the corresponding voice/layer sources.
- Per-layer channel filtering and MPE zone behavior must not leak expression data between unrelated layers.

## 3. MPE

- R1 supports per-note pitch, pressure and timbre expression.
- The benchmark documents Strike/Slide/Pressure/Timbre(CC74) and pitch-bend range 48 semitones. GRANULAR SYNTH's exact MPE compatibility profile and zone configuration remain to be specified; do not hardcode the benchmark's channel allocation without review.
- MPE expression must be associated with the correct note/voice across overlapping notes.
- Channel aftertouch behavior in MPE mode must be defined consistently with poly pressure; outside MPE, channel aftertouch remains a channel-level source.
- Pitch-bend range compatibility must be deterministic and communicated to the host/controller where protocol support exists.

## 4. MIDI Learn and mappings

- Any automatable parameter can be assigned through MIDI Learn or manual mapping, subject to the final parameter-exposure decision Q-T12.
- Mapping types include CC, MIDI channel and Note, per R1_PRODUCT_SPECIFICATION.md §5.
- Global and per-preset mapping scopes, conflict detection and recovery follow LIVE_PERFORMANCE_SPECIFICATION.md.
- Soft-takeover/pickup prevents a mapped hardware control from causing a value jump when its physical position differs from the current software value.
- Mapping activation, deactivation, conflict and recovery must be visible to the user.
- A saved mapping set needed during a performance must be recallable using the controller; building/editing a mapping list may use the UI as documented in LIVE_PERFORMANCE_SPECIFICATION.md §8.
- No R1 controller feedback is required.

## 5. CC, 14-bit CC and NRPN

- Support incoming 7-bit CC and 14-bit CC where the controller sends paired MSB/LSB messages.
- Support NRPN input for the parameters covered by the mapping/parameter model.
- The mapping layer must distinguish incomplete, stale or terminated NRPN sequences and must not apply partial data as a valid parameter value.
- The exact NRPN timeout, pair handling, scaling and precedence between 7-bit and 14-bit messages are implementation-level protocol details to be specified and tested in Phase 3/5.
- Benchmark CC-number compatibility is L2 in current requirements; it is not an R1 requirement.
- MIDI output of parameter feedback and sequencer notes/clock is not an R1 requirement (sequencer MIDI output is L3).

## 6. Program Change

- Program Change receive can be enabled/disabled.
- In Standalone, Program Change recalls a preset as required by GS-MID-005.
- Plugin behavior is supported only where the host/format permits it.
- The mapping from bank select/program number to the searchable, unbounded library and Performance Set is unresolved; do not assume the benchmark's fixed 128-slot structure.
- Program Change must not silently interrupt the active Pattern; the R1 rule that changing Preset does not replace or stop the active Pattern still applies.

## 7. Clock and transport

- Standalone accepts external MIDI Clock.
- The central sequencer synchronizes to internal BPM, external MIDI Clock, Ableton Link and DAW/host transport as required by GS-SEQ-005.
- Host/DAW synchronization is used in plugin contexts; exact host API and clock arbitration are Phase 3.
- Clock loss, unstable clock, Start/Continue/Stop, song-position pointer and clock restart behavior must be explicitly specified before implementation.
- No per-layer independent sequencer clock is introduced; shared transport follows the approved R1 product spec.
- MIDI Clock/transport output is L3 and is not an R1 commitment.

## 8. Channel-mode and panic behavior

- All Notes Off releases active notes through normal release behavior.
- All Sound Off immediately silences voices and effect tails, consistent with the mapped benchmark behavior and GS-MID-006.
- Reset Controllers restores controller-driven modulation/expression state to defined neutral/default values without changing saved patch values.
- Panic is reachable in one controller gesture at all times during standalone performance (LIVE_PERFORMANCE_SPECIFICATION.md §9).
- Freeze state interaction with panic is not inferred; Freeze semantics are a Phase 3 decision under GS-SMG-021.

## 9. Live operation

- All essential operations listed in LIVE_PERFORMANCE_SPECIFICATION.md §1 and all central sequencer functions/editing must be controller-reachable.
- Generic class-compliant controllers are the baseline. The product must not promise identical ergonomics across controllers with different physical controls.
- Mapping recall and live setup changes required mid-set must be accessible; initial device selection and detailed mapping construction may remain setup-time.
- No motorized-fader, LED-ring, display or other MIDI feedback is required in R1.

## 10. Unresolved questions

| Question | Status |
|---|---|
| Same MIDI channel assigned to multiple layers | Product behavior: all matching layers receive the event; implementation/configuration UX Phase 3/4 |
| MPE zone/master/member channel configuration | OPEN; exact compatibility profile |
| MPE pitch-bend range and negotiation | OPEN; benchmark documents 48 semitones, product profile not fully settled |
| Program Change mapping to library/performance bank | Phase 4 product interaction detail; unmapped messages must not recall arbitrary content |
| Clock loss/restart/song-position behavior | OPEN |
| NRPN timeout, termination and 7/14-bit precedence | Phase 3 protocol detail |
| Exact host parameter exposure | Q-T12 / Phase 3 |
| Benchmark CC map compatibility | L2 |
| MIDI feedback and named controller templates | Explicitly out of R1 by DEC-021 |

## 11. Acceptance intent

Tests must cover per-layer channel routing, overlapping notes, MPE expression isolation, MIDI Learn/manual mapping, soft takeover, mapping conflicts/recovery, Program Change, MIDI Clock input and clock-loss handling once specified, channel-mode messages, panic reachability and controller-only operation for the full essential-operation list. Host-format-specific behavior is tested after Phase 3 integration decisions.

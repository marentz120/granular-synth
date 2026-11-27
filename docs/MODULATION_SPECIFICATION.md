# Modulation Specification

**Version:** 0.1 — Phase 2 working specification  
**Status:** Product behavior approved for Phase 2; DEC-024, DEC-031 and DEC-032 settle the relevant product semantics. This document does not authorize DSP implementation.  
**Authority:** R1_PRODUCT_SPECIFICATION.md, GRANULAR_SYNTH_REQUIREMENTS.md GS-MOD-001…012, DEC-015…021.  
**Benchmark evidence:** GR_MEGA_FEATURE_MAP.md §10.1–10.4. Manual-derived behavior is identified as benchmark reference, not automatically a product requirement.

## 1. Purpose and scope

The modulation system provides reusable control signals that can influence sound-generation, layer, envelope, filter, effect and macro parameters. It must work consistently in Standalone and plugin builds, and must be addressable through the common parameter model used by MIDI Learn, host automation and sequencer modulation.

R1 includes four LFOs per layer, a modulation matrix with at least 50 rows, direct LFO routing, the approved R1 modulation sources and destinations, 10 macros per patch, envelope following, per-note random sources, and visible summed modulation. Drawn XY trajectories and CV input are not R1 commitments.

## 2. R1 functional inventory

| Capability | R1 behavior | Traceability |
|---|---|---|
| LFO count | Four independent LFO instances per layer | GS-MOD-001 |
| LFO rate | Free-running 0–50 Hz or tempo-synchronized division | GS-MOD-001; LFO-02 |
| Waveforms | Sine, triangle, rising saw, falling saw, square, random/sample-and-hold | GS-MOD-001; LFO-03 |
| LFO controls | Phase, amplitude quantization, four polarity modes, key sync Off/On/Legato | GS-MOD-001; LFO-05…08 |
| Direct route | Each LFO can target a parameter with an amount, independent of matrix rows | GS-MOD-006; LFO-09 |
| Matrix | At least 50 rows; Enable, Source, Curve, Amount, Polarity, Destination | GS-MOD-003; MTX-01…03 |
| Matrix operations | Add, enable/disable, edit, copy, paste, delete rows | GS-MOD-007; MTX-09 |
| Sources | LFO1–4, Mod Wheel, Key Track, Velocity, Pitch Bend, Aftertouch, MPE Timbre, Release Velocity, Aux Envelope, Random1–4, XY performance pad, sequencer mod lanes, input/layer envelope follower | GS-MOD-004 |
| Destinations | Every continuous parameter belonging to an R1 engine, voice, envelope, filter, LFO, effect and macro; benchmark parity target is at least 149 only where corresponding R1 parameters exist | GS-MOD-005; MTX-05 |
| Macros | 10 per patch; multi-destination assignments with start/end range, curve and invert; macros can themselves be modulated and are host-automatable | GS-MOD-008; MAC-01…06 |
| Feedback | Show summed modulation on controls and a recent-destinations monitor | GS-MOD-009; MTX-14 |
| Envelope follower | Input or selectable layer source; attack/release and gain controls | GS-MOD-004; MTX-11 |
| Random sources | Four independent per-note random streams | GS-MOD-004; GEN-01 |

## 3. Functional semantics

### 3.1 Modulation signal model

- A modulation source produces a normalized control signal.
- Each matrix row applies its enabled state, source, curve, amount and polarity before contributing to its destination.
- Multiple active rows targeting one parameter are summed into a single modulation contribution. Direct LFO routing contributes to the same final destination value; it must not create a separate, contradictory control path.
- The parameter's base value remains independently editable. Modulation changes the effective value, not the stored base value.
- The effective value is constrained to the destination's legal range. The exact clamping/saturation rule must be consistent and visible; no out-of-range value may reach DSP.
- Parameter smoothing and click-safety follow the destination's requirements and GS-NFR-003. Modulation must not bypass smoothing merely because it is high-rate.
- A disabled row contributes zero and remains stored in the patch.
- An unavailable source contributes neutral modulation and must not corrupt or reset the destination.

### 3.2 Curves and polarity

The benchmark describes a curve control from low-flat through linear to high-flat, and four polarity modes: positive-unipolar, negative-unipolar, positive-bipolar and negative-bipolar. R1 shall expose the same functional concepts. Exact curve equation and UI taper are to be documented as implementation-independent mathematical definitions before DSP implementation; they are not inferable exactly from the manual.

### 3.3 LFO behavior

- Free rate range: 0–50 Hz; 0 means stopped.
- Synchronized mode follows the shared product transport/clock domain. The product must not introduce independent per-layer sequencer clocks; DEC-019 governs the shared transport.
- Phase is the start phase of the waveform.
- Amplitude quantization converts a continuous waveform into a stepped waveform; the manual documents continuous through two steps.
- Key sync modes are Off, On and Legato.
- Direct routing allows one destination and amount per LFO, independent of matrix-row allocation.
- LFO3/4 must not be assigned different product semantics merely because the reference device uses separate NRPN clock multipliers/dividers.

### 3.4 Sources

- MIDI sources use the relevant incoming channel/note context and the layer's MIDI channel/MPE assignment.
- Velocity, release velocity and per-note expression are associated with the triggering note/voice where applicable.
- Random1–4 produce independent deterministic values per note, with seedable per-instance random streams per GS-NFR-005.
- XY pad supplies two normalized axes (X and Y); the pad is a software performance control, not a hardware touchscreen dependency.
- Sequencer modulation lanes are sourced from the central R1 sequencer and follow its shared timing.
- Envelope follower can listen to the live input or a selectable layer signal. The source selection and follower controls are stored with the layer patch.

### 3.5 Destinations

- Only parameters that exist in R1 can be required R1 destinations.
- Every continuous R1 parameter in the engine, voice, envelope, filter, LFO, effect and macro domains must be considered for modulation.
- Discrete/enum parameters are excluded unless an explicit requirement defines safe stepped modulation.
- FFT-size controls, CV destinations, Draw X/Y, and parameters belonging only to deferred engines/effects are not R1 obligations.
- Host automation exposure is governed by the parameter exposure decision in Q-T12; this specification does not decide the exact exposed parameter count or ID scheme.

### 3.6 Macros

- Exactly 10 macros are committed for R1. The manual's NRPN table enumerates up to 20, but its prose and destination list document 10; the R1 requirement explicitly specifies 10.
- A macro can control multiple destinations. Each assignment has a start value, end value, curve and invert behavior.
- Macro assignments can target any eligible modulation destination.
- Macros are themselves modulation sources/destinations only to the extent already specified by GS-MOD-008 and GS-MOD-005.
- Macro values and assignments are saved with the patch.
- Copy/paste/delete operations are available in the macro editor.

### 3.7 Modulation visualization

- A modulated control displays the combined modulation contribution, not only the last row that changed it.
- A recent-destinations monitor displays the latest used destinations and their summed modulation values.
- The display must distinguish base value from effective value without requiring the user to infer which one is being shown.
- Visualization must not block the audio thread or alter modulation behavior.

## 4. Persistence and interaction

- LFO configuration, matrix rows, macro assignments and envelope-follower configuration are part of the layer Patch state.
- XY pad current position is a performance value; persistence/reset behavior remains a Phase 4 interaction detail.
- Copy/paste of a layer or patch copies its modulation configuration.
- Copy/paste of a matrix row preserves all row fields and destination reference.
- Undo/Redo applies to editing operations, consistent with the global R1 editing model.
- MIDI Learn and host automation use the same logical parameter identity as modulation destinations.

## 5. Edge cases and failure behavior

- A destination removed from a future version is handled through preset migration; unknown destinations must be ignored safely and reported to the user where practical.
- A source that is unavailable (e.g. no live input or no active note) contributes neutral output.
- Disabling a source or row must not produce a discontinuity; affected destinations follow the same smoothing/click-safety policy as other parameter changes.
- A modulation sum that exceeds the destination range is constrained by the destination's documented range.
- Random sources must be deterministic under a fixed seed and reproducible offline.
- Modulation matrix row count has a minimum capacity requirement of 50. No arbitrary maximum above that is specified at product level; safe technical limits are measured later.

## 6. Unresolved questions

| ID | Question | Status / handling |
|---|---|---|
| Q-M11 | Are LFOs per-voice or per-layer? | **ANSWERED — DEC-024:** per-LFO Poly/Mono switch; default Mono (shared per-layer phase), Poly uses independent per-voice phase. |
| MAC-07 | Is 20-macro support required? | R1 remains 10. Do not promote to 20 absent approval. |
| MAC-04 | Are macro modulation values per-voice or shared? | **ANSWERED — DEC-031:** base macro value is shared per layer patch; per-note sources may produce per-voice modulated destinations. DSP evaluation order Phase 3. |
| MTX-03 | Exact curve equation and taper | Needs a precise product definition; manual provides qualitative behavior only. |
| MTX-04 | XY pad coordinate and source normalization | **ANSWERED — DEC-032:** each axis normalized 0.0–1.0; saved patch recall position defaults to center (0.5, 0.5). UI details Phase 4. |
| Q-T12 | Exact host parameter exposure and stable IDs | Phase 3 decision; this document does not decide it. |
| MTX-05 | Exact R1 destination inventory | Reconcile against the final R1 parameter inventory; do not count deferred-only parameters. |

## 7. Benchmark-only / deferred capabilities

- Drawn XY trajectory modulator: L3 in current requirements.
- CV inputs: optional/later; not R1.
- Patch randomizer: L2.
- 20 macros: not an R1 commitment (DEC-036).
- Poly/Mono LFO scope: resolved by DEC-024; Mono default, Poly selectable per LFO.

## 8. Acceptance intent (criteria finalized in Phase 5)

A test suite must verify source output, row enable/disable, curve/polarity, multi-row summing, direct LFO routing, destination clamping, macro multi-assignment, deterministic random streams, envelope follower input selection, persistence, and safe behavior when sources disappear. Numerical tolerances and CPU budgets are Phase 5/architecture inputs.

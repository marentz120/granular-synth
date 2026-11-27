# Effects Specification

**Version:** 0.1 — Phase 2 working specification  
**Status:** R1 effect inventory approved by DEC-023; parameter-level details remain proposed/open where noted. No DSP implementation authorized.  
**Authority:** GRANULAR_SYNTH_REQUIREMENTS.md GS-FX-001…008; R1_PRODUCT_SPECIFICATION.md; DEC-015…025.  
**Benchmark evidence:** GR_MEGA_FEATURE_MAP.md §12 and §1.2.

## 1. R1 scope

Each of the four layers has an independent serial effects chain with four reorderable slots. R1 effect types are Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator and Compressor. Each slot has a dry/wet control. Audio input and synthesized layer signal can be sent to the layer chain. All R1 effect parameters are automatable and modulatable subject to the common parameter model. Preset switching supports the approved effect-tail choice: linger or cut.

Master-bus EQ/effects are L3. Large Reverb, Resonator, Flanger, Vibrato, Ring Modulator, Reducer and Distortion are L2. Duplicate instances of the same effect type in one layer are optional, not required for R1. FX chain and single-effect presets are L2.

## 2. Chain behavior

- Each layer owns an independent ordered list of four FX slots.
- Audio traverses enabled slots in order; reordering changes signal order.
- Each slot has a wet/dry mix. The dry path must remain available when wet is zero.
- Slot bypass, slot replacement and reordering must not create clicks; parameter transitions follow GS-NFR-003.
- The chain receives the layer's synthesized signal and the explicitly supported live-input send. Send gain is independent of per-slot wet/dry.
- Audio-input send and synth send have independent gain controls. The benchmark documents −100 dB to +24 dB; retain this as the benchmark-derived proposed range pending confirmation against product gain-staging requirements.
- FX state belongs to the layer Patch and is included in copy/paste and preset recall.
- On preset switch, tail behavior is selectable: Linger preserves existing tails; Cut silences/resets tails. The exact crossfade and reset implementation is Phase 3.

## 3. R1 effect inventory

The benchmark feature map provides the following controls. Ranges and defaults not explicitly documented in the source remain TBD rather than invented.

| Effect | Documented control set | Source |
|---|---|---|
| Delay | Length/time, feedback amount, dry, wet; independent stereo channels | FX-T03, manual p.83 |
| Ping-pong Delay | Length/time, feedback amount, dry, wet | FX-T04, manual p.83 |
| Reverb | Room size, damping, width, dry, wet; simple/low-CPU algorithm | FX-T08, manual p.83 |
| Chorus | Rate, feedback, centre, bandwidth, dry, wet | FX-T01, manual p.83 |
| EQ3 | Three-band shelf; low/mid/high gains; low/mid and mid/high crossover frequencies; dry/wet | FX-T06, manual p.83 |
| Saturator | Tanh-type saturation; drive, post gain, dry, wet | FX-T13, manual p.83 |
| Compressor | Threshold, knee, ratio, attack, release, dry, wet; VU metering | FX-T02, manual p.83 |

## 4. Per-effect behavior requirements

### 4.1 Delay and Ping-pong Delay
- Delay time/length and feedback/amount are independent parameters.
- Delay supports independent stereo channels; Ping-pong uses alternating stereo repeats.
- Dry/wet mix is available per slot.
- Time synchronization to the shared clock is not explicit in the benchmark's effect parameter description. Treat tempo-sync behavior as an unresolved product question, not as a committed feature.
- Feedback must remain within a stable operating range; exact range and saturation behavior require DSP design and tests.

### 4.2 Reverb
- R1 uses the simple, CPU-light reverb class identified in the feature map.
- Controls: room size, damping, width, dry and wet.
- Large Reverb is L2 and must not be conflated with the R1 Reverb.

### 4.3 Chorus
- Controls: rate, feedback, centre, bandwidth, dry and wet.
- Modulation of continuous controls is supported through the common modulation model.
- Exact units, ranges and default values are not documented in the available benchmark map and remain TBD.

### 4.4 EQ3
- Three-band shelf topology.
- Three gain controls and two crossover controls.
- The mapped manual indicates gain from silence to +6 dB and crossover spans of 50 Hz–5 kHz and 200 Hz–18 kHz. These are benchmark values, not yet approved GRANULAR SYNTH parameter ranges.
- Crossover ordering must remain valid; the exact constraint and UI response are to be defined before implementation.

### 4.5 Saturator
- Tanh-type saturation.
- Controls: drive, post gain, dry and wet.
- Gain staging and compensation behavior must be explicitly specified during DSP design; no automatic loudness compensation is implied by this document.

### 4.6 Compressor
- Controls: threshold, knee, ratio, attack, release, dry and wet.
- VU meters are documented by the benchmark. Whether R1 includes dedicated metering is a UI/product decision; do not infer a specific meter layout from the hardware.
- Detector topology, sidechain options, lookahead and gain-reduction display are not established by the current requirements and remain uncommitted.

## 5. Modulation and automation

- Every continuous parameter of every R1 effect is a candidate modulation destination and host automation parameter under GS-FX-004.
- Discrete controls are not modulatable unless explicitly specified.
- Base values remain independently editable; modulation affects effective values through the shared parameter model.
- Effect parameter IDs, host exposure and automation scaling remain Phase 3 decisions (Q-T12).
- Modulation and automation must not bypass smoothing or real-time safety requirements.

## 6. Presets, copy and state

- FX chain and slot state are stored with the layer Patch.
- Copy/paste of a layer or Patch copies the complete FX chain.
- Copy/paste of a slot preserves effect type, parameter values, wet/dry and slot state.
- FX chain presets and individual FX presets are L2, not R1.
- On preset recall, the user-selected tail policy applies: Linger or Cut.
- Persistence format and migration are governed by the preset/state specification, not defined here.

## 7. Edge cases

- Empty or disabled slot behaves as a transparent path.
- Reorder, bypass, preset recall and parameter changes must be click-safe.
- If an effect state is invalid or unknown in a preset, load the rest of the patch and report/skip the invalid effect rather than crashing.
- If live input is unavailable, its send contributes silence; the synth path remains operational.
- CPU overload handling and safe effect-count limits are measured in later phases. The product scope requires four slots; no unsupported claim about unlimited simultaneous effect processing is made.

## 8. Unresolved questions

| Question | Status |
|---|---|
| Delay time units, min/max, tempo divisions and sync behavior | OPEN; benchmark map does not establish all semantics |
| Exact ranges, units and defaults for Chorus, Compressor, Saturator and Reverb controls | OPEN; do not invent |
| EQ3 crossover ordering and invalid-value behavior | OPEN |
| Compressor detector topology, sidechain, lookahead and meter scope | Not specified; not R1 commitments |
| FX send range −100…+24 dB | Benchmark-derived candidate range; validate against product gain staging in Phase 3 |
| Duplicate same-type effect instances | OPT; no R1 requirement |
| FX chain/single-effect presets | L2 |
| Master-bus FX | L3 |

## 9. Deferred effects

Large Reverb, Resonator (MBR), Flanger, Vibrato, Ring Modulator (including AM mode), Reducer and Distortion are L2. Master bus EQ/FX is L3. Do not introduce these as R1 dependencies.

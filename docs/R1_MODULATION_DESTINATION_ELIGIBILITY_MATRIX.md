# R1 Modulation Destination Eligibility Matrix

**Version:** 0.1 — candidate inventory and scope audit (2026-11-28)  
**Status:** Working inventory; not a complete canonical parameter registry and not an implementation authorization.  
**Authority:** GS-MOD-005, GS-MOD-004, DEC-024, DEC-031, DEC-036, DEC-052…055; `MODULATION_SPECIFICATION.md`; `GRANULAR_SYNTH_REQUIREMENTS.md`; `PARAMETER_ID_SPECIFICATION.md`.  
**Related gate:** MTX-05 in `PHASE5_DECISION_CLOSURE_REGISTER.md`; Q-T12 parameter identity/registry approval.

## 1. Purpose and interpretation

GS-MOD-005 requires modulation destinations to cover every eligible continuous parameter belonging to an R1 engine, envelope, filter, LFO, FX or macro. This document translates that rule into a domain-level eligibility inventory so deferred features are not accidentally included and continuous controls are not overlooked.

This is a **candidate inventory**, not the final enumerated parameter list. The repository does not yet contain an approved, exhaustive R1 parameter registry: `PARAMETER_ID_SPECIFICATION.md` remains a proposal under Q-T12, and the exact set of parameter descriptors, IDs, ranges, units and modulation flags is not frozen. No numeric destination count is claimed.

## 2. Eligibility rules

A parameter is a candidate R1 modulation destination when all of the following apply:

1. It belongs to a feature included in R1.
2. It represents a continuous value, rather than a mode, action, topology selection or structural resource change.
3. Its value can be changed safely through the common parameter model without violating voice, sample, scheduler, host or real-time lifecycle invariants.
4. Its scope is defined (global, layer, voice, performance or sequencer) and its behavior is meaningful for the source context.

A candidate is **not automatically approved** merely because a UI control exists. The canonical descriptor must explicitly state `modulatable=true`; a parameter may be automatable but not modulatable, or vice versa.

For per-note sources (velocity, release velocity, poly aftertouch, MPE pressure/timbre, per-note random), the destination must support the relevant voice scope. Shared/per-layer destinations use the layer/source aggregation contract; they must not be given accidental per-voice semantics.

## 3. Candidate destination domains

| Domain | Candidate continuous parameter families | Explicitly not destinations by default | Scope / notes |
|---|---|---|---|
| Granular engine | Grain rate; grain size; grain position/scan position; spray amount; pan; pan-spray amount; mid-side width; direction probability; scan amount; pitch; pitch-spray amount; continuous ratio-mode control values; window shape controls (PowAR sides/tilt/curve; Raised-Cosine tilt/curve/AM) | Engine selection; ratio-mode selector; scan mode; snap/fine mode; spawn mode; key-trigger mode; clock division; Voice Sync toggle; local-pitch toggle; grain-stealing enable/mode; normalization ON/OFF; interpolation-quality selector; window-family selector | Usually layer or voice, with Spawn-class parameters sampled at grain birth where specified. Exact scope must be assigned per descriptor. |
| Sampler engine | Playback scan/position; spray amount; pan; pan-spray amount; continuous pitch/transpose offsets where exposed as sampler controls | Engine selection; one-shot/loop/ping-pong mode; loop-release toggle; loop/crop boundary edits; sample selection/replacement; Env Pile-up toggle; interpolation mode selector | Per-voice playback controls must preserve active sample/loop bounds. Do not inherit granular-only parameters unless explicitly active in Sampler. |
| Voice / pitch | Glide time; velocity sensitivity; transpose semitone/cents values where exposed as continuous controls; continuous fine-tune values | Voice allocation actions; sustain CC64 state; Always-glide mode selector; discrete note/chord/scale selections; pitch-bend range configuration unless the descriptor explicitly allows modulation | Per-voice or layer scope according to the shared voice contract. MIDI event values are sources, not automatically destinations. |
| Envelopes | For each of Pitch, Filter, Amp and Aux envelopes: Attack, Decay, Sustain level, Release; continuous stage-curve controls if represented as continuous values; bipolar amount | Envelope loop mode (L2); envelope assignment/routing topology; trigger/retrigger actions; discrete curve-family selectors | Per-voice envelope parameters. DEC-051 time range applies to time-valued stages; Sustain is a level. |
| Filters | LPF cutoff and resonance; HPF cutoff and resonance; continuous key-tracking amount if exposed | Filter chain bypass; LPF/HPF routing mode; 6/12/18/24 dB slope selector; filter type/topology; discrete response modes | Per-voice. R1 has LPF and HPF only under DEC-052. Filter topology and resonance response remain technical gates. |
| LFOs | For each of four LFOs per layer: rate, phase, amplitude/depth, and amplitude-quantization amount if represented continuously | Waveform; polarity mode; key-sync mode; Poly/Mono mode; tempo division; discrete quantization-step count | LFO phase mode follows DEC-024. Rate/phase/depth candidates require explicit scope and smoothing metadata. |
| Macros | Ten macro base values per Patch; continuous start/end mapping values and assignment amounts where exposed as editable parameters | Add/delete/reorder assignment actions; discrete curve selector; invert toggle | Macro base value is shared per layer Patch under DEC-031; per-note sources may create per-voice modulation of eligible destinations. Macros may themselves be destinations under GS-MOD-005/008. |
| FX (final R1 milestone M14) | Slot wet/dry; synth-send and audio-input-send gain; continuous parameters for Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator and Compressor | Slot order/type/replacement; bypass; discrete delay-sync division; effect topology/mode selectors | R1 FX remains in scope but implementation and FX parameter closure are deferred to M14 by DEC-056. Do not block core milestones on FX descriptors; complete this domain before M14. |
| Layer / performance / sequencer | Candidate only: layer output level/pan and other continuous R1 performance parameters if GS requirements explicitly establish them as parameterized controls | Pattern/step structure, note events, transport actions, mute/solo actions, preset selection and other structural/event commands | GS-MOD-005 does not explicitly name these domains. Include only if the canonical R1 requirements and approved parameter model classify a control as an eligible continuous engine/layer/performance parameter; do not silently expand scope. |

## 4. Sources and scope compatibility

R1 source inventory is GS-MOD-004: LFO1–4, Mod Wheel, Key Track, Velocity, Pitch Bend, Channel/Poly Aftertouch, MPE Timbre, Release Velocity, Aux Envelope, Random1–4, XY performance pad, sequencer modulation lanes and input/layer envelope follower.

The destination registry must carry enough scope metadata to validate source/destination compatibility:

- **Layer-shared sources:** Mod Wheel, shared LFOs (Mono mode), XY axes, layer/input envelope follower and shared macro values.
- **Per-note / per-voice sources:** Velocity, Release Velocity, Key Track, Pitch Bend and MPE expression, plus per-note Random1–4 and Poly-mode LFOs.
- **Sequencer sources:** central sequencer modulation lanes; timing follows the single shared transport (DEC-019).
- **Unavailable sources:** contribute neutral modulation and do not reset or corrupt the destination.

This section defines compatibility categories only. Exact source normalization, source evaluation order, control rate and host/event precedence remain governed by their respective specifications and technical gates.

## 5. Explicit R1 exclusions

The following are excluded from the R1 destination inventory:

- Granular Slice, Tape and Spectral engine parameters.
- Spectrogram controls and FFT-size controls.
- Draw X/Y trajectory modulator controls and CV destinations.
- Patch randomizer controls.
- L2 granular probability/filter/anti-aliasing features.
- L2 sampler streaming, loop crossfade and zero-crossing controls.
- L2 envelope loop modes.
- L2/L3/OPT effects and master-bus FX.
- Structural operations, file/sample selection, engine/effect type selection, discrete modes and actions, unless an explicit R1 requirement defines safe stepped modulation.

The seven approved R1 FX types are not excluded; they are simply delivered at M14 under DEC-056.

## 6. Registry reconciliation checklist

The final MTX-05 closure requires a generated, row-by-row reconciliation against the canonical R1 parameter registry:

- [ ] Every R1 continuous parameter descriptor is reviewed for modulation eligibility.
- [ ] Every eligible descriptor has `modulatable=true` and a defined scope.
- [ ] Every excluded descriptor has a reason (discrete, structural, deferred, unsafe, or not in GS-MOD-005 domain).
- [ ] Per-voice vs per-layer scope is reconciled with DEC-024 and DEC-031.
- [ ] Spawn-class destinations are identified and sampled at the correct lifecycle boundary.
- [ ] FX destinations are included in the M14 closure pass, not accidentally treated as delivered before then.
- [ ] No benchmark-only destination is added solely to reach the GR-MEGA count of 149.
- [ ] Registry completeness, uniqueness, range/curve metadata and descriptor flags pass the Q-T12 review.
- [ ] Destination-list acceptance evidence proves that all eligible descriptors are selectable and that excluded descriptors are not selectable.

## 7. Open items / limits

1. **No exact destination count:** the GR-MEGA count of 149 is benchmark evidence only. R1 count must be generated from the approved GRANULAR SYNTH registry; do not force parity.
2. **Q-T12 remains open:** the proposed ID grammar, numeric host IDs, metadata schema and exposure policy are not approved by this document.
3. **Technical safety remains open:** control-rate evaluation, smoothing classes, parameter precedence and per-voice aggregation require the relevant engineering decisions.
4. **FX closure is staged:** the FX domain must be reconciled before M14; this inventory does not reopen or accelerate FX implementation.
5. **No implementation or tests:** this document is a planning/audit artifact only.

## 8. Closure recommendation

Treat this document as the domain-level eligibility baseline for MTX-05. Close MTX-05 only after Q-T12 produces the approved canonical parameter registry and an automated reconciliation shows 100% coverage of eligible R1 continuous parameters, with all exclusions justified. Until then, MTX-05 remains **partially prepared, not closed**.

# Advanced Granular Control Reachability — R1

**Version:** 0.1 — Phase 4 UI reachability proposal (2026-11-28)  
**Status:** Proposal for owner review. Does not approve parameter IDs, host exposure, DSP behavior, numeric ranges/defaults or implementation.  
**Authority:** GS-GRN-005…020, GS-UI-002/007, GS-MOD-003/005, GS-MID-003/008; DEC-043…055; UI_UX_SPECIFICATION.md; PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md; PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md; R1_ENGINE_PARAMETER_CANDIDATE_INVENTORY.md.

## 1. Purpose and rules

This appendix turns the R1 Granular candidate list into a named UI reachability proposal. It distinguishes:

- **Touch:** a named control in the Granular workspace or a named contextual panel.
- **Encoder:** available through the four-encoder contextual focus system. A–H assignments remain exactly those in `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md §2.5`; this document does not reassign them.
- **MIDI Learn:** the control may be a selectable mapping target if it is an automatable parameter under GS-MID-003. That is a candidate rule, not a claim that the final registry or host automation flag exists.
- **Modulation:** only eligible continuous R1 parameters are candidates under GS-MOD-005. Discrete modes and structural actions are not continuous destinations.
- **Controller-only performance:** GS-MID-008 applies to essential live-performance operations, not every detailed sound-design parameter or one-time setup task. A parameter being MIDI Learn-capable does not make it an essential live operation.

All controls remain scoped to the layer or voice defined by the approved product semantics. A selected-layer UI focus must never silently retarget an existing mapping.

## 2. Proposed reachability matrix

| R1 control / family | Requirement | Proposed touch surface | Encoder reachability | MIDI Learn disposition | Modulation disposition | Notes / unresolved |
|---|---|---|---|---|---|---|
| Grain Size | GS-GRN-002 | Granular primary control | A–H slot A | Candidate parameter target | Continuous candidate | Exact range/default already partly specified in inventory; descriptor scope needs Q-T12 |
| Grain Rate | GS-GRN-001 | Granular advanced / density context | A–H slot B | Candidate parameter target | Continuous candidate | Rate is independent control; Density is derived |
| Density | GS-GRN-003 | Persistent read-only display beside Size/Rate | Display only; not an independent encoder destination | Not a writable MIDI target | Exclude as independent destination | Display Rate × Size; never imply editability |
| Position | GS-GRN-004 | Waveform/source-region control | A–H slot C | Candidate parameter target | Continuous candidate | Exact normalized/sample-relative range remains TBD |
| Pitch | GS-GRN-010 | Granular primary control | A–H slot D | Candidate parameter target | Continuous candidate subject to descriptor scope | Coarse/fine semantics in requirement; exact host scaling open |
| Scan | GS-GRN-009 | Advanced playback/scan panel | A–H slot E | Candidate parameter target | Continuous candidate | Near-zero behavior DEC-043; scan mode and snap are separate discrete controls |
| Spray Amount | GS-GRN-005 | Advanced Grain Distribution panel | A–H slot F | Candidate parameter target | Continuous candidate | Polarity is a separate enum; sample-relative mapping needs descriptor definition |
| Spray Polarity | GS-GRN-005 | Adjacent selector in Grain Distribution | Contextual focus after Spray | Selectable mapping only if product/registry treats it as automatable; not a continuous destination | Exclude from continuous modulation | Bipolar, +Unipolar, −Unipolar |
| Pan | GS-GRN-006 | Advanced Spatial panel | A–H slot G | Candidate parameter target | Continuous candidate | Base-pan range and scope require reconciliation |
| Pan Spray Amount | GS-GRN-006; DEC-053 | Advanced Spatial panel | A–H slot H | Candidate parameter target | Continuous candidate | Each grain samples at spawn and retains pan for lifetime |
| Pan Spray Distribution | GS-GRN-006; DEC-053 | Adjacent selector in Advanced Spatial | Contextual focus | Selectable mapping only if approved as automatable | Exclude from continuous modulation | Uniform / Centre Weighted |
| Mid-Side Width | GS-GRN-007 | Advanced Spatial panel | Contextual encoder page | Candidate parameter target | Continuous candidate | Per-layer control; range/unit TBD |
| Direction Probability | GS-GRN-008 | Advanced Playback panel, labelled probability | Contextual encoder page | Candidate parameter target | Continuous candidate | Avoid ambiguous “Reverse” toggle; show probability direction |
| Scan Fine/Snap | GS-GRN-009 | Scan mode selector | Contextual focus | Discrete target only if approved for Learn | Exclude from continuous modulation | Fine / ±0.25 snap; mapping semantics remain TBD |
| Pitch Spray Amount | GS-GRN-010 | Advanced Pitch panel | Contextual encoder page | Candidate parameter target | Continuous candidate | Range and per-grain/per-voice scope TBD |
| Pitch Spray Polarity | GS-GRN-010 | Adjacent selector in Advanced Pitch | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | Exact polarity values need reconciliation with requirement/spec |
| Pitch Snap | GS-GRN-010; GS-CHD-005 | Advanced Pitch / Quantize panel | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | Semitone/chord/scale behavior; scale list and scope not redefined here |
| Ratio Mode | GS-GRN-011 | Advanced Timing / Ratio panel | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | Free, DensitySize, DensityRate, ScanRate, ScanOverlap, TimeStretch |
| Ratio inputs | GS-GRN-011 | Controls shown conditionally under selected Ratio Mode | Contextual encoder page | Candidate target only for continuous inputs | Continuous candidates after mode-specific eligibility review | No formulas, ranges or values invented; hide/disable irrelevant inputs with explanation |
| PowAR Window Sides | GS-GRN-012/013 | Grain Window panel, PowAR tab | Contextual encoder page | Candidate target if continuous/automatable after type is settled | Candidate only if continuous descriptor | Type and numeric range TBD |
| PowAR Tilt | GS-GRN-012/013 | Grain Window panel, PowAR tab | Contextual encoder page | Candidate parameter target | Continuous candidate | Existing-grain updates must remain click-free |
| PowAR Curve | GS-GRN-012/013 | Grain Window panel, PowAR tab | Contextual encoder page | Candidate parameter target | Continuous candidate | Existing-grain updates must remain click-free |
| Raised-Cosine Tilt | GS-GRN-012/013 | Grain Window panel, Raised Cosine tab | Contextual encoder page | Candidate parameter target | Continuous candidate | Existing-grain updates must remain click-free |
| Raised-Cosine Curve | GS-GRN-012/013 | Grain Window panel, Raised Cosine tab | Contextual encoder page | Candidate parameter target | Continuous candidate | Existing-grain updates must remain click-free |
| Raised-Cosine AM | GS-GRN-012/013 | Grain Window panel, Raised Cosine tab | Contextual encoder page | Candidate parameter target | Continuous candidate | Existing-grain updates must remain click-free |
| Grain Key Trigger | GS-GRN-014; DEC-048 | Advanced Trigger panel | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | Off / On / Legato; Note Off and independent grain clock remain distinct |
| Grain Clock Division | GS-GRN-014 | Advanced Timing panel | Contextual focus | Discrete/stepped target only if approved | Exclude from continuous modulation | Straight/triplet/dotted, 1/64…64/1; transport arbitration remains technical |
| Voice Sync | GS-GRN-014; DEC-050/055 | Advanced Timing panel | Contextual focus | Boolean target candidate if approved | Exclude from continuous modulation | Changes apply at next valid scheduler boundary |
| Spawn Mode | GS-GRN-015; DEC-049 | Advanced Trigger / Cloud Initialization panel | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | Gradual, Direct, Direct Reset, Recycle |
| Local Pitch | GS-GRN-016 | Advanced Pitch panel | Contextual focus | Boolean target candidate if approved | Exclude from continuous modulation | Grain retains birth pitch when enabled |
| Grain Stealing Threshold | GS-GRN-017 | Advanced Voice/Grain Management panel | Contextual encoder page | Candidate parameter target if continuous/automatable | Candidate only after type/scope confirmed | 0 means off; maximum not numerically defined |
| Grain-Count Normalization | GS-GRN-018; DEC-046 | Advanced Level / Grain Management panel | Contextual focus | Boolean target candidate if approved | Exclude from continuous modulation | 1/√N product behavior approved; count domain/smoothing technical |
| Granular Interpolation Quality | GS-GRN-020 | Engine Quality section in Advanced / Settings panel | Contextual focus | Enum target only if approved | Exclude from continuous modulation | Candidate UI placement only; default and DSP validation remain Q-T14 |

## 3. UI hierarchy proposal

1. **Primary workspace:** preserve the owner-confirmed five visible elements: Size, Density (read-only), Position, Pitch and Spread. Do not displace them.
2. **Spread alias gate:** “Spread” remains an unresolved display label. It must not be bound to Spray, Pitch Spray or Pan Spray until the product owner confirms its meaning. The three distinct controls above remain separately named in the advanced panel.
3. **Advanced panels:** group controls into Distribution, Spatial, Pitch/Quantize, Timing/Ratio, Grain Window, Trigger/Cloud, Grain Management and Engine Quality. These are proposed panel names, not new product subsystems.
4. **Contextual encoder pages:** every editable parameter above must be focusable by touch and reachable by deterministic encoder focus navigation. Existing A–H slots remain unchanged. No control may be silently substituted when inapplicable.
5. **MIDI Learn target browser:** show human-readable name, engine, layer/voice scope, control type and current mapping state. Do not display invented stable IDs or claim host-automation eligibility.
6. **Modulation destination browser:** list only approved eligible continuous descriptors after Q-T12; do not expose enum/boolean selectors as continuous destinations.

## 4. Controller reachability classification

- **Essential live operations:** transport, Freeze, Fill, Mute, Pattern operations, Panic and other operations enumerated in LIVE_PERFORMANCE_SPECIFICATION.md remain controller-only reachable under GS-MID-008.
- **Sound-design parameters:** MIDI Learn may be offered for approved automatable parameters under GS-MID-003; this does not mean every parameter must be assigned to a physical controller by default.
- **Discrete selectors:** may be mappable only after the action/value model and registry semantics are defined; do not represent them as continuous modulation targets.
- **Setup-only tasks:** sample loading and detailed initial patch configuration are outside the controller-only live-operation guarantee.

## 5. Open items / gates

1. Owner confirmation of the meaning of the persistent “Spread” control.
2. Final parameter types and ranges for Ratio inputs, window sides, pitch spray polarity, grain stealing threshold and other TBD rows.
3. Q-T12 canonical descriptor IDs, scopes, update classes, automation flags and state migration.
4. Exact MIDI Learn eligibility and action/value representation for enum/boolean controls.
5. MTX-05 descriptor-level modulation reconciliation.
6. Phase 5 acceptance criteria for touch, encoder and MIDI mapping behavior.

**Boundary:** UI reachability proposal only. No parameter ID, host numeric ID, DSP algorithm, MIDI CC layout, automation exposure, implementation, test or release decision is made here.

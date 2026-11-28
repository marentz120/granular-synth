# R1 Engine Parameter Coverage Audit — Granular, Sampler and Shared Voice

**Version:** 0.1 — documentation audit (2026-11-28)  
**Status:** Review findings and traceability only. No owner sign-off, Q-T12 approval, implementation authorization or acceptance closure.  
**Compared artifacts:** `R1_ENGINE_PARAMETER_CANDIDATE_INVENTORY.md`, `GRANULAR_SYNTH_REQUIREMENTS.md`, `UI_UX_SPECIFICATION.md`, `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md`, `PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md`, `MIDI_MAPPING_UX_SPECIFICATION.md`, `MODULATION_SPECIFICATION.md`, `R1_MODULATION_DESTINATION_ELIGIBILITY_MATRIX.md`, `PARAMETER_ID_SPECIFICATION.md`, `PHASE5_DECISION_CLOSURE_REGISTER.md`.

## 1. Executive result

The candidate inventory is broadly consistent with the R1 product requirements and the Phase 4 UI proposal, but it is **not yet a complete or internally normalized parameter catalogue**.

- The Granular and Sampler A–H encoder bank has explicit eight-slot proposals for both engines and is traceable to R1 requirements.
- Density is correctly treated as a derived display value (Rate × Size), not as an independent writable parameter or encoder destination.
- The five always-visible Granular controls are documented, but the UI label **Spread** is not mapped unambiguously to a canonical candidate parameter.
- Advanced engine controls are described as contextual/secondary controls, but many are not individually listed as named UI or controller targets.
- Shared voice controls have workspace-level placement but lack a complete target-by-target reachability map.
- Sampler interpolation quality is required in R1, but its user-facing selection/presentation is not explicitly placed in the UI specification.
- MIDI Learn is described as able to target a selected control/parameter, but the controller target catalogue does not enumerate every candidate engine/shared-voice parameter.
- Host automation exposure remains a Q-T12 proposal, not an approved or audited exposure list.
- No canonical parameter IDs, host IDs, defaults, complete ranges, smoothing classes or final modulatable/automatable flags can be approved from these documents.

**Disposition:** candidate inventory retained; Phase 4/5 reconciliation gaps recorded. Q-T12 and MTX-05 remain open.

## 2. Coverage matrix

| Parameter family | Requirements / candidate inventory | Touch UI | Encoder A–H / contextual encoders | MIDI Learn / controller target catalogue | Modulation eligibility | Audit result |
|---|---|---|---|---|---|---|
| Grain Size, Rate | GS-GRN-001/002 | Granular workspace; Size primary | A Size, B Rate | Generic core-engine targets; not individually enumerated in full mapping list | Candidate continuous destinations | Strong coverage; IDs/ranges/defaults still open |
| Density | GS-GRN-003 derived Rate × Size | Always-visible primary display | Explicitly not a separate A–H target | Must not be offered as an independent writable target unless a later decision changes its derived status | Exclude as independent destination by default | Correctly handled as derived/read-only; ensure UI does not imply editability |
| Position, Scan, Pitch | GS-GRN-004/009/010 | Position/Pitch primary; Scan contextual | C Position, D Pitch, E Scan | Scan/Position and Pitch named in performance catalogue | Candidate continuous destinations | Strong mapping; clarify Position vs Scan distinction and scopes |
| Spray, Pitch Spray, Pan Spray, Pan | GS-GRN-005/006/010 | Spread primary label; advanced Spray/Pan Spray in contextual controls | F Spray, G Pan, H Pan Spray | Named partially; catalogue says Granular Spray/Pan Spray | Candidate continuous destinations; distribution selector excluded | Partial: Spread label ambiguity; Pitch Spray is not individually surfaced in target catalogue |
| Direction probability | GS-GRN-008 | Advanced/contextual controls only by implication | Not assigned A–H | Direction target named, but target class/range/behavior unclear | Candidate continuous destination | Partial: define whether UI target is probability control and its display semantics |
| Mid-Side Width | GS-GRN-007 | No explicit named control in current UI excerpts | Not assigned A–H | Not individually named | Candidate continuous layer destination | Gap: add explicit touch/context and MIDI target placement |
| Ratio modes and inputs | GS-GRN-011 | Not individually named | Not assigned A–H | Not individually named | Mode selector excluded; continuous mode inputs candidate | Gap: define contextual presentation and distinguish mode selector from continuous inputs |
| Window families/shape controls | GS-GRN-012/013 | Grain shape/window is mentioned as contextual/secondary | Not assigned A–H | Not individually named | Continuous shape values candidate; family selector excluded | Partial: family and each shape control need named UI/target coverage |
| Grain trigger/clock/Voice Sync | GS-GRN-014; DEC-048/050/055 | Not individually named in UI surface inventory | Not assigned A–H | Not individually named | Enum/boolean controls excluded from continuous destinations | Gap: add named controls to advanced Granular panel and controller policy where appropriate |
| Spawn modes, Local Pitch, Grain Stealing, Normalization | GS-GRN-015…018; DEC-046/049 | Not individually named | Not assigned A–H | Not individually named | Modes/toggles excluded from continuous destination list | Gap: advanced panel and mapping reachability need explicit treatment |
| Granular interpolation | GS-GRN-020 | Not explicitly named | Not assigned A–H | Not individually named | Enum selector excluded | Gap: R1 quality choice needs a visible setting and state indication |
| Sampler Scan, Pitch/Rate | GS-SMP-001 | Dedicated Sampler workspace | A Scan, B Pitch/Playback Rate | Named in performance catalogue | Candidate continuous destinations | Strong coverage; parameter units/ranges and lifecycle open |
| Sampler Sample Start/Stop, Loop Start/End | GS-SMP-002 | Waveform/flag editor | C Sample Start, D Sample Stop; Loop Start/End in waveform editor | Sample/loop boundary controls named broadly | Structural/boundary classification unresolved; do not assume continuous modulation | Partial: define editor affordance and parameter-vs-asset-state treatment |
| Sampler Spray, Pan Spray, Direction | GS-SMP-001 | Dedicated Sampler workspace implied; detailed controls not enumerated | E Spray, F Pan Spray, G Direction | Named in performance catalogue | Continuous candidates; distribution enum excluded | Strong A–H coverage, but explicit touch control labels and shared-control identity need reconciliation |
| Sampler Playback Mode, Loop Release | GS-SMP-002; DEC-030 | Loop/One-shot/Bounce and Loop Release ON/OFF explicitly shown | H Loop Mode | Loop mode is an A–H selection target; loop release not individually named | Enum/boolean, not continuous destinations | Partial: Loop Release target should be explicit in mapping catalogue |
| Sampler Env Pile-up | GS-SMP-004; DEC-037 | Env Pile-up toggle explicitly shown, default OFF | Not assigned A–H | Not individually named | Boolean excluded from continuous destination list | UI covered; MIDI target reachability not explicit |
| Sampler Interpolation | GS-SMP-003 | No explicit control placement | Not assigned A–H | Not individually named | Enum selector excluded | Gap: mandatory high-quality option needs user-facing selection/presentation |
| Shared voice: Glide, Velocity, Pitch Bend, Transpose | GS-VOI-003…005 | Voice/MIDI contextual panels | No complete parameter-specific encoder map | MIDI mapping supports selected control/parameter generically; targets not enumerated | Continuous candidates where scope supports | Partial: explicit control and per-layer/per-voice target scope needed |
| Four ADSR envelopes, curves, amount, invert | GS-VOI-006; DEC-051 | Modulation workspace; envelope encoders Attack/Decay/Sustain/Release | Contextual encoder defaults exist | Not all four envelopes/stages enumerated as named targets | Continuous stage/amount candidates; curve/invert discrete TBD | Partial: envelope identity and target naming across Pitch/Filter/Amp/Aux must be explicit |
| LPF/HPF, resonance, slope, routing, bypass, key tracking | GS-VOI-008/009; DEC-052 | Voice/Envelope/Filter contextual panels | Not fully enumerated | Not individually enumerated | Cutoff/resonance/key-track candidates; slope/routing/bypass discrete | Partial: full control list, filter identity and target scopes need a trace |
| Sub-oscillator controls | GS-VOI-010; DEC-054 | Voice/Envelope/Filter contextual panels | Not fully enumerated | Not individually enumerated | Level/balance continuous candidates; frequency/quantize discrete | Partial: fixed sine and the remaining controls need explicit UI/target entries |

## 3. Findings requiring correction or explicit disposition

### F-01 — “Spread” is not a canonical parameter name yet

The UI specification makes Spread one of five permanently visible Granular controls. The candidate inventory contains Spray Amount, Pitch Spray Amount, Pan Spray Amount and Position, but no parameter explicitly named Spread. The encoder proposal separately uses Spray and Pan Spray.

**Recommendation:** retain the visible “Spread” label only if the product owner confirms exactly which existing parameter it represents. Do not create a new Spread parameter or map it to Spray/Pitch Spray/Pan Spray by assumption. Until clarified, mark the UI label as an unresolved display alias and do not create a separate ID or modulation destination.

### F-02 — Derived Density must remain non-writable

GS-GRN-003 defines Density as Rate × Size. The encoder proposal correctly states that Density is displayed and is not a second independent A–H target.

**Recommendation:** keep Density read-only/derived in the UI, MIDI Learn, host automation and modulation destination lists unless a separate approved product decision explicitly changes its semantics.

### F-03 — Advanced Granular controls are not fully traceable to UI/controller targets

The A–H bank covers Size, Rate, Position, Pitch, Scan, Spray, Pan and Pan Spray. It does not enumerate the remaining R1 controls: Mid-Side Width, Direction Probability, Pitch Spray and polarity/snap behavior, Ratio Mode and its continuous inputs, window-family and shape controls, Grain Key Trigger, Grain Clock Division, Voice Sync, Spawn Mode, Local Pitch, Grain Stealing Threshold and Grain-Count Normalization, and Interpolation Quality.

The UI says advanced controls are available through contextual/secondary controls, but this is not a complete per-control reachability specification. The MIDI target catalogue also does not enumerate each one.

**Recommendation:** add a named advanced Granular control inventory with a surface/context for every R1 parameter. For each row, state whether it is touch-editable, encoder-focusable, MIDI Learn eligible, host-automatable candidate, modulatable candidate, or deliberately excluded. Do not assign IDs or numeric mapping values yet.

### F-04 — Sampler boundary controls mix parameter and sample-state semantics

The A–H proposal places Sample Start and Sample Stop in encoder slots C/D, while Loop Start and Loop End are directly editable in the waveform/flag editor. The candidate inventory currently labels these “continuous or structural boundary”.

**Recommendation:** preserve the existing product workflow, but classify them provisionally as sample-region boundary state, not as ordinary continuous modulation targets. Q-T12/Phase 3 must decide whether these are parameter descriptors, sample-asset metadata, or a split model. No automation/modulation exposure should be assumed.

### F-05 — Sampler interpolation has no explicit UI location

GS-SMP-003 requires a high-quality polyphase/windowed-sinc option. The Sampler UI describes playback/loop behavior and Env Pile-up, but does not specify where the interpolation quality option is selected or how the active mode is displayed.

**Recommendation:** add an explicit Sampler quality/settings control in the Sampler contextual panel. The exact modes, default, filter length and transition behavior remain technical gates; this is a UI reachability recommendation, not a DSP choice.

### F-06 — Shared voice controls lack parameter-level controller traceability

The UI gives Voice/Envelope/Filter contextual panels and a generic MIDI Learn workflow. It does not enumerate all shared voice controls as controller targets, especially the four separate envelope identities, LPF/HPF sub-controls, key tracking, sub-oscillator controls, glide, velocity curve and pitch-bend/transpose.

**Recommendation:** create an explicit shared-voice target catalogue with engine applicability and scope. Keep per-voice DSP destinations distinct from per-layer patch controls; active-layer focus must not silently retarget an existing mapping.

### F-07 — MIDI Learn generic workflow is not proof of complete target coverage

The mapping UX permits selection of a target control/parameter and the controller catalogue names major performance controls. Neither document proves every R1 parameter has a discoverable, correctly scoped MIDI Learn target.

**Recommendation:** build a generated target coverage table from the eventual canonical registry. Each descriptor must resolve to one of: MIDI Learn target, deliberately unmappable with reason, or structural action/event target. This is blocked on Q-T12 and the action/event model.

### F-08 — Host automation exposure remains a proposal

PARAMETER_ID_SPECIFICATION.md proposes exposing all explicitly automatable R1 parameters. There is no approved host exposure list, numeric host ID manifest, or descriptor-level automatable flag.

**Recommendation:** do not describe any candidate as host-automatable today. After Q-T12 approval, produce a separate host exposure matrix covering Standalone, AU and VST3, with explicit exclusions and migration implications.

### F-09 — Modulation eligibility needs descriptor-level reconciliation

The domain-level modulation matrix lists broad candidate families, but the current engine inventory includes unresolved scopes and continuous/stepped ambiguities. Shared voice and Sampler controls need row-level reconciliation, and FX remains staged for M14.

**Recommendation:** retain MTX-05 as partially prepared. Close it only after Q-T12 registry approval, 100% descriptor reconciliation and explicit reasons for exclusions. Do not use the benchmark count of 149 as a target.

### F-10 — Cross-engine shared identity is unresolved

The product uses related concepts across Granular and Sampler (Scan, Pitch, Spray, Pan Spray, Direction and sample boundaries), but semantic equivalence and lifecycle behavior are not established for every control.

**Recommendation:** create one canonical descriptor only where the parameter has genuinely identical meaning, units, range, scope and update semantics. Otherwise use engine-specific descriptors with explicit applicability. Do not merge controls solely because labels match.

## 4. Required follow-up artifacts

| Artifact | Required content | Gate |
|---|---|---|
| Advanced Granular UI reachability table | Every R1 Granular candidate mapped to a named touch/context surface and controller target or justified exclusion | Phase 4 owner review |
| Shared Voice target catalogue | Glide, velocity, pitch, four envelopes, LPF/HPF, key tracking and sub-osc controls with explicit scope | Phase 4 owner review + Q-T12 |
| Sampler settings/reachability update | Interpolation selection/presentation and explicit Loop Release / Env Pile-up MIDI target disposition | Phase 4 owner review; interpolation details remain technical |
| Parameter descriptor registry | Stable IDs, type, unit, range, default, scope, engine applicability, update class, automation/modulation/MIDI flags and state version | Q-T12 approval |
| Cross-surface coverage report | Registry-to-touch, encoder, MIDI Learn, modulation, host automation and persistence reconciliation | After registry exists |
| FX descriptor reconciliation | Seven R1 effects and all eligible parameters | M14 under DEC-056 |

## 5. Explicit non-findings

- No evidence here justifies changing approved R1 scope.
- No evidence justifies adding Granular Slice, Tape, Spectral, Sampler streaming, loop crossfade or other deferred controls.
- No exact parameter count is asserted.
- No parameter IDs, host numeric IDs, DSP algorithms, control rates, smoothing constants, interpolation defaults or resource budgets are approved.
- No acceptance tests, host tests, benchmarks or DSP implementation were executed.
- This audit does not close Q-T12 or MTX-05.

## 6. Recommended next step

The highest-value next action is to prepare two UI-level appendices: **Advanced Granular Control Reachability** and **Shared Voice Control Reachability**, using only existing R1 requirements. Then revise the Sampler workspace proposal to place interpolation quality and explicit MIDI Learn targets. Keep all assignments as proposals for owner review; stable IDs and host exposure remain blocked on Q-T12.

**Boundary:** documentation-only audit. No production code, tests, benchmarks, host prototypes, PR merge or release authorized.

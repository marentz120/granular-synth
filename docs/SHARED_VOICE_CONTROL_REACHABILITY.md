# Shared Voice Control Reachability — R1

**Version:** 0.1 — Phase 4 UI reachability proposal (2026-11-28)  
**Status:** Proposal for owner review. Does not approve parameter IDs, host exposure, DSP behavior, numeric defaults, MIDI mapping architecture or implementation.  
**Authority:** GS-VOI-001…010, GS-CHD-005, GS-MOD-004/005, GS-MID-003/008, GS-UI-007; DEC-024, DEC-031, DEC-038, DEC-051, DEC-052, DEC-054; UI_UX_SPECIFICATION.md; PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md; PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md; MIDI_MAPPING_UX_SPECIFICATION.md; R1_ENGINE_PARAMETER_CANDIDATE_INVENTORY.md.

## 1. Purpose and scope

This document defines a UI-level reachability proposal for shared voice controls used by the R1 Granular and Sampler engines. “Shared” means the control family belongs to the common voice model; it does not automatically mean every descriptor has identical scope, units, lifecycle or applicability in both engines.

The UI must preserve the four independent layer/patch contexts. Per-voice controls affect the addressed voice(s); layer-level controls affect the addressed layer. Existing MIDI mappings must retain their explicit scope regardless of current UI focus.

MIDI Learn target eligibility is conditional on GS-MID-003 (“any automatable parameter”) and the eventual Q-T12 descriptor contract. GS-MID-008 guarantees controller-only access to essential live-performance operations, not every detailed voice-design parameter.

## 2. Proposed reachability matrix

| Control family | R1 behavior | Proposed touch surface | Encoder reachability | MIDI Learn disposition | Modulation disposition | Scope / open points |
|---|---|---|---|---|---|---|
| Voice allocation / voice stealing | GS-VOI-001 | Voice Status / Allocation section; status and policy presentation | Contextual focus only for any approved user-facing threshold; do not invent a new control | Structural/action target only if an explicit user control exists | Not a continuous destination by default | R1 defines stealing preference and click-free fade; no user-selectable allocation policy is asserted |
| Sustain / Hold CC64 | GS-VOI-002 | Performance/MIDI status area; clear pedal state | Contextual focus only if an approved control is exposed | MIDI CC64 input is behavior, not a user parameter; do not create a duplicate Learn target for the incoming sustain message | Not a destination by default | Keep MIDI performance input separate from parameter mapping |
| Glide Time | GS-VOI-003 | Voice panel, Pitch section | Contextual encoder page | Candidate parameter target if automatable under Q-T12 | Continuous candidate | 0–2000 ms; exact per-layer/per-voice scope and default TBD |
| Always Glide | GS-VOI-003 | Adjacent toggle in Voice / Pitch | Contextual focus | Boolean target only if approved for mapping | Exclude from continuous modulation | Off/On; detailed transition behavior TBD |
| Velocity Sensitivity | GS-VOI-004 | Voice panel, Performance Response section | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | 0–100%; response mapping and scope TBD |
| Velocity Curve | GS-VOI-004 | Voice panel, Performance Response section | Contextual focus | Enum/curve target only if approved | Not a continuous destination unless curve is explicitly continuous | Curve set and numeric representation TBD |
| Pitch Bend Range | GS-VOI-005; DEC-038 | Voice panel, Pitch section; display layer/MPE range clearly | Contextual encoder page only if exposed as a user-editable setting | Candidate target subject to host/MIDI mapping contract | Continuous/stepped candidate only after Q-T12 classification | Product range 0–48 semitones; MPE per-note range fixed at 48 by DEC-038; host metadata remains technical |
| Transpose — semitones | GS-VOI-005 | Voice panel, Pitch section | Contextual encoder page | Candidate parameter target if automatable | Candidate stepped/continuous treatment requires descriptor decision | Per-layer/per-voice semantics TBD |
| Transpose — cents | GS-VOI-005 | Adjacent fine-tune control in Pitch section | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate only after descriptor definition | Range/default TBD; keep separate from semitone control if semantic resolution differs |
| Pitch Envelope ADSR | GS-VOI-006; DEC-051 | Modulation workspace → Envelopes → Pitch | Existing Modulation encoder context: Attack/Decay/Sustain/Release, with explicit envelope selector | Each exposed automatable stage may be a target; target name must include “Pitch Envelope” | Continuous stages/amount candidate; curves/invert discrete or type TBD | Per voice; Attack/Decay/Release 0.1 ms–45 s; sustain level range TBD |
| Filter Envelope ADSR | GS-VOI-006; DEC-051 | Modulation workspace → Envelopes → Filter | Same four-stage contextual encoder page after selecting Filter Envelope | Same rule; target names include “Filter Envelope” | Same as above | Per voice; do not share IDs with Pitch/Amp/Aux stages unless truly same parameter identity |
| Amp Envelope ADSR | GS-VOI-006; DEC-051 | Modulation workspace → Envelopes → Amp | Same four-stage contextual encoder page after selecting Amp Envelope | Same rule; target names include “Amp Envelope” | Same as above | Per voice |
| Aux Envelope ADSR | GS-VOI-006; DEC-051 | Modulation workspace → Envelopes → Aux | Same four-stage contextual encoder page after selecting Aux Envelope | Same rule; target names include “Aux Envelope” | Same as above | Per voice |
| ADSR stage curves | GS-VOI-006 | Selected envelope → Stage Shape subsection | Contextual encoder page where continuous; selector if discrete | Target only after type and automation status are settled | Candidate only if genuinely continuous | Four envelopes × stage identity must be explicit; click-safe behavior required |
| ADSR Amount | GS-VOI-006 | Selected envelope → Amount | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Bipolar; range/default TBD |
| ADSR Invert | GS-VOI-006 | Selected envelope → Invert toggle | Contextual focus | Boolean target only if approved | Exclude from continuous modulation | Off/On |
| LPF Cutoff | GS-VOI-008; DEC-052 | Voice panel → Filters → LPF | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Per voice; 0–20 kHz requirement range; taper/default/smoothing technical |
| LPF Resonance | GS-VOI-008; DEC-052 | Voice panel → Filters → LPF | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Numeric range/curve TBD |
| HPF Cutoff | GS-VOI-008; DEC-052 | Voice panel → Filters → HPF | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Per voice; 0–20 kHz requirement range; taper/default/smoothing technical |
| HPF Resonance | GS-VOI-008; DEC-052 | Voice panel → Filters → HPF | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Numeric range/curve TBD |
| LPF Slope | GS-VOI-008; DEC-052 | Filter panel → LPF Slope selector | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | 6/12/18/24 dB/oct |
| HPF Slope | GS-VOI-008; DEC-052 | Filter panel → HPF Slope selector | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | 6/12/18/24 dB/oct |
| Filter Routing | DEC-052 | Filter panel → Routing selector | Contextual focus | Discrete target only if approved | Exclude from continuous modulation | LPF only / LPF+HPF / HPF only |
| Filter Bypass | DEC-052 | Filter panel → Bypass control | Contextual focus | Boolean target only if approved | Exclude from continuous modulation | Preserve filter settings while bypassed |
| Filter Key Tracking | GS-VOI-009 | Filter panel → Key Tracking | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Range/default and LPF/HPF applicability TBD |
| Sub-Oscillator Frequency | GS-VOI-010; DEC-054 | Voice panel → Sub Oscillator | Contextual focus | Enum target only if approved | Exclude from continuous modulation | Fixed sine; −1 or −2 octaves; no waveform selector |
| Sub-Oscillator Level | GS-VOI-010; DEC-054 | Voice panel → Sub Oscillator | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Range/unit/default TBD |
| Sub-Oscillator Poly Balance | GS-VOI-010; DEC-054 | Voice panel → Sub Oscillator | Contextual encoder page | Candidate parameter target if automatable | Continuous candidate | Range/default TBD |
| Sub-Oscillator Quantize | GS-VOI-010; GS-CHD-005; DEC-054 | Voice panel → Sub Oscillator → Quantize | Contextual focus | Discrete target only if approved | Exclude from continuous modulation unless requirement clarifies continuous amount | Exact values and relation to chord/scale quantization need reconciliation |

## 3. Proposed workspace organization

### 3.1 Voice / Performance

- Voice status and allocation diagnostics.
- Sustain pedal state (CC64).
- Glide Time and Always Glide.
- Velocity Sensitivity and Velocity Curve.
- Pitch Bend Range and Transpose (semitone + cents).
- Per-layer/per-voice scope indicator beside each control.

### 3.2 Envelopes

- Explicit envelope selector: Pitch, Filter, Amp, Aux.
- Four stage controls for the selected envelope: Attack, Decay, Sustain, Release.
- Stage curve, Amount and Invert in a secondary subsection.
- Every target label must include the envelope identity and stage, for example “Filter Envelope — Attack”; “Attack” alone is ambiguous.
- The existing four-encoder Modulation workspace defaults remain Attack, Decay, Sustain and Release. The selected envelope identity must remain visible and must not silently change a mapping target.

### 3.3 Filters

- Separate LPF and HPF subsections with independent Cutoff, Resonance and Slope.
- Routing and Bypass remain distinct controls; do not collapse them into one ambiguous filter-mode selector.
- Key Tracking has a named control and an explicit indication of which filter(s) it affects once that product behavior is resolved.
- Preserve DEC-052: only LPF/HPF; no notch, band-pass or peak controls in R1.

### 3.4 Sub-Oscillator

- Display fixed sine waveform as descriptive status, not as a selector.
- Frequency selector: −1 / −2 octaves.
- Level, Poly Balance and Quantize controls.
- No waveform dropdown or additional oscillator shapes.

## 4. Controller and MIDI Learn rules

1. Any exposed automatable parameter may be offered in MIDI Learn under GS-MID-003, subject to the eventual descriptor registry and mapping rules.
2. The target browser must identify envelope name, stage, filter type and layer/voice scope. Avoid ambiguous labels such as “Attack”, “Cutoff” or “Level” without context.
3. A mapping bound to Layer 2 or a specific patch/voice scope must not follow current UI focus to another layer.
4. Discrete selectors and booleans are not continuous modulation destinations. Their MIDI Learn support requires an explicit value/action representation and approval.
5. Sustain CC64 remains a MIDI performance input, not an ordinary parameter Learn target.
6. GS-MID-008 controller-only operation applies to essential live-performance operations. Detailed envelope/filter/sub-oscillator editing may be MIDI-mappable but is not automatically required to be controller-only accessible.
7. R1 does not require MIDI feedback to controller LEDs/rings/displays (DEC-021).

## 5. Modulation eligibility proposal

Candidate continuous destinations:
- Glide Time, Velocity Sensitivity (subject to source/destination scope).
- Pitch Bend Range and Transpose only if their descriptor semantics permit modulation without conflicting with MIDI expression.
- Each envelope's time stages, Sustain level and Amount.
- LPF/HPF Cutoff and Resonance; Filter Key Tracking.
- Sub-Oscillator Level and Poly Balance.

Not continuous destinations by default:
- Always Glide, Velocity Curve, ADSR Invert, filter Slope/Routing/Bypass, Sub-Oscillator Frequency and Quantize, voice allocation actions, sustain CC64.

This is a candidate classification only. MTX-05 remains open until Q-T12 defines descriptors and every row is reconciled for source scope, destination scope, smoothing/lifecycle and justified exclusion.

## 6. Open decisions / gates

1. Confirm exact per-layer vs per-voice scope for each shared control.
2. Define whether pitch-bend range is an editable parameter, host/MIDI configuration or both, while preserving DEC-038.
3. Define envelope stage curve types and exact target identities.
4. Define LPF/HPF resonance range/curve, key-tracking applicability and control update semantics.
5. Define Sub-Oscillator Level/Poly Balance ranges and Quantize values.
6. Approve Q-T12 descriptor IDs, host numeric mapping, state migration, automation and MIDI Learn flags.
7. Complete MTX-05 descriptor-level modulation reconciliation and Phase 5 acceptance coverage.

**Boundary:** UI reachability proposal only. No architecture, DSP algorithm, MIDI CC layout, parameter ID, host exposure, implementation, test or release decision is made here.

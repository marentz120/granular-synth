# R1 Engine Parameter Candidate Inventory — Granular and Sampler

**Version:** 0.1 — documentation candidate inventory (2026-11-28)  
**Status:** Candidate inventory only; not a canonical registry, parameter-ID approval, DSP contract, or implementation authorization.  
**Scope:** R1 Granular and Sampler engine controls plus shared voice controls. FX, modulation-source controls, macros, sequencer and layer mixer are excluded.  
**Authority:** GRANULAR_SYNTH_REQUIREMENTS.md (GS-GRN-001…020, GS-SMP-001…004, GS-VOI-001…010), SAMPLER_SPECIFICATION.md, MODULATION_SPECIFICATION.md, PARAMETER_ID_SPECIFICATION.md, DEC-030, DEC-035, DEC-037, DEC-043…055.

## 1. Reading this inventory

This document translates approved R1 requirements into candidate rows. It does not invent values absent from approved sources.

- **Range/default:** populated only where an approved requirement or decision supplies the value; otherwise TBD.
- **Type:** continuous, stepped, enum, boolean, derived/read-only, or structural/action.
- **Scope:** product scope only; final layer/voice/grain descriptor scope still requires Q-T12 reconciliation.
- **Update/lifecycle:** only explicit approved boundaries are stated. TBD must not be inferred from likely DSP implementation.
- **Modulation/automation:** candidate eligibility is not approval. Q-T12 must settle descriptor flags and host exposure.

## 2. Granular engine candidates

| Candidate parameter | Type | Unit | Approved range / values | Default | Scope | Update/lifecycle | Source |
|---|---|---|---|---|---|---|---|
| Grain Rate | Continuous | grains/s per voice | 0.1–1000 | TBD | Per voice | TBD | GS-GRN-001 |
| Grain Size | Continuous | ms | 0.1–5000 | TBD | Per voice | TBD | GS-GRN-002; DEC-035 |
| Grain Density display | Derived/read-only | grains (rate × size) | Derived | N/A | Per voice | Recompute/display contract TBD | GS-GRN-003; not writable by default |
| Position | Continuous | sample-relative/normalized TBD | Resolution ≥14-bit; range TBD | TBD | TBD | TBD | GS-GRN-004 |
| Spray Amount | Continuous | sample-relative amount | 0 to full sample | TBD | TBD | TBD | GS-GRN-005 |
| Spray Polarity | Enum | polarity | Bipolar, +Unipolar, −Unipolar | TBD | TBD | TBD | GS-GRN-005 |
| Pan | Continuous | normalized pan | −1…+1 is defined for pan-spray legal interval; base-pan descriptor range needs confirmation | TBD | TBD | TBD | GS-GRN-006; DEC-053 |
| Pan Spray Amount | Continuous | normalized pan spread | Maximum spread around base pan, bounded by [−1,+1] | TBD | TBD | Each grain samples pan at spawn and retains it for its lifetime | DEC-053 |
| Pan Spray Distribution | Enum | distribution | Uniform, Centre Weighted | TBD | TBD | Sampled at grain spawn | DEC-053 |
| Mid-Side Width | Continuous | width | Range/unit TBD | TBD | Per layer | TBD | GS-GRN-007 |
| Direction Probability | Continuous | probability | All reverse ↔ 50/50 ↔ all forward; numeric mapping TBD | TBD | TBD | TBD | GS-GRN-008 |
| Scan | Continuous | multiplier | −2×…+2× | TBD | TBD | Near-zero region retains last valid Rate/Size; epsilon remains technical | GS-GRN-009; DEC-043 |
| Scan Mode | Enum | mode | Bounce, Wrap, Kill | TBD | Per layer | Boundary behavior in DEC-045/047 | DEC-045, DEC-047 |
| One-Shot | Boolean | N/A | Off/On | TBD | Per layer | Stops spawning after one complete scan cycle; active grains finish envelopes | DEC-044 |
| Scan Fine/Snap | Enum | mode | Fine, ±0.25 snap | TBD | TBD | TBD | GS-GRN-009 |
| Pitch | Continuous/stepped | semitones | Coarse ±1 octave; fine ±1 semitone | TBD | Per grain/voice TBD | TBD | GS-GRN-010 |
| Pitch Spray Amount | Continuous | pitch interval | Range TBD | TBD | TBD | TBD | GS-GRN-010 |
| Pitch Spray Polarity | Enum | polarity | Values TBD | TBD | TBD | TBD | GS-GRN-010 |
| Pitch Snap | Enum/state | musical quantization | Semitones, chord notes; scale quantization also specified by GS-CHD-005 | TBD | TBD | TBD | GS-GRN-010 |
| Ratio Mode | Enum | mode | Free, DensitySize, DensityRate, ScanRate, ScanOverlap, TimeStretch | TBD | TBD | TBD | GS-GRN-011 |
| Ratio continuous inputs | Continuous | mode-specific | Range/unit per mode TBD | TBD | TBD | TBD | GS-GRN-011; no equations invented |
| PowAR Sides | Continuous/stepped TBD | TBD | TBD | TBD | TBD | Existing-grain click-free behavior required; update class TBD | GS-GRN-012/013 |
| PowAR Tilt / Curve | Continuous | normalized/window-specific | TBD | TBD | TBD | Existing-grain click-free behavior required; update class TBD | GS-GRN-012/013 |
| Raised-Cosine Tilt / Curve / AM | Continuous | normalized/window-specific | TBD | TBD | TBD | Existing-grain click-free behavior required; update class TBD | GS-GRN-012/013 |
| Grain Key Trigger | Enum | mode | Off, On, Legato | TBD | Per layer | Note-trigger semantics; does not alter independent grain clock | DEC-048 |
| Grain Clock Division | Enum/stepped | musical division | Straight/triplet/dotted; 1/64…64/1 | TBD | Per layer | Scheduler/transport arbitration TBD | GS-GRN-014 |
| Voice Sync | Boolean | N/A | Off/On | TBD | Per layer | Changes take effect at next valid scheduler boundary; DEC-055 | DEC-050/055 |
| Spawn Mode | Enum | mode | Gradual, Direct, Direct Reset, Recycle | TBD | Per layer | Voice/scheduler initialization per DEC-049 | DEC-049 |
| Local Pitch | Boolean | N/A | Off/On | TBD | Per layer/voice TBD | Grain retains birth pitch when enabled; transition details TBD | GS-GRN-016 |
| Grain Stealing Threshold | Continuous/stepped TBD | threshold | 0 (off)…maximum; max TBD | TBD | TBD | TBD | GS-GRN-017 |
| Grain-Count Normalization | Boolean | N/A | Off/On | TBD | Counting domain TBD | Smoothing/counting domain remain technical | GS-GRN-018; DEC-046 |
| Interpolation Quality | Enum | quality mode | At least Linear and one higher-quality mode (cubic Hermite minimum) | TBD | Per layer | Transition behavior TBD | GS-GRN-020; Q-T14 |

### 2.1 R1 Granular exclusions

- Grain Probability, Granular Filter and anti-aliased upward transposition are L2 (GS-GRN-021…023).
- Granular Slice, Tape and Spectral controls are not R1.
- Sample/file loading, sample replacement, Panic and recording start/stop are structural/state operations, not continuous engine parameters.
- Density is derived/read-only unless a later approved requirement defines a writable control.

## 3. Sampler engine candidates

Sampler is conventional sample playback: pitch changes playback speed; it is not time-stretched. It shares sample assets, flags and voice/envelope infrastructure but does not inherit granular-only controls as active Sampler controls.

| Candidate parameter | Type | Unit | Approved range / values | Default | Scope | Update/lifecycle | Source |
|---|---|---|---|---|---|---|---|
| Playback Pitch / Speed | Continuous | semitones or ratio TBD | Range TBD; pitch changes playback speed | TBD | Per voice/playhead | TBD | GS-SMP-001 |
| Scan / Spawn Position | Continuous | sample-relative position | Range TBD; constrained by active crop/loop bounds | TBD | Per voice/playhead | New note uses selected scan/spawn position; later updates TBD | GS-SMP-001; SAMPLER_SPEC §2 |
| Spray Amount | Continuous | sample-relative | Mapping/range TBD; do not assume granular mapping without shared-control reconciliation | TBD | Per note/playhead | Start-position randomization described; exact lifecycle TBD | GS-SMP-001 |
| Pan | Continuous | normalized pan | TBD | TBD | Per voice/layer TBD | TBD | GS-SMP-001 |
| Pan Spray Amount | Continuous | normalized spread | Mapping TBD; legal stereo interval [−1,+1] | TBD | Per note/playhead | Placement sampled; Sampler-specific lifecycle needs confirmation | GS-SMP-001; DEC-053 |
| Pan Spray Distribution | Enum | distribution | Uniform, Centre Weighted | TBD | Per note/playhead | Verify shared Sampler control identity | DEC-053; GS-SMP-001 |
| Direction | Enum/boolean | direction | Forward, Reverse (other values TBD) | TBD | Per voice/playhead | TBD | GS-SMP-001 |
| Playback Mode | Enum | mode | One-shot, Loop, Ping-pong | TBD | Per voice/playhead | Sample-boundary behavior | GS-SMP-002 |
| Loop Release | Boolean | N/A | Off/On | TBD | Voice/layer policy TBD | On: loop through voice release. Off: exit at next valid loop boundary and continue toward Stop | DEC-030 |
| Sample Start / Stop | Continuous or structural boundary | frames/normalized position | Constrained by sample length; mapping TBD | TBD | Shared sample/layer | Boundary changes take effect on next note where specified | GS-SMP-002; SAMPLER_SPEC §§2–3 |
| Loop Start / Loop End | Continuous or structural boundary | frames/normalized position | Constrained by sample length; mapping TBD | TBD | Shared sample/layer | Boundary change takes effect on next note | SAMPLER_SPEC §3 |
| Env Pile-up | Boolean | N/A | Off/On | Off | Per layer | Same-note retrigger behavior; voice lifecycle contract TBD | GS-SMP-004; DEC-037 |
| Interpolation Quality | Enum | quality mode | High-quality polyphase/windowed-sinc option required; exact mode set TBD | TBD | Per layer | Transition behavior TBD | GS-SMP-003 |

### 3.1 Sampler exclusions / non-parameter operations

- Long samples up to 60 minutes and streaming are L2 (GS-SMP-005); loop crossfade/zero-crossing are L2 (GS-SMP-006); SFZ multisample import is L3 (GS-SMP-007).
- Sample load/replace, relink, Freeze capture/thaw and recording are asset/state operations, not ordinary continuous Sampler parameters.
- Grain rate/size, grain windows, spawn modes, grain clock, Voice Sync and grain normalization are not active Sampler controls.

## 4. Shared voice controls — cross-engine, not duplicated

The canonical registry should use one semantic identity where behavior is genuinely shared, with engine applicability metadata. Do not create duplicate IDs merely because controls appear in different UI workspaces.

| Candidate parameter | Type | Unit | Approved range / values | Default | Scope | Update/lifecycle | Source |
|---|---|---|---|---|---|---|---|
| Glide Time | Continuous | ms | 0–2000 | TBD | Voice/layer TBD | TBD | GS-VOI-003 |
| Always Glide | Boolean | N/A | Off/On | TBD | TBD | TBD | GS-VOI-003 |
| Velocity Sensitivity | Continuous | % | 0–100 | TBD | Voice/layer TBD | Note/voice response; mapping TBD | GS-VOI-004 |
| Velocity Curve | Continuous/enum TBD | curve | Curve set/range TBD | TBD | TBD | TBD | GS-VOI-004 |
| Pitch Bend Range | Continuous/stepped | semitones | 0–48 in GS-VOI-005; MPE per-note range fixed at 48 by DEC-038 | TBD | Layer/MPE zone TBD | Host/MIDI mapping technical | GS-VOI-005; DEC-038 |
| Transpose | Stepped/continuous pair | semitones + cents | TBD | TBD | Voice/layer TBD | TBD | GS-VOI-005 |
| ADSR Attack / Decay / Release (four envelopes) | Continuous | ms | 0.1–45,000 | TBD | Per voice | Update behavior TBD; 0.1 ms floor is approved | GS-VOI-006; DEC-051 |
| ADSR Sustain (four envelopes) | Continuous | normalized level | Range TBD | TBD | Per voice | TBD | GS-VOI-006; DEC-051 |
| ADSR Stage Curves | Continuous/enum TBD | curve | TBD | TBD | Per voice | Click-safe behavior required; lifecycle TBD | GS-VOI-006 |
| ADSR Amount | Continuous | bipolar amount | Range TBD | TBD | Per voice | TBD | GS-VOI-006 |
| ADSR Invert | Boolean | N/A | Off/On | TBD | Per voice | TBD | GS-VOI-006 |
| LPF / HPF Cutoff | Continuous | Hz | 0–20,000 in GS-VOI-008 | TBD | Per voice | Smoothing/lifecycle technical | GS-VOI-008; DEC-052 |
| LPF / HPF Resonance | Continuous | TBD | Numeric range/curve TBD | TBD | Per voice | Smoothing/lifecycle technical | GS-VOI-008; DEC-052 |
| Filter Slope | Enum | dB/oct | 6, 12, 18, 24 | TBD | Per voice/filter | Stepped; update boundary TBD | GS-VOI-008; DEC-052 |
| Filter Routing | Enum | topology | LPF only, LPF+HPF, HPF only | TBD | Per voice | Stepped; update boundary TBD | DEC-052 |
| Filter Bypass | Boolean | N/A | Off/On | TBD | Per voice | TBD | DEC-052 |
| Filter Key Tracking | Continuous | normalized amount | Range TBD | TBD | Per voice | TBD | GS-VOI-009 |
| Sub-Oscillator Frequency | Enum | octaves | −1, −2 | TBD | Per voice | TBD | GS-VOI-010; DEC-054 |
| Sub-Oscillator Level | Continuous | level/dB TBD | Range TBD | TBD | Per voice | TBD | GS-VOI-010; DEC-054 |
| Sub-Oscillator Poly Balance | Continuous | normalized balance TBD | Range TBD | TBD | Per voice | TBD | GS-VOI-010; DEC-054 |
| Sub-Oscillator Quantize | Boolean/enum TBD | N/A | Option required; values TBD | TBD | Per voice | TBD | GS-VOI-010; DEC-054 |

## 5. Modulation and host-automation eligibility — preliminary only

- Continuous candidates are eligible for review, not automatically marked modulatable or automatable.
- Enum, boolean, structural-boundary and action/state rows require explicit justification before modulation or host automation exposure.
- Spawn/lifecycle behavior must enter descriptor metadata only after the relevant engineering contract is approved.
- R1_MODULATION_DESTINATION_ELIGIBILITY_MATRIX.md remains the broader MTX-05 domain baseline. This inventory supplies engine-specific candidate rows and does not close MTX-05.
- FX are absent because DEC-056 stages FX parameter-contract closure for M14.

## 6. Open questions and reconciliation

1. **Q-T12 remains open:** canonical ID grammar, numeric host IDs, descriptor schema, host exposure and migration rules are not approved.
2. **Missing ranges/defaults:** position, pan, pitch spray, ratio inputs, window controls, Sampler scan/spray mapping, direction representation, voice defaults, filter resonance and sub-oscillator level/balance need source-backed decisions or engineering contracts.
3. **Shared-control identity:** decide which controls are genuinely shared across Granular and Sampler; avoid duplicate IDs or accidental control inheritance.
4. **Lifecycle metadata:** define update class, smoothing, active-voice behavior and host-event precedence. Only Pan Spray grain sampling, One-Shot, Voice Sync and selected Sampler boundary semantics are explicit.
5. **Sampler interpolation:** polyphase/windowed-sinc is mandatory in R1; mode set, default, filter length and transition behavior remain technical gates.
6. **Modulation reconciliation:** compare every final R1 continuous descriptor with MTX-05 and justify exclusions.
7. **No execution evidence:** no DSP implementation, host validation, tests, performance measurements or acceptance completion are claimed.

## 7. Recommended closure sequence

1. Resolve product-level TBDs only where the source documents require a user-facing choice; do not invent DSP-specific values.
2. Obtain owner/technical disposition for Q-T12 before creating the canonical registry.
3. Generate the registry from the approved descriptor contract and this candidate inventory.
4. Reconcile R1 engine/shared-voice descriptors against modulation, MIDI Learn, automation, preset/state and UI exposure.
5. Close MTX-05 only after 100% row-level reconciliation and evidence; FX rows join at M14.

**Boundary:** documentation-only preparation. No code, DSP, host prototype, tests, benchmarks, parameter-ID publication, PR merge or release is authorized by this document.

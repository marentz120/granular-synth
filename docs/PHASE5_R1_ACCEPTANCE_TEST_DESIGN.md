# GRANULAR SYNTH — Phase 5 R1 Acceptance Test Design

**Version:** 0.3  
**Status:** Test-design proposal; owner/technical review required. No tests executed.  
**Baseline branch:** `phase4/ui-ux-specification`  
**Authority:** `R1_ACCEPTANCE_CRITERIA.md`, `PHASE5_TEST_DESIGN_BACKLOG.md`, `GRANULAR_SYNTH_REQUIREMENTS.md` v1.0, approved DEC records, Phase 3 interface and architecture contracts, and Phase 4 UX proposals.

## 1. Purpose and boundary

This document turns the 71 R1 requirements without an existing acceptance/release-gate link into an actionable test-design blueprint. It supplements, rather than replaces, the requirement-level intent/evidence/dependency rows in `PHASE5_TEST_DESIGN_BACKLOG.md`.

It defines reusable fixture families, scenario sequences, observable oracles, evidence artifacts, failure classifications and dependency gates. It does **not** create formal `AC-*` identifiers, select unresolved product behavior, assign unapproved numeric thresholds, authorize code/host prototypes/benchmarks, or claim a test passed.

The 71 requirement IDs are preserved individually in §3. A shared suite may exercise several requirements, but evidence must retain a requirement-level assertion and trace.

## 2. Corrected coverage baseline

| Measure | Count |
|---|---:|
| Canonical R1 requirements | 124 |
| Direct acceptance-criteria links | 49 |
| Additional cross-domain release-gate links | 4 |
| Existing links in total | 53 |
| Requirements without either link | 71 |
| Those 71 represented in the existing Phase 5 backlog | 71 |
| Missing backlog entries | 0 |

The 71 backlog rows are present exactly once in the requirement-level planning table. This is planning coverage only. The formal acceptance inventory remains 53/124 until formal criteria are added and approved.

## 3. Requirement-level test-design register

The detailed candidate test intent, evidence and dependency for each row remain in `PHASE5_TEST_DESIGN_BACKLOG.md` §3. This register assigns each requirement to a proposed test family and states the observable assertion that must be preserved when formal acceptance wording is authored.

| Requirement | Test family | Candidate observable oracle | Design status |
|---|---|---|---|
| GS-PLT-002 | Packaging / architecture | Produced Universal 2 artifact contains exactly the architecture slices required by the approved platform policy; package metadata and binary inspection agree. | Platform policy gate |
| GS-PLT-003 | Render configuration / host integration | For every approved sample-rate and block-size case, processing remains valid across size changes; parameter-flush calls do not advance audio time or access audio buffers. | Sample-rate policy gate |
| GS-ARC-001 | Four-layer integration | Four layers retain independent patch, voice, modulation, FX and MIDI state while simultaneous rendering and shared-channel fan-out follow DEC-034. | Designable |
| GS-ARC-002 | Voice allocator boundaries | Each layer accepts its configured polyphony within the approved 1–20 range; allocator state does not leak between layers. | AR-02 interaction gate |
| GS-ARC-003 | Per-voice grain pool | A voice admits no more than 128 simultaneous grains; boundary admission and retirement preserve ownership/count invariants. | Designable |
| GS-ARC-004 | Global resource admission | Repeated identical overload fixtures produce the same admission/degradation trace and preserve safe output; global budget value and policy are not frozen until AR-02/Q-T07. | Decision/benchmark gate |
| GS-ARC-006 | Audio bus topology | Master and four optional layer outputs map to the declared channels; inactive/omitted buses do not cause invalid access or duplicate summing. | Host matrix gate |
| GS-ARC-007 | Signal routing | Distinct source signals are observable at the specified wet, dry, layer-stem and master taps in the approved order; no path is summed twice. | Designable |
| GS-ARC-008 | Default headroom | Four-layer dense default-state fixtures show no unintended clipping under the approved reference fixture and subsequently approved headroom oracle. | Threshold gate |
| GS-GRN-001 | Grain scheduler range | 0.1 and 1000 grains/s endpoints are accepted as specified; generated event timing follows the selected rate without drift beyond the later-approved timing tolerance. | Timing tolerance gate |
| GS-GRN-002 | Grain-size conversion | 0.1 and 5000 ms endpoints convert to the expected frame duration for every approved sample rate. | Q-T17 gate |
| GS-GRN-003 | Derived density model | Displayed density equals the canonical Rate × Size model after either input changes; UI and model use the same value. | Display formatting gate |
| GS-GRN-004 | Position precision | The exposed position supports at least 14-bit resolution and internal calculations retain double-precision representation across the normalized range. | Designable |
| GS-GRN-005 | Spray distribution | Bipolar, positive-unipolar and negative-unipolar modes produce values within their defined domains; seeded fixtures are reproducible. | Statistical tolerance gate |
| GS-GRN-006 | Pan distribution | Uniform and centre-weighted modes produce stereo positions consistent with their approved distribution definitions; pan and pan-spray remain independent. | Curve/tolerance gate |
| GS-GRN-007 | Mid-side width | Neutral and boundary settings produce the approved stereo reference vectors without cross-layer state leakage. | Designable |
| GS-GRN-008 | Direction probability | Probability endpoints select only the specified direction; midpoint distribution is evaluated using fixed seeds and an approved statistical tolerance. | Statistical tolerance gate |
| GS-GRN-009 | Scan / sample region | Fine, snap and ±2× range cases respect active Start/Stop/Loop flags and never read outside the valid sample region. | Designable |
| GS-GRN-010 | Pitch mapping | Coarse/fine pitch, spray polarity and scale/chord snapping resolve to the same canonical pitch model used by chord quantization. | Shared pitch-contract gate |
| GS-GRN-011 | Ratio modes | Each of six modes follows its specified mapping; scan=0 behavior is tested only after the source specification states that behavior unambiguously. | Specification gate |
| GS-GRN-012 | Window functions | PowAR and Raised Cosine variants match approved numerical reference vectors for sides/tilt/curve/AM settings. | Numerical tolerance gate |
| GS-GRN-013 | Active-grain window transition | Window changes/modulation do not create a discontinuity in already-playing grains under the approved measurement method. | GS-NFR-003 threshold gate |
| GS-GRN-014 | Trigger / clock division | Off/On/Legato and straight/triplet/dotted divisions schedule events at the expected positions on a deterministic transport timeline. | IF-08/clock authority gate |
| GS-GRN-015 | Spawn modes | Gradual, Direct, Direct-reset and Recycle produce the specified state transitions and reset/recycle boundaries. | Specification gate |
| GS-GRN-016 | Local pitch | ON retains the grain's birth pitch after later note changes; OFF follows the specified live-pitch behavior. | Designable |
| GS-GRN-017 | Grain stealing | OFF preserves the configured pool behavior; active mode selects/retire grains according to the approved policy without an audible discontinuity. | AR-02 gate |
| GS-GRN-014 | Grain Key Trigger | Off produces no key-triggered spawn; On triggers on each accepted Note On; Legato triggers only on zero-to-nonzero held-note transition. Note Off/voice-release and grain clock behavior remain independent. | Event-order and held-note lifecycle gate |
| GS-GRN-018 | Grain-count normalization | With normalization ON, per-grain gain follows DEC-046 (1/√N relative to the single-grain reference) as active grain count changes; OFF bypasses this gain law. | Counting domain, smoothing, ON/OFF reference and numeric tolerance gate |
| GS-GRN-019 | Event timing precision | Event offsets remain invariant across block partitions and meet the defined sub-sample placement contract; same-offset precedence is not asserted before AR-01. | AR-01 gate |
| GS-GRN-020 | Interpolation | Linear and cubic-Hermite outputs match numerical references and remain bounds-safe; full-load performance is a separate gated qualification. | Reference machine gate |
| GS-CHD-001 | Chord model | All 12 slots are addressable in both R1 engines; fine tune, duplicate-note and lowest-note-root behavior match the chord contract. | Designable |
| GS-CHD-002 | Chord voice distribution | Granular chord distribution preserves the specified level behavior; Sampler stacking produces the specified per-note voices. | Reference-render gate |
| GS-CHD-003 | Arpeggiator | All ten modes generate the specified event order; random/shuffle cases reproduce under fixed seeds. | Golden-sequence specification gate |
| GS-CHD-004 | Chord capture | Arm/capture/commit stores the captured note set once; recall produces no missing or duplicate notes. Overlap/held-note semantics remain explicitly gated. | Owner/specification gate |
| GS-CHD-005 | Scale / quantization | Preset and user scales produce identical canonical pitch decisions across per-voice, per-grain and sub-oscillator paths. | Shared pitch-contract gate |
| GS-VOI-001 | Voice allocator | Under pressure, victim selection follows released → non-sustained → oldest priority and retirement uses the approved fade policy. | AR-02/fade gate |
| GS-VOI-002 | Sustain | CC64 down/up, note-off and layer/channel routing preserve the approved held/released voice lifecycle. | Designable |
| GS-VOI-003 | Glide | 0 and 2000 ms endpoints and Always mode produce the specified pitch trajectories across note transitions. | Trajectory tolerance gate |
| GS-VOI-004 | Velocity response | 0–100% sensitivity and the approved velocity curve produce the specified response at endpoints and representative intermediate values. | Curve/default gate |
| GS-VOI-005 | Pitch bend / transpose | Channel pitch bend and transpose combine according to the approved mapping at range endpoints; channel and MPE per-note ranges are tested separately. | Mapping contract gate |
| GS-VOI-006 | ADSR | Four envelopes expose approved stage ranges (0.1 ms–45 s); stage curves, bipolar amount and inversion match numerical/timing oracles. | Timing tolerance gate |
| GS-VOI-008 | Filter chain | LPF/HPF ranges, resonance, 6/12/18/24 dB slopes, bypass and LPF→HPF order match approved response references. | Response tolerance gate |
| GS-VOI-009 | Key tracking | Note changes produce the approved cutoff-tracking response across the specified note range. | Curve/range specification gate |
| GS-VOI-010 | Sub-oscillator | −1/−2 octave, level, poly balance and quantize behavior match the canonical pitch model. | Shared pitch-contract gate |
| GS-SMG-002 | Import / audition UI | Finder/DAW drag-drop and in-app audition complete with visible status; audition output corresponds to the selected asset. | Host/UI gate |
| GS-SMG-003 | Asynchronous sample swap | During load, the prior valid sample remains active; publication occurs only after preparation; render continuity and ownership traces remain valid. | IF-03/IF-05 gate |
| GS-SMG-004 | Normalize on import | ON applies the approved normalization target/algorithm; OFF preserves source amplitude and source asset. | Algorithm specification gate |
| GS-SMG-005 | Waveform visualization | Zoom/scroll and voice/grain/flag/slice/record-head overlays track the model snapshot without reading mutable audio-thread state. | Phase 4 visual gate |
| GS-SMG-006 | Sampler region/playback | Start/Stop/Loop flags and One-shot/Loop/Bounce, key-trigger and clock-sync cases respect valid region boundaries and DEC-030 loop-release behavior. | Designable |
| GS-SMG-007 | Non-destructive edits | Crop variants, soft-cut crossfade and normalize preserve the source asset; Undo restores the prior edit state and audio. | IF-05/IF-11 gate |
| GS-SMG-019 | Long sample memory admission | Six-minute stereo/48 kHz per-layer target is loaded/processed without corruption or unsafe admission; memory ceiling/soak thresholds require Q-T07 approval. | Resource/benchmark gate |
| GS-SEQ-001 | Central sequencer model | One sequencer drives four layers; variable length/resolution and up to eight events per step survive save/load and deterministic scheduling. | Designable |
| GS-SEQ-002 | Parameter locks / writer precedence | Lanes and locks apply at their specified timeline points; conflicting same-offset writers remain decision-gated by AR-01. | AR-01 gate |
| GS-SEQ-003 | Timing grid | Straight/triplet, quantization, microtiming, Swing and Humanize produce expected event positions; Humanize is reproducible under fixed seeds. | Seed/curve specification gate |
| GS-SEQ-004 | Pattern recall / chaining | Save/load, chain order and end-of-bar switch occur at the specified boundary; active Pattern identity remains observable. | Designable |
| GS-SEQ-008 | Play/Drone and Panic | Per-layer latch is isolated; Panic silences as specified and remains distinct from Freeze and transport Stop. | AC-AUD-025 cross-link |
| GS-SEQ-009 | Layer mix / position controls | Controller and UI changes affect only the addressed layer; captured layer outputs reflect the requested position/mix change. | DEC-016/021; Q-T12 |
| GS-SEQ-011 | Fill / Mute | Fill changes granular character without changing rhythmic structure; layer/step mute is immediate and event unmute follows the approved rule. | Product-rule reference |
| GS-SEQ-012 | Live edit / save lifecycle | Edit, pending bar-end activation, Save, temporary Draft, Undo and Redo produce the specified state transitions; no persistent version archive is introduced. | Designable |
| GS-PRS-009 | Patch comments | Comments round-trip with their own Patch and do not leak into another Patch or overwrite unrelated preset state. | UI placement gate |
| GS-UI-001 | Waveform / grain view | UI renders waveform and grain state from immutable snapshots; stale or unavailable snapshots do not block audio or expose mutable runtime pointers. | Snapshot contract gate |
| GS-UI-002 | Engine A–H bank | Switching Granular/Sampler changes the displayed target map without changing underlying parameter values; generic MIDI mappings address the correct target/scope. | Owner A–H approval; Q-T12 |
| GS-UI-003 | Window / density visualization | Window shape reflects active parameters and density display matches the GS-GRN-003 canonical calculation. | Visual formatting gate |
| GS-UI-004 | Accessibility / scaling | Resize/HiDPI, keyboard navigation, tooltips and hover readout remain usable across the approved OS/display matrix. | Q-T03/platform gate |
| GS-UI-005 | CPU meter | Voice/FX/housekeeping metrics are sourced from stable snapshots and update without blocking render processing. | Snapshot contract; no CPU budget implied |
| GS-UI-006 | Shared editor behavior | Standalone, AU and VST3 wrappers preserve the same editor semantics for equivalent state; host-specific differences are recorded. | Q-P09/component gate |
| GS-UI-007 | MIDI Learn editor | Learn, manual assign, conflict, reassign, unmap, reset and controller-only essential workflows produce visible, recoverable mapping state. | DEC-021; Q-T12 |
| GS-NFR-004 | Algorithmic latency | Reported and measured latency are separately accounted for; R1 Granular/Sampler add no algorithmic latency under the approved process model. | Latency definition gate |
| GS-NFR-005 | RNG determinism | Fixed-seed offline renders reproduce within the approved same-platform oracle; independent instances do not share mutable RNG state. | FP/cross-architecture policy gate |
| GS-NFR-009 | Progressive sample analysis | Five-minute stereo asset analysis reports progress and publishes waveform peaks progressively without UI freeze under the approved responsiveness oracle. | Q-T07/reference-machine gate |
| GS-NFR-011 | Test registration meta-check | Every implemented DSP component is present in the component/test inventory and has its required test-family links in CI. | Implementation inventory gate |
| GS-NFR-013 | Release packaging | Supported macOS minimum, hardened-runtime signing and notarization checks match the approved distribution policy. | Q-T03/legal/distribution gate |

## 4. Shared test-suite design

The following suites reduce duplicated setup while retaining per-requirement assertions.

### 4.1 Platform and render-contract suite

**Requirements:** GS-PLT-002, GS-PLT-003.

**Fixture:** build artifact manifest; approved macOS/architecture matrix; deterministic audio input; sample-rate/block-size matrix; host-process event script.

**Sequence:**
1. Inspect package metadata and binary architecture slices.
2. Instantiate each approved format/environment.
3. Render a fixed input across each approved sample-rate/block-size pair.
4. Change block size between processing runs and compare event positions/output continuity.
5. Issue parameter updates during a VST3 parameter-flush call with no audio buses.

**Oracle/evidence:** architecture inspection report, host/build manifest, block/event trace and render comparison. Do not declare supported values until Q-T03/Q-T17 are approved.

### 4.2 Layer, allocator and routing suite

**Requirements:** GS-ARC-001/002/003/004/006/007/008; GS-VOI-001.

**Fixture:** four distinguishable layer patches and source signals; unique parameter sentinels; allocator counters; output-bus capture points.

**Sequence:**
1. Configure each layer with different patch, modulation, FX and channel values.
2. Trigger shared-channel and distinct-channel note events.
3. Reach the per-layer voice and per-voice grain boundaries.
4. Exercise global overload only after AR-02 policy is resolved.
5. Capture master and optional layer buses while toggling activation.
6. Inject distinct dry/wet signals and compare each defined routing tap.

**Oracle/evidence:** state snapshots, allocator ownership/count traces, bus map, routed render captures and peak/RMS report. Global capacity, headroom margin and host topology pass thresholds remain gated.

### 4.3 Granular DSP suite

**Requirements:** GS-GRN-001…020.

**Fixture:** deterministic sample assets with known boundaries and impulses; fixed random seeds; reference parameter vectors; sample-rate matrix; event timeline.

**Sequence:**
1. Test every approved parameter endpoint and representative interior values.
2. Compare grain scheduler timestamps against the deterministic timeline.
3. Test spray/pan/direction distributions using seeded runs.
4. Test scan/loop/stop boundaries against known sample-region flags.
5. Compare window and interpolation outputs to approved numerical reference vectors.
6. Change window/pitch/scan during active grains and capture transition traces.
7. Repeat event fixtures under different block partitions.

**Oracle/evidence:** parameter/property report, event trace, numerical error report, deterministic render hashes or tolerance-based comparison, and transition waveform. Statistical and audible-discontinuity thresholds must be approved before final pass/fail wording.

### 4.4 Chord, voice and pitch suite

**Requirements:** GS-CHD-001…005; GS-VOI-001…010.

**Fixture:** deterministic MIDI note sequences including repeated pitches, overlapping notes, sustain, velocity endpoints, pitch bend and fixed scale definitions.

**Sequence:**
1. Exercise all 12 chord slots in both R1 engines.
2. Capture chord recording and recall; include held/overlap cases only after their semantics are approved.
3. Exercise each arpeggiator mode with a fixed seed and transport.
4. Test voice allocator priority, sustain, glide, velocity, bend/transpose and envelope/filter/sub-oscillator controls.
5. Compare all quantized pitch decisions against one canonical scale/chord mapping oracle.

**Oracle/evidence:** MIDI event log, voice lifecycle trace, pitch trajectory, numerical filter/envelope reference output and deterministic render. AR-02 and unresolved curve/timing details block final pass wording where noted.

### 4.5 Sample, recording and source-lifecycle suite

**Requirements:** GS-SMG-002/003/004/005/006/007/019.

**Fixture:** known WAV/AIFF/FLAC files, source checksums, long stereo sample, valid/invalid sample regions, concurrent render load and edit history.

**Sequence:**
1. Import by drag/drop and browser; audition selected asset.
2. Trigger a sample replacement while rendering continues.
3. Verify old sample remains active until new sample is ready and published.
4. Compare normalize ON/OFF output and source checksum.
5. Zoom/scroll waveform and inspect overlay snapshots.
6. Edit/crop/crossfade/normalize; Undo and compare restored audio/source.
7. Exercise Sampler region and loop-release boundaries.
8. Run long-sample memory-admission only after reference hardware and resource policy are approved.

**Oracle/evidence:** import lifecycle log, source hashes, audio continuity trace, view snapshots, edit history and memory report. No decoder or file I/O is permitted on the audio callback.

### 4.6 Sequencer suite

**Requirements:** GS-SEQ-001/002/003/004/008/009/011/012.

**Fixture:** four-layer Pattern with variable length/resolution, eight events on a step, parameter locks, deterministic transport and fixed Humanize seed.

**Sequence:**
1. Save/recall Patterns and verify event/layer data.
2. Schedule events across variable-length boundaries and all supported timing grids.
3. Exercise locks and competing writers only after AR-01 is resolved.
4. Edit during playback; verify pending end-of-bar activation.
5. Exercise Save, temporary Draft, Undo and Redo.
6. Exercise per-layer Play/Drone, Fill, layer/step/event Mute and Panic.

**Oracle/evidence:** Pattern state snapshots, event timeline, output captures, undo history and controller action trace. The 16-step strip is only a viewport; tests must include Pattern positions outside the visible window.

### 4.7 Preset, UI and controller suite

**Requirements:** GS-PRS-009; GS-UI-001…007.

**Fixture:** distinct Presets/Patches, Granular and Sampler engine states, generic MIDI controller event script, mapping conflicts, display scaling snapshots and immutable UI telemetry snapshots.

**Sequence:**
1. Save/recall comments and ensure Patch isolation.
2. Verify waveform/grain and window/density visualizations against model snapshots.
3. Switch engine and verify A–H target map and value preservation after owner approval.
4. Exercise MIDI Learn, manual mapping, conflict, cancel, unmap and reset.
5. Run keyboard navigation, scaling, tooltip and hover review on approved display configurations.
6. Compare shared editor semantics across wrappers when host/component decisions are approved.

**Oracle/evidence:** UI snapshots, view-model assertions, controller event/mapping logs, accessibility checklist and cross-wrapper comparison. Generic controller reachability is tested without requiring named templates or MIDI feedback.

### 4.8 Non-functional and release suite

**Requirements:** GS-NFR-004/005/009/011/013.

**Fixture:** approved reference machine, build manifest, seeded offline-render harness, long sample, component/test inventory and signed release candidate.

**Sequence:**
1. Account for algorithmic versus reported/measured latency.
2. Repeat fixed-seed renders and compare same-platform outputs.
3. Run independent RNG instances and verify stream isolation.
4. Analyze a five-minute stereo sample while observing UI progress and responsiveness.
5. Audit each implemented DSP component against required automated test families.
6. Validate macOS minimum, architecture, hardened runtime, signing and notarization after policy approval.

**Oracle/evidence:** latency report, deterministic render comparison, RNG isolation trace, UI responsiveness evidence, coverage inventory and release validation report. CPU/RAM/latency thresholds require explicit reference-machine approval.

## 5. Common test-case record

Every eventual formal acceptance case should include:

- **Requirement trace:** canonical GS ID, relevant DEC/IF/AC references and any cross-cutting rationale.
- **Preconditions:** build/format, host/OS, state, patch, sample, device, controller and resource conditions.
- **Stimulus:** exact input events, parameter values, timing, state transition or failure injection.
- **Expected result:** observable state/output and explicit negative assertions (what must not change).
- **Pass condition:** binary or numeric oracle sourced from an approved requirement/decision; numeric tolerance must cite its authority.
- **Evidence:** machine-readable event/state log plus render/UI/host artifact as applicable.
- **Failure classification:** product behavior, host limitation, test harness, resource gate, unresolved decision or inconclusive evidence.
- **Reproducibility:** deterministic seed, fixture hash, build hash and environment manifest where applicable.

A test with a missing expected result, unpinned fixture or unresolved behavior is **design-incomplete**, not pass-ready.

## 6. Decision and measurement gates

| Gate | Affected families | Required action before pass criteria are frozen |
|---|---|---|
| AR-01 | GS-GRN-019, GS-SEQ-002, same-offset parameter/event writes | Resolve writer precedence and same-offset tie-break behavior. |
| AR-02 | GS-ARC-002/004, GS-GRN-017, GS-VOI-001 | Resolve deterministic voice/grain overload policy; keep per-voice ceiling distinct from global budget. |
| Q-T03 | GS-PLT-002, GS-UI-004, GS-NFR-013 | Approve macOS minimum, supported architectures and release-signing policy. |
| Q-T07 | GS-ARC-004/008, GS-GRN-020, GS-SMG-019, GS-UI-005, GS-NFR-009 | Select reference hardware and separately authorize benchmark/resource measurement. |
| Q-T12 | GS-UI-002/007, GS-NFR-006, mapping/parameter suites | Approve parameter identity/exposure model before parameter registry acceptance. |
| Q-T17 | GS-PLT-003, GS-GRN-002, sample-rate-sensitive suites | Approve supported sample-rate and block-size matrix. |
| Q-P09 | GS-ARC-006, GS-SMG-002, GS-UI-006, MPE/host suites | Approve host/format validation matrix and any host prototype scope. |
| AR-03…AR-05 / AR-06 | MPE identity and state restore suites | Resolve owner/host-specific identity, expression and restore policy gates before final pass conditions. |
| Q-M03 / Q-M04 | GS-GRN-009/011 | Q-M03 near-zero behavior is approved by DEC-043. Q-M04 mode count is approved by DEC-045: exactly Bounce, Wrap and Kill; One-Shot is a separate behavior per DEC-044. DEC-047 defines Kill at the terminal sample-region boundary: no new grains, immediate termination of active grains in that layer, no global Panic or transport-state change. Explicit Stop/Panic command semantics remain separate. |
| Q-M14 | GS-GRN-014 | Trigger semantics approved by DEC-048; verify zero/nonzero held-note transitions, overlapping Note Ons, Note Off independence and grain-clock independence. |
| Q-M09 | GS-GRN-018; GS-ARC-008 | Normalization law approved by DEC-046 (1/√N); define counting domain/smoothing and the default four-layer headroom oracle. Density-17 benchmark matching requires authorized reference measurement. |
| Q-M14 / Q-M16 | GS-GRN-014 | Confirm the R1 Off/On/Legato trigger semantics and define Voice Sync behavior without assuming that benchmark labels are equivalent. |
| Q-M17 | GS-GRN-015 | Define the four spawn-mode state transitions, reset points and recycle behavior before writing expected-result assertions. |
| Q-M18 | GS-VOI-006 | Reconcile the stated 0.1 ms minimum envelope stage with the still-proposed benchmark ambiguity; keep the R1 requirement and source question consistent. |
| Q-I07 | GS-VOI-008 | Confirm the R1 filter-mode inventory and exact relationship between LPF/HPF modes, routing, slopes and bypass; do not infer an undocumented SVF mode list. |
| Q-I08 | GS-GRN-006 | Define the centre-weighted pan-spray distribution mathematically and specify seeded statistical tolerance alongside the uniform mode. |
| Q-I12 | GS-VOI-010 | Decide the R1 sub-oscillator waveform/default or explicitly narrow the requirement so the test oracle is unambiguous. |
| Effects parameter contract | GS-FX-002/004/006 | Specify Delay units/ranges/divisions/sync, Chorus/Compressor/Saturator/Reverb ranges and defaults, EQ3 crossover ordering/invalid-value behavior, send range and effect-specific automation. |
| Modulation destination inventory (MTX-05) | GS-MOD-005 | Reconcile the exact eligible R1 continuous-destination inventory against the R1 parameter catalogue; exclude deferred-only destinations. |
| Sampler interpolation contract | GS-SMP-003 | Specify available interpolation modes, default, filter length and transition behavior; keep CPU qualification separate from correctness. |
| Audio I/O and recording contract | GS-SMG-001/008/009/012/020 | Close decoder/resampling choices, format-specific live-input routing, recording-buffer cap and layer/master tap equivalence; Q-T04/Q-T16/Q-P06/Q-T07 remain relevant. |
| MIDI protocol contract | GS-MID-002/003/007/008 | Specify MPE zone/member-channel setup, host expression delivery, NRPN timeout/pair handling and 7/14-bit precedence; retain AR-03…AR-05 host gates. |
| Preset/state contract | GS-PRS-004/006; GS-NFR-012 | Specify serialization schema, atomic save/recovery, version migration, embedded-audio admission and corrupt/missing-asset recovery; Q-T15/state-size gates remain open. |
| R1 domain-spec gaps | GS-GRN-006/008/012/013, GS-CHD-003/004, GS-VOI-004/006/009, GS-SMG-004, GS-NFR-004/005/009 | Make the referenced algorithm, curve, timing, overlap or measurement semantics explicit in the authoritative specification. |
| R1 scope guard | Q-M15, Q-I16, Q-I17, Q-I18 | Confirm these benchmark-only questions do not silently expand R1: the current R1 model does not require per-layer time-signature emulation, keyboard-position voice-steal heuristics or Elektron-style preset switching; DEC-032 defines the R1 XY pad. Update OPEN_QUESTIONS to mark non-R1 items deferred/resolved. |

### Measurement policy

- Product-defined parameter ranges and counts may be tested as binary boundary assertions.
- Statistical behavior requires a fixed seed, a declared sample count and an approved tolerance; none is inferred here.
- Audio discontinuity, latency, CPU, memory and UI responsiveness require a named measurement method and approved threshold.
- Correctness tests may be designed independently of performance qualification where the dependency allows it.
- No benchmark, host prototype or performance claim is authorized by this design document.

## 7. Proposed execution order

1. **Specification closure:** resolve or explicitly defer AR-01/02, Q-T03/07/12/17, Q-P09 and the domain-spec gaps above.
2. **Fixture freeze:** approve sample assets, MIDI/event scripts, reference vectors, seeds, build manifest and reference machine.
3. **Model-level tests:** parameter boundaries, state machines, allocators, chord/pitch mapping, sequencer model and deterministic event scheduling.
4. **Offline DSP tests:** windows, interpolation, routing, grain behavior, envelopes, filters and deterministic render comparisons.
5. **UI/controller tests:** accessibility, generic controller-only reachability, MIDI Learn, A–H mapping after owner approval, waveform/metrics snapshots.
6. **Host/device integration:** only after host matrix and prototype authorization.
7. **Performance qualification:** only after reference hardware and numeric thresholds are approved.
8. **Acceptance audit:** reconcile all 124 R1 IDs to formal AC links or explicit cross-cutting rationale; record actual evidence and pass/fail status.

## 8. Exit criteria for this design pass

- [x] All 71 unlinked R1 requirements are represented in the Phase 5 backlog.
- [x] Each of the 71 is assigned to a shared test family and an observable candidate oracle in §3.
- [x] Shared fixture and execution patterns are specified for the eight suite families.
- [x] Unresolved decision, host, domain-spec and measurement gates are explicit, including R1 scan, normalization, trigger/spawn, filter, pan-spray and sub-oscillator details.
- [ ] Owner approves the test-design blueprint and resolves/delegates the listed product gates.
- [ ] Formal AC IDs and pass/fail wording are added in the authoritative acceptance document.
- [ ] Reference environment, fixtures, numeric thresholds and CI execution plan are approved.
- [ ] Tests are implemented/executed and evidence is recorded; none is claimed here.

**Conclusion:** The 71 requirement gaps now have a structured test-design path and candidate observable oracles. They do not yet have 71 approved formal acceptance criteria, numeric thresholds or passing test results. Phase 5 remains a planning/design activity and does not authorize Phase 6 implementation.

# Phase 5 Test-Design Backlog — R1 Acceptance Traceability

**Version:** 0.4 — R1 semantic gates reconciled, 2026-11-28
**Status:** Planning backlog only. No new product decisions, acceptance IDs, numeric thresholds, implementation, host prototype or benchmark is authorized by this document.
**Authority:** `GRANULAR_SYNTH_REQUIREMENTS.md`, `R1_PRODUCT_SPECIFICATION.md`, approved DEC records, `R1_ACCEPTANCE_CRITERIA.md`, and Phase 3 interface/architecture contracts.

## 1. Purpose

The companion `PHASE5_R1_ACCEPTANCE_TEST_DESIGN.md` assigns all 71 unlinked requirements to shared test families, adds candidate observable oracles, fixture sequences, evidence rules and decision/measurement gates. This backlog remains the canonical requirement-level intent/evidence/dependency register; the companion document does not create formal AC IDs or claim execution.

This backlog converts every R1 requirement that still lacks an acceptance link into a Phase 5 test-design task. Each row retains the canonical requirement ID, identifies the observable behavior to validate, suggests evidence suitable for the test family, and records unresolved dependencies. Rows may later be grouped into shared suites, but each requirement must remain individually traceable. Candidate evidence is not a pass result, and a candidate API symbol is not acceptance coverage.

## 2. Corrected traceability baseline

| Measure | Count |
|---|---:|
| Canonical R1 requirement IDs | 124 |
| Requirements explicitly referenced by an AC table row | 49 |
| Additional requirements covered by cross-domain release gates in `R1_ACCEPTANCE_CRITERIA.md` §7 | 4 |
| **Requirements with an existing acceptance/release-gate link** | **53** |
| **Requirements requiring a new explicit AC link or a documented cross-cutting rationale** | **71** |

The earlier 47/77 count was understated because shorthand references in the acceptance tables were not expanded consistently. The corrected 49 direct references include `GS-MOD-002`, `GS-MOD-005`, `GS-MOD-007`, `GS-PRS-008`, `GS-SMG-010` and `GS-SMG-011`. The four separate §7 release-gate links are `GS-PLT-004`, `GS-NFR-002`, `GS-NFR-006` and `GS-NFR-007`. The 71 rows below have neither a direct AC-table reference nor one of those release-gate links.

## 3. Test-design rules

- Do not assign new `AC-*` IDs in this backlog. Phase 5 owns formal AC numbering and pass/fail wording.
- Preserve exact product scope and approved decisions. Deferred engines/features (Slice, Tape, Spectral, spectrogram, MIDI feedback and named-controller templates) are not promoted into R1.
- Prefer one shared test suite where technically appropriate, but retain a requirement-level assertion and trace for every row.
- Separate functional correctness from performance qualification. Numeric CPU/RAM/latency/headroom thresholds require a reference machine and explicit approval; no benchmark is authorized here.
- Where an unresolved decision controls observable behavior, record the dependency and do not freeze a pass condition until that decision is resolved.
- Evidence types are proposals: unit/property, deterministic offline render, host/device integration, state round-trip, manual UI/controller review, stress/resource, and release/package validation.

### Platform and host integration

| Requirement | Candidate test-design intent | Candidate evidence | Dependency / gate |
|---|---|---|---|
| GS-PLT-002 | Universal 2 package contains the approved Intel and Apple Silicon slices; verify binary architecture metadata and packaging consistency. | Build/package matrix; architecture inspection of produced artifacts. | Q-T03: minimum macOS and architecture/package policy must be finalized; no deployment target is selected here. |
| GS-PLT-003 | Exercise the approved sample-rate and fixed/variable block-size matrix, including transitions between negotiated sizes. | Host/device integration matrix; render continuity; VST3 parameter-flush calls without audio buses; AU maximumFramesToRender allocation/reconfiguration; callback-context evidence. | Q-T17: 44.1–192 kHz and 16–4096 are proposed values, not yet an approved support commitment. |

### Layer architecture, voice/grain capacity and routing

| Requirement | Candidate test-design intent | Candidate evidence | Dependency / gate |
|---|---|---|---|
| GS-ARC-001 | Prove four layers retain independent patch, voice, modulation, FX and MIDI-channel state while rendering together. | Multi-layer integration test; state-isolation assertions and routed render captures. | DEC-017 fixes four independent layers; DEC-034 requires all layers sharing a MIDI channel to receive matching events. |
| GS-ARC-002 | Verify per-layer polyphony configuration boundaries from 1 through 20 voices and isolation between allocators. | Allocator unit/property tests plus multi-layer integration. | No unresolved product decision; overload policy for simultaneous pressure is linked to AR-02. |
| GS-ARC-003 | Verify the per-voice ceiling of 128 simultaneous grains, including admission/release at the boundary. | Grain-pool boundary tests; active-count and ownership assertions. | Keep per-voice limit distinct from the global budget in GS-ARC-004. |
| GS-ARC-004 | Verify deterministic global grain-budget admission and graceful degradation across all four layers. | Deterministic stress/overload test; event trace and audio evidence. | AR-02 and Q-T07 remain open; numeric budget and measured degradation thresholds require separate benchmark authorization. |
| GS-ARC-006 | Verify one stereo master plus four optional stereo layer outputs, activation/deactivation and no duplicate summing. | AU/VST3/Standalone host matrix; bus-map and captured-output evidence, including inactive null buffers, omitted trailing inactive buses, default-active not honored and no duplicate summing. | Q-P09 host matrix and AU component strategy remain open; host-specific prototype requires explicit authorization. |
| GS-ARC-007 | Verify the complete signal path: per-layer wet input/FX plus parallel dry live-input sum before master, and the specified voice filter order. | Deterministic routing render with distinct test signals on each path; tap comparison. | Preserve approved routing; layer stems are post-FX/pre-layer-fader and master capture is post-mix/pre-output. |
| GS-ARC-008 | Verify default voice gain and grain-count normalization prevent clipping with four dense layers under the defined default patch state. | Peak/RMS render analysis over representative signal fixtures. | Phase 5 must define fixtures and headroom/pass thresholds; do not infer a numeric margin now. |

### Granular engine

| Requirement | Candidate test-design intent | Candidate evidence | Dependency / gate |
|---|---|---|---|
| GS-GRN-001 | Verify grain-rate range 0.1–1000 grains/s per voice, including boundaries and stable scheduling. | Parameter-boundary tests and deterministic scheduler traces. | No additional owner gate identified; global capacity behavior remains AR-02. |
| GS-GRN-002 | Verify grain-size range 0.1–5000 ms and conversion to frames across supported sample rates. | Parameter-boundary/property tests; frame-count calculations. | DEC-035 fixes the product range; sample-rate matrix depends on Q-T17. |
| GS-GRN-003 | Verify the live density display equals the defined rate × size calculation and updates when either input changes. | Model/property tests plus UI observation snapshots. | Phase 4 defines visual presentation; do not invent display rounding or refresh cadence in Phase 3. |
| GS-GRN-004 | Verify position resolution is at least 14-bit while the internal representation retains double precision. | Numerical precision/property tests across the normalized position range. | No additional owner gate identified. |
| GS-GRN-005 | Verify spray spans the sample with bipolar, positive-unipolar and negative-unipolar polarity modes. | Seeded property tests and render captures at endpoints/intermediate values. | Phase 5 must define statistical tolerances for random distributions. |
| GS-GRN-006 | Verify pan, pan spray and at least uniform and centre-weighted pan-spray distributions. | Seeded statistical tests and stereo render analysis. | Distribution names are specified; exact centre-weighted curve and statistical tolerance need Phase 5 definition. |
| GS-GRN-007 | Verify per-layer mid-side width behavior, including neutral and boundary settings. | Stereo reference-vector tests and channel-correlation/render analysis. | No additional owner gate identified. |
| GS-GRN-008 | Verify reverse/forward probability endpoints and the 50/50 midpoint using reproducible seeds. | Seeded statistical/property tests and event/render evidence. | Phase 5 must set sample counts and statistical tolerance. |
| GS-GRN-009 | Verify scan range −2×…+2×, fine and ±0.25-snap modes, and interaction with stop/loop flags. | Parameter-boundary and playback-boundary tests. | Use the canonical sample-region and loop semantics; no new behavior is inferred. |
| GS-GRN-010 | Verify coarse/fine pitch ranges, pitch-spray polarity and semitone/chord-note snapping. | Pitch mapping/property tests and deterministic render captures. | Chord snapping must use the same approved scale/chord model as GS-CHD-005. |
| GS-GRN-011 | Verify all six ratio modes and explicitly test the documented behavior at scan = 0. | Mode-by-mode property tests and deterministic render comparisons. | The scan-zero rule must be explicit in the source specification before a pass condition is frozen. |
| GS-GRN-012 | Verify PowAR and Raised Cosine window families and their exposed sides/tilt/curve/AM controls. | Window-function numerical tests, reference vectors and render captures. | Phase 5 defines acceptable numerical error; no proprietary reference implementation is assumed. |
| GS-GRN-013 | Verify changing/modulating a window never introduces an audible discontinuity in grains already playing. | Active-grain transition tests under rapid parameter modulation; waveform/discontinuity evidence. | GS-NFR-003 applies; Phase 5 must define the measurement method and threshold. |
| GS-GRN-014 | Verify Off/On/Legato key trigger, straight/triplet/dotted clock divisions from 1/64 through 64/1, and voice-clock sync. | Event-scheduler tests against a deterministic transport timeline. | Transport authority must follow IF-08; host/MIDI clock arbitration remains a separate adapter gate. |
| GS-GRN-015 | Verify Gradual, Direct, Direct-reset and Recycle spawn modes, including their reset/recycle boundaries. | Deterministic scheduler state-transition tests and event traces. | Exact transition semantics must be made explicit in the detailed engine specification before final pass criteria. |
| GS-GRN-016 | Verify local-pitch ON captures birth pitch and OFF follows the specified live pitch behavior. | Per-grain state tests with pitch changes after grain creation. | No additional owner gate identified. |
| GS-GRN-017 | Verify grain-stealing threshold OFF/active behavior and click-safe retirement when a grain is reclaimed. | Overload tests with deterministic candidate/event traces and audio discontinuity evidence. | AR-02 remains open; do not select threshold, comparator policy or fade duration in this backlog. |
| GS-GRN-018 | Verify grain-count normalization ON/OFF and gain behavior as active grain count changes. | Deterministic multi-grain render sweeps and gain/peak analysis. | Phase 5 defines reference inputs and acceptable gain deviation. |
| GS-GRN-019 | Verify grain onset placement at the required sub-sample accuracy and preserve exact event offsets through block segmentation. | Sample-offset scheduler tests and high-resolution render/event traces. | AR-01 event ordering remains open; define the observable meaning of sub-sample accuracy before implementation acceptance. |
| GS-GRN-020 | Verify Linear and cubic-Hermite minimum interpolation correctness, bounds safety and operation at the required full-polyphony load. | Numerical reference tests plus full-load performance evidence. | Reference hardware and performance thresholds require Q-T07 and explicit benchmark authorization; correctness tests can be designed separately. |

### Chord, voice and performance behavior

| Requirement | Candidate test-design intent | Candidate evidence | Dependency / gate |
|---|---|---|---|
| GS-CHD-001 | Verify chord mode in every R1 engine, 12 slots, per-slot fine tune, duplicate notes and lowest-note root behavior. | Chord-model property tests and MIDI/render captures per engine. | R1 engine set is Granular and Sampler only; deferred engines are excluded. |
| GS-CHD-002 | Verify granular chord distribution across grains at constant level and Sampler voice stacking behavior. | Deterministic render comparison and per-note/per-grain allocation trace. | Do not imply a non-R1 engine behavior test. |
| GS-CHD-003 | Verify all ten specified granular arpeggiator modes, including deterministic handling of random and shuffle modes. | Seeded event-sequence tests and transport-boundary tests. | Random/shuffle expected sequences must be specified by the sequencer design before freezing golden fixtures. |
| GS-CHD-004 | Verify chord recording captures the played notes and restores the captured chord without note loss or duplication. | MIDI input capture and state round-trip tests. | Phase 5 should define behavior for overlapping/held notes if not already specified in the chord domain spec. |
| GS-CHD-005 | Verify preset and user scales, per-voice/per-grain quantization and sub-oscillator quantization. | Pitch-mapping property tests and deterministic render captures. | Use one canonical scale/pitch mapping contract across granular, voice and sub-oscillator paths. |
| GS-VOI-001 | Verify voice stealing prefers released, non-sustained, oldest voices and uses the required fast fade rather than abrupt termination. | Allocator ordering tests and audio transition captures under voice pressure. | AR-02 remains open for interaction with global grain reclamation; no fade duration or capacity is selected here. |
| GS-VOI-002 | Verify CC64 sustain/hold behavior across note-off, pedal release and channel/layer routing. | MIDI event-state tests and voice-lifetime traces. | Follow the approved MIDI channel-sharing rule where layers share a channel. |
| GS-VOI-003 | Verify glide from 0–2000 ms and Always mode across note transitions. | Pitch trajectory tests and deterministic render captures. | Phase 5 defines trajectory tolerances and retrigger scenarios. |
| GS-VOI-004 | Verify velocity sensitivity 0–100% and the configured velocity curve across the input range. | Parameter/property tests and MIDI-to-amplitude response captures. | Exact curve families/defaults must come from the approved voice specification. |
| GS-VOI-005 | Verify pitch-bend range 0–48 semitones and transpose in semitones plus cents. | MIDI/pitch mapping tests at range boundaries and combined bend/transpose cases. | MPE per-note 48-semitone behavior remains DEC-038; do not conflate channel and per-note expression. |
| GS-VOI-006 | Verify four ADSR envelopes, 0.1 ms–45 s stage ranges, stage curves, bipolar amount and inversion. | Envelope stage/property tests and deterministic control-rate/audio-rate renders. | Phase 5 defines timing tolerance at short/long extremes. |
| GS-VOI-008 | Verify per-voice LPF/HPF ranges, resonance, 6/12/18/24 dB slopes, routing/bypass and LPF→HPF order. | Filter response/reference tests and routing assertions. | Preserve the approved filter order; exact response tolerances are Phase 5. |
| GS-VOI-009 | Verify filter key tracking across note range and configured tracking response. | Pitch-to-cutoff mapping/property tests and response captures. | Tracking curve/range must be explicit in the voice/filter specification. |
| GS-VOI-010 | Verify per-voice sub-oscillator −1/−2 octave, level, poly balance and quantize option. | Pitch/level tests and deterministic render captures. | Quantized behavior must match GS-CHD-005. |

### Sample management and recording

| Requirement | Candidate test-design intent | Candidate evidence | Dependency / gate |
|---|---|---|---|
| GS-SMG-002 | Verify drag/drop from Finder and supported DAWs plus in-app browser audition preview. | Standalone and host UI integration; import/audition event and audio evidence. | Phase 4 UI workflow and Q-P09 host matrix are prerequisites for final acceptance. |
| GS-SMG-003 | Verify sample loading never blocks/glitches audio and keeps the previous valid sample active until the replacement is prepared and published. | Concurrent load/render test, generation trace and audio continuity evidence. | Use IF-03/IF-05 ownership and retirement contracts; no decoder or file I/O on the audio thread. |
| GS-SMG-004 | Verify optional normalize-on-import behavior and that disabled normalization leaves source amplitude unchanged. | Import fixture tests comparing source and imported sample peak/metadata. | Normalization target/algorithm must be explicit in the sample-management specification. |
| GS-SMG-005 | Verify waveform zoom/scroll and visible voice/grain positions, flags, slices and record head. | UI snapshot/view-model tests plus manual visual review. | Spectrogram is L2 and must not be pulled into R1 acceptance. |
| GS-SMG-006 | Verify start/stop/loop flags and one-shot/loop/bounce scan modes, key trigger modes and clock sync. | Sample-region state tests and deterministic playback-boundary tests. | Use the shared transport contract and approved Sampler loop-release behavior. |
| GS-SMG-007 | Verify crop variants, soft-cut crossfade and normalize remain non-destructive and fully undoable. | Edit-history state tests and before/after audio comparisons. | Phase 4 defines interaction; source asset immutability and undo ownership follow IF-05/IF-11. |
| GS-SMG-019 | Verify the DEC-028 target of at least six minutes stereo at 48 kHz per layer without unsafe memory admission or corruption. | Memory-admission/soak evidence and decoded-frame integrity checks. | Target remains conditional on safe feasibility; Q-T07/reference machine and explicit benchmark authorization are required before performance pass thresholds. |

### Sequencer

| Requirement | Candidate test-design intent | Candidate evidence | Dependency / gate |
|---|---|---|---|
| GS-SEQ-001 | Verify one central sequencer controls four layers, variable pattern length/resolution and up to eight simultaneous events per step with event-level data. | Pattern-model property tests, event scheduling traces and four-layer render captures. | DEC-019 approved; resource-bound limits must not be converted into arbitrary product caps. |
| GS-SEQ-002 | Verify modulation lanes and parameter locks/overrides, including live-edit application semantics. | Event/parameter-writer tests and deterministic timeline captures. | AR-01 must resolve writer precedence for host automation, MIDI, sequencer locks and UI gestures. |
| GS-SEQ-003 | Verify straight/triplet grids, per-pattern quantization, microtiming, Swing and Humanize. | Deterministic timing-grid property tests and event-position traces. | Randomized Humanize behavior must use seedable per-instance RNGs. |
| GS-SEQ-004 | Verify pattern save/recall, end-of-bar switching, chaining and ordered Performance Sets. | State round-trip and transport-boundary integration tests. | No fixed product-level count cap; technical resource limits require profiling and must be reported explicitly. |
| GS-SEQ-008 | Verify per-layer latched play/drone mode and panic behavior under normal and failure conditions. | MIDI/control integration tests and bounded panic-response evidence. | Panic semantics must remain distinct from Freeze state; see AC-AUD-025. |
| GS-SEQ-009 | Verify layer mix and per-layer position controls are available and respond during performance. | Controller/parameter integration tests and layer-output captures. | Controller reachability is governed by DEC-016/021 and GS-MID-008. |
| GS-SEQ-011 | Verify global latched Fill with continuous intensity, granular-character changes without rhythm changes, and immediate layer/step mute behavior. | Sequencer state/event tests and paired render comparisons. | Individual event unmute timing must follow the approved product rule in R1_PRODUCT_SPECIFICATION.md §4.3. |
| GS-SEQ-012 | Verify live pattern editing, bar-end activation, explicit Save, temporary Draft restore, Undo/Redo and latest-saved-version-only persistence. | Pattern state-machine tests, save/recall round trips and undo-history evidence. | No persistent version archive is required; preserve the approved temporary Draft behavior. |

### Presets, UI and non-functional behavior

| Requirement | Candidate test-design intent | Candidate evidence | Dependency / gate |
|---|---|---|---|
| GS-PRS-009 | Verify comments are stored and recalled per patch without leaking across unrelated patches or overwriting other preset state. | Preset state round-trip and isolation tests. | Phase 4 defines where comments appear; do not add collaboration or cloud-sync scope. |
| GS-UI-001 | Verify the main view presents the sample waveform and live grain visualization using non-blocking snapshots. | UI integration/manual review plus snapshot sequence evidence. | Detailed layout and visual acceptance belong to Phase 4; audio thread must not expose mutable runtime pointers. |
| GS-UI-002 | Verify the eight-knob engine section remaps by selected engine and remains usable with generic MIDI mapping. | UI/controller mapping integration tests for Granular and Sampler. | No named-controller template or MIDI feedback is required in R1 (DEC-021). |
| GS-UI-003 | Verify window-shape visualization corresponds to the active window parameters and displays the density equation correctly. | View-model/property tests plus visual review. | Share the same density calculation as GS-GRN-003; Phase 4 defines formatting/rounding. |
| GS-UI-004 | Verify resizing, HiDPI scaling, keyboard navigation, tooltips and hover value readout across supported display configurations. | Manual accessibility/usability matrix and UI automation where reliable. | Supported OS/display matrix depends on Q-T03; detailed UI acceptance is Phase 4. |
| GS-UI-005 | Verify CPU meter reports voices, FX and housekeeping categories from stable snapshots without blocking rendering. | Metric snapshot tests and UI freshness review. | No CPU budget or performance threshold is implied; Q-T07 remains gated. |
| GS-UI-006 | Verify Standalone and plugin editors use the same shared editor component and preserve semantic behavior across wrappers. | Architecture/component identity review plus cross-format UI integration evidence. | AU/VST3 host matrix and component strategy remain open; no plugin shell is authorized by this backlog. |
| GS-UI-007 | Verify global/per-preset MIDI Learn mappings, conflict detection, reassignment, reset/unmap recovery and controller-only essential operations. | Mapping model/state tests, UI workflow review and controller-only scenario evidence. | DEC-021 excludes named-controller templates and MIDI feedback; Phase 4 mapping UX is a prerequisite. |
| GS-NFR-004 | Verify R1 Granular and Sampler report and introduce no algorithmic added latency under the approved process model. | Latency accounting and impulse/loopback evidence in supported host/device modes. | Do not include deferred Slice/Tape/Spectral in R1 acceptance; definition of reported versus measured latency must be explicit. |
| GS-NFR-005 | Verify all randomness is seedable per instance and fixed-seed offline renders are reproducible. | Deterministic offline-render comparison and RNG isolation tests. | Cross-architecture bit-identical behavior is not assumed; clarify GS-PLT-004/FP policy before cross-architecture claims. |
| GS-NFR-009 | Verify a five-minute stereo sample can decode/analyze without UI freeze and waveform peaks become visible progressively. | Import/load integration test with progress/snapshot evidence and UI responsiveness observation. | Phase 5 must define a measurable responsiveness criterion and reference machine; no benchmark is authorized here. |
| GS-NFR-011 | Verify every implemented DSP component is registered with automated tests and required test families. | Component-to-test inventory audit and CI evidence. | This is a coverage/meta-test obligation; component inventory is finalized during implementation, not in this documentation-only pass. |
| GS-NFR-013 | Verify minimum macOS support, hardened-runtime signing and notarized release artifacts. | Release packaging/signing validation matrix. | Q-T03 and distribution/legal gates remain open; do not choose a minimum OS or signing workflow here. |

## 5. Cross-cutting decisions and evidence gates

| Gate | Affected requirements / families | Required before final acceptance design |
|---|---|---|
| Q-T03 | GS-PLT-002, GS-NFR-013, GS-UI-004 | Confirm minimum macOS, architecture/package support and the target signing/notarization environment. |
| Q-T17 | GS-PLT-003, GS-GRN-002 and sample-rate-sensitive behavior | Confirm supported sample-rate and buffer-size matrix; do not convert proposed values into a support promise before approval. |
| Q-P09 | GS-ARC-006, GS-SMG-002, GS-UI-006 and host-specific validation | Approve target hosts/versions and the AU/VST3 component/bus validation matrix. |
| AR-01 | GS-GRN-019, GS-SEQ-002 and same-offset parameter/event behavior | Resolve deterministic same-offset ordering and writer precedence before final event-level pass conditions. |
| AR-02 | GS-ARC-002, GS-ARC-003, GS-ARC-004, GS-GRN-017, GS-VOI-001 | Resolve deterministic voice/grain overload policy; keep voice stealing distinct from global grain-budget reclamation. |
| Q-T07 | GS-ARC-004, GS-GRN-020, GS-SMG-019, GS-UI-005, GS-NFR-002, GS-NFR-009 | Establish reference hardware and obtain explicit benchmark authorization before numeric performance thresholds or claims. |
| Q-T12 | GS-NFR-006, GS-MOD-005, GS-UI-002, GS-UI-007 | Approve stable host parameter-ID/exposure scheme before freezing parameter registry coverage. |
| Q-M03 / Q-M04 | GS-GRN-009/011 | Define scan modes and the ratio-mode behavior at scan = 0; current proposals are not approved test oracles. |
| Q-M09 | GS-GRN-018, GS-ARC-008 | Define the grain-count normalization law and the default four-layer headroom oracle; numeric matching requires an authorized reference measurement. |
| Q-M14 / Q-M16 | GS-GRN-014 | Confirm Off/On/Legato grain-trigger semantics and define Voice Sync behavior without assuming benchmark label equivalence. |
| Q-M17 | GS-GRN-015 | Define the four spawn-mode state transitions, reset points and recycle behavior. |
| Q-M18 | GS-VOI-006 | Reconcile the 0.1 ms envelope-stage minimum in the R1 requirement with the still-proposed benchmark ambiguity. |
| Q-I07 | GS-VOI-008 | Confirm R1 filter-mode inventory and the relationship between LPF/HPF modes, routing, slopes and bypass. |
| Q-I08 | GS-GRN-006 | Define centre-weighted pan-spray distribution and the statistical oracle alongside uniform distribution. |
| Q-I12 | GS-VOI-010 | Decide the R1 sub-oscillator waveform/default or explicitly narrow the requirement. |
| R1 scope guard | Q-M15, Q-I16, Q-I17, Q-I18 | Keep benchmark-only time-signature behavior, keyboard-position voice-steal heuristics and Elektron-style preset switching out of R1 unless the owner adds scope; DEC-032 defines the R1 XY pad. |
| DEC-042 mixed-layer Global Freeze preparation-failure policy | GS-SMG-021 and AC-AUD-028 | DEC-042 Source Snapshot product semantics are approved. Owner confirmation remains required only for successful-subset versus all-or-nothing behavior when preparation fails for one or more layers. Existing AC-AUD-028 is explicitly proposed pending review. |

## 6. Exit criteria for this backlog

- All 71 rows have either a new requirement-specific AC/test link or a precise documented rationale pointing to an existing cross-cutting criterion.
- Every pass condition has an observable result, fixture/setup, evidence artifact and failure classification.
- Numeric thresholds are sourced from an approved requirement or approved Phase 5 measurement plan; no values are guessed.
- All owner/engineering gates above are resolved or explicitly recorded as deferred with their effect on release scope.
- Requirement/API/interface/test traceability is reconciled in one audit, with no false claim that a planned test has passed.

## 7. Current boundary

This document is a planning artifact only. It does not authorize DSP implementation, AU/VST3 Plugin Shells, host prototypes, benchmarks, framework selection, deployment-target selection, parameter-ID freeze or PR merge.


## 5. Cross-format event and transport parity supplements

These shared test families supplement the requirement-level backlog above. They do not change the corrected 53/124 acceptance-link count, add AC IDs, or claim passing results. A shared suite may cover both linked and currently unlinked requirements, but each requirement remains individually traceable.

| Test family | Candidate fixture / assertions | Requirement and dependency |
|---|---|---|
| AU absolute event time | Valid/invalid block sample-time anchor; signed AU event times; immediate scheduling; negative host sample time; exact offsets 0 and final sample; future/stale handling without clamping | GS-MID-001/002/003/005/006; GS-PLT-003 |
| VST3 discrete event time | Block-relative offsets; busIndex/channel; source-local order; Note On before Note Expression; missing noteId; event-list ordering | GS-MID-001/002/003/005/006; DEC-038 |
| AU/VST3 automation equivalence | AU step/ramp and non-rampable behavior; AU declared range/unit conversion; VST3 normalized [0,1] queue points, omitted initial point, previous-block anchor, constant-slope continuation and two-point jumps; compare canonical values | GS-MOD-005; GS-SEQ-009; Q-T12/Q-T13 |
| MPE identity bridge | VST3 noteId versus AU cable/member-channel lifecycle; overlapping notes, same-pitch retrigger, note-off and channel reuse; per-note pitch/pressure/timbre and 48-semitone pitch range | GS-MID-002; GS-MOD-004; DEC-038; AR-03…AR-05 |
| Transport validity and discontinuity | Null/failed context; independent field validity; tempo/time-signature change; stop/start; seek; cycle range and loop wrap | GS-SEQ-001…012; GS-MID-007; DEC-039; Q-P09 |
| Intra-block cycle transition | Compare host that splits at cycle boundary with host that does not; verify explicit unknown/insufficient intra-block timeline when no reliable transition is supplied | GS-SEQ-001…012; host-specific prototype gate |
| VST3 parameter flush | No audio buses with parameter updates; no audio scratch access, frame counter, sequencer or transport advance | GS-PLT-003; GS-NFR-001 |
| Same-offset cross-source batch | Host automation, MIDI mapping, sequencer lock and UI write at identical sample offset; prove deterministic batch capture without asserting a winner | AR-01; GS-MOD-005; GS-SEQ-009 |
| Variable-block partition parity | Render the same event/automation fixture using different legal block partitions; compare absolute event positions, curve continuity and deterministic output | GS-PLT-003/004; GS-NFR-006 |

**Design restrictions:** no numeric event capacity, timing tolerance, overflow recovery or host version is set here. No benchmark is authorized. Formal AC grouping and pass/fail thresholds remain Phase 5 work.


## 6. MPE note identity and lifecycle supplements

These test families refine the earlier MPE identity bridge row. They do not add AC IDs, set numeric capacities or claim host support.

| Test family | Candidate fixture / assertions | Requirement and dependency |
|---|---|---|
| VST3 note-ID admission | Valid ID, -1, duplicate active ID, stale event, ID reuse only after prior lifecycle ends | GS-MID-002; AR-04 |
| VST3 Note Expression lifecycle | Expression before Note On, after Note On, after Note Off during release, after voice completion | GS-MID-002; GS-VOI-001…010 |
| VST3 type discovery | INoteExpressionController declarations per event bus/channel; standard/custom Pressure and Timbre alternatives | GS-MID-002; AR-04; Q-P09 |
| AU cable and zone isolation | Multiple cables, lower/upper zones, missing/invalid zone configuration and zone reconfiguration | GS-MID-002; AR-03 |
| AU member-channel sharing | Multiple active notes on one member channel; expression reaches every active note in that group and no other cable/channel | GS-MID-002; DEC-034 |
| AU same-pitch Note Off | Repeated same-pitch Note On on one member channel, unmatched Note Off and ambiguous match reporting | GS-MID-002; AR-03 |
| AU channel reuse and sustain | Note Off, release tail/sustain, member-channel reuse and proof that old tails do not follow new expression | GS-MID-002; AR-03 |
| Cross-layer expression fan-out | One source note routed to multiple matching layers; each layer voice receives one expression update | GS-MID-001/002; DEC-034 |
| Expression conversion | Pitch-bend minimum/center/maximum; pressure and CC74 endpoints; VST3 normalized-value conversion | GS-MID-002; DEC-038; AR-05 |
| Bounded identity overload | Identity-table saturation, event overflow, stale-generation rejection, panic under overload and no render allocation | GS-MID-002; GS-NFR-001; AR-02 |

Formal AC grouping, pass/fail wording, exact capacity and host-version matrix remain Phase 5 work. No benchmark is authorized.


## 7. MPE host-delivery compatibility supplements

These host-specific fixtures extend the MPE identity tests in §6. They do not add AC IDs, claim host support, change the 124-row inventory or alter the 53/124 acceptance-link baseline.

| Host / format | Fixture | Required evidence |
|---|---|---|
| Logic Pro / AU | MPE controller into a third-party AU that declares MPE support; MIDI Mono Mode-compatible channel-per-note path; base-channel message; configurable pitch range | Component discovery, actual AU event cable/channel bytes, channel-per-note behavior, base-channel scope, pitch-range configuration and min/center/max conversion |
| Ableton Live / AU | `supportsMPE` property true/false; MPE mode on/off; lower/upper zone RPN 6; overlapping notes | Property query, channelized Note On/Off/Pitch Bend/CC74/Channel Pressure, zone changes and member-channel reuse |
| Ableton Live / VST3 | Plugin absent/present in recognized list; manual MPE enable/disable; saved default configuration; lower/upper RPN 6 | Automatic versus manual recognition, actual MIDI event types/channels, CC74/pressure/pitch delivery; prove whether any VST3 Note Expression event is present rather than assuming `noteId` |
| Bitwig Studio / VST3 | Use MPE off/on; Force MPE; PB range configured to ±48; overlapping notes and per-note expression lanes | Separate trace of `NoteExpressionValueEvent`/`noteId` and channelized MIDI 1.0 messages; type IDs, event bus, range and endpoint conversion |
| REAPER / AU and VST3 | Raw multichannel MIDI input, RPN zone configuration, overlapping notes, per-note pitch/pressure/CC74 and VST3 Note Expression probe | Actual event trace, channel preservation, RPN handling, Note Expression presence/absence and signed-semitone conversion |

Cross-host assertions:
- The same fixture must be exercised through both channelized MPE and VST3 Note Expression wherever the host exposes each path.
- Both paths must normalize to the same signed pitch/pressure/timbre semantics without fabricating a host note ID.
- Record whether MPE is auto-recognized, manually enabled or requires vendor allow-listing.
- Verify zone/member-channel count, shared-channel identity, same-pitch retrigger, sustain, channel reuse, and DEC-034 layer fan-out.
- Verify the host's actual pitch-bend configuration at minimum, center and maximum; DEC-038 does not imply that every host defaults to ±48.
- No host prototype, performance benchmark or numeric pass threshold is authorized by this backlog.

## 8. Detailed MPE event normalization and lifecycle fixtures

The broad MPE families in §§6–7 are refined by PHASE3_MPE_EVENT_NORMALIZATION_AND_LIFECYCLE_FIXTURES.md. It supplies deterministic symbolic sequences for VST3 valid/missing/duplicate/reused note IDs; channelized shared-member-channel expression; same-pitch ambiguity; member-channel reuse; cable/event-bus/zone isolation; DEC-034 fan-out; semantic endpoints; event ordering; Panic and bounded identity exhaustion.

The catalogue distinguishes **Invariant verified**, **Invariant failed**, **Decision-gated** and **Host-unverified** outcomes. It creates no formal test IDs or passing results. AR-03…AR-05, Q-P09, exact capacities, formal AC grouping and host execution remain open.

## 9. MPE reset, state restore and host lifecycle

| Requirement / decision | Candidate test-design intent | Evidence | Gate |
|---|---|---|---|
| GS-MID-002 | CC 121 resets controller state without clearing identities; CC 123 requests release and respects sustain; CC 120 terminates only addressed channel scope | Deterministic MIDI fixtures and identity/voice snapshots | MPE reset defaults; AR-03/AR-04 |
| GS-MID-002 / DEC-034 | Zone/cable/event-bus/device reconfiguration invalidates only affected ingress generation; old tails never follow a new lease | Event trace and generation snapshots | AR-03; Q-P09 |
| DEC-025 | Save excludes transient note IDs/active maps while embedding required unsaved recordings | State manifest and round-trip | Q-T15; host callback limits |
| DEC-025 / DEC-042 | Valid restore is atomic and preserves Freeze/recordings; invalid restore preserves previous state | State round-trip, corruption and memory-admission fixtures | Q-T15; host prototype |
| GS-NFR-001 | AU reset and VST3 processing stop/start invalidate stale identities without render allocation/blocking/destruction | RT instrumentation and lifecycle trace | AU caller context; VST3 call sequence |
| DEC-025 / AR-06 | Restore with held notes/release tails cannot alias old identities into new state; verify selected audible tail policy | Host state-restore tests with noteId/member-channel reuse | AR-06 owner decision; Q-P09 |
| DEC-025 | VST3 component state precedes controller component-state and controller-specific state; no duplicate host writes | Processor/controller state trace | Steinberg persistence sequence |
| DEC-042 | CC 120/Panic and host reset do not thaw Freeze or erase snapshot | Freeze/recording state round-trip | DEC-042; recorder behavior across deactivation remains host validation |

No formal AC IDs, test IDs, capacities, pass thresholds or results are created. No benchmark or host prototype is authorized by this backlog.


## 10. Cross-host state persistence and reset evidence fixtures

These fixtures extend §9's MPE reset/restore backlog. They are cross-cutting host-integration tests, not new R1 requirements or formal AC IDs. The evidence baseline remains 124 requirements / 53 existing acceptance-or-release-gate links / 71 requirements needing explicit links or cross-cutting rationale.

| Host / format | Candidate test family | Required evidence | Dependency / gate |
|---|---|---|---|
| Logic Pro / AU | Project plug-in-state save/reopen; plug-in setting save/load and Reset Setting; AU render reset/deallocation; optional-bus activation after reopen | Host build, AU identifier, state manifest/hash, callback trace, active bus/routing snapshot | Q-P06/Q-P09; AU host prototype; custom payload U |
| Ableton Live / AU | Live Set save/reopen; AU preset round-trip; DEC-025 recording and DEC-042 Freeze payload; missing external asset/relink; Collect All and Save distinction | Set/project artifact, plug-in state payload hash, relink result, bus/routing snapshot | Q-P09/Q-T15; Ableton does not collect third-party plug-ins as project assets |
| Ableton Live / VST3 | Live Set and per-instance VST preset/bank round-trip; processor/controller synchronization; embedded payload and bus restore | Component/controller state trace, parameter-write count, chunk hashes, routing snapshot | Q-P09/Q-T15; exact opaque state payload U |
| Bitwig Studio / VST3 | Project-associated plug-in-state save/reopen; record hosting mode; controlled Reload Plug-in/reopen lifecycle; embedded payload and active-bus restore | Project folder inventory, plug-in-state hashes, hosting mode, process reload trace, routing snapshot | Q-P09/Q-T15; sandbox reload is not proof of transient-voice persistence |
| REAPER / AU | Project save/reopen; FX-chain preset vs per-instance state; offline/online FX load; AU reset/deallocation | Project artifact, preset/state comparison, lifecycle trace, embedded payload hashes | Q-P09/Q-T15; exact AU state callback behavior U |
| REAPER / VST3 | Project save/reopen; FX-chain state; default vs optional Hard reset on playback start; offline/online FX load | Project artifact, VST3 component/controller state trace, reset trace, payload hashes | Q-P09/Q-T15; exact opaque state payload U |

### 10.1 Shared state round-trip assertions

- Four-layer parameter sentinels restore exactly.
- Required unsaved recording and Freeze chunks restore with matching length and checksum.
- External library references retain stable identity; missing assets produce relink-required status.
- Active host note IDs, MPE leases and NoteInstanceIds are not serialized or resurrected.
- VST3 controller synchronization creates no duplicate host parameter writes.
- Bus activation/routing is asserted separately from plug-in-state restore.
- Corrupt/truncated payload, migration failure and memory-admission rejection preserve the prior active state.
- CC 120/121/123, Panic, host stop, process reload and state restore remain separate test stimuli; none may silently thaw DEC-042 Freeze.
- For active voices at restore commit, retain both AR-06 branches (terminate at commit vs non-addressable old-generation tails) as alternatives until owner approval.

### 10.2 Evidence discipline

No host test, callback trace, payload-size limit, memory threshold, AC ID or pass result is created by this backlog. Pin exact host/OS/architecture/plug-in builds before execution. Any host prototype or benchmark requires separate explicit owner authorization.

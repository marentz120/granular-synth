# Phase 2 — Specification Completion Audit

**Status:** Historical Phase 2 gap-audit snapshot. Its identified product questions have since been resolved or explicitly deferred in DEC-022…DEC-041; the six domain specifications and R1 Release Scope Matrix are the current specifications. This document does not authorize implementation.
**Baseline:** `main` at the current approved R1 product scope and DEC-015…DEC-021.
**Purpose:** Complete only requirements that are still open, underspecified, contradictory, or missing. Existing approved requirements remain unchanged.
**Benchmark source:** Tasty Chips GR-MEGA User Manual 1.8.d, as mapped in `GR_MEGA_FEATURE_MAP.md`. Official support page and firmware roadmap are supplementary evidence, not authority to change GRANULAR SYNTH scope.

## 1. Governing rules

1. Do not revise, weaken, remove, or reinterpret any existing approved GRANULAR SYNTH requirement or decision.
2. Do not reopen DEC-015…DEC-021.
3. Use GR-MEGA 1.8.d only to fill a genuine specification gap. A benchmark feature is not automatically a GRANULAR SYNTH requirement.
4. Where the benchmark describes hardware-only behavior, translate only the underlying useful capability to macOS software where appropriate; do not copy hardware-specific UI or implementation.
5. Preserve GRANULAR SYNTH's existing product choices, including exactly four layers, Granular + Sampler in R1, central sequencer, per-layer/global Freeze, recording, and generic MIDI controller operation.
6. Distinguish manual-confirmed facts [MC], manufacturer release-note facts [MC via RN], interpretation [INT], proposal [PROP], and unverified behavior [UNV], following the feature-map convention.
7. A proposed tier is not an approved commitment. Any new scope that would affect release scope must be surfaced for owner approval through the existing review process.
8. No architecture, framework, DSP implementation, resource cap, or host bus decision is made in this document.

## 2. Baseline inspected

The following repository documents were inspected on the current `main` branch:

- `R1_PRODUCT_SPECIFICATION.md`
- `GRANULAR_SYNTH_REQUIREMENTS.md`
- `GR_MEGA_FEATURE_MAP.md`
- `OPEN_QUESTIONS.md`
- `LIVE_PERFORMANCE_SPECIFICATION.md`
- `DECISIONS_LOG.md`
- `DSP_ARCHITECTURE.md`

The feature map already contains substantial manual-derived research for Sampler, sample management, modulation, MIDI, effects, presets/projects, and performance. This work therefore focuses on gaps and consistency, not repeating the benchmark inventory.

## 3. Domain audit

### 3.1 Modulation

**Already specified:** four LFOs per layer; LFO rate, sync, waveforms, phase, amplitude quantization, polarity and key-sync modes; modulation matrix with at least 50 rows; source and destination coverage; quick assignment; row copy/paste/delete; 10 macros; modulation display/monitor; envelope follower; Random 1–4; sequencer modulation sources.

**Open or underspecified:**
- LFO execution scope is unresolved: per-voice versus per-layer. Existing `GS-MOD-002` proposes a Poly/Mono choice, while the manual does not settle the behavior (`LFO-11`, [UNV]).
- Macro count ambiguity: manual prose documents 10, while NRPN entries enumerate 20 (`MAC-07`, [UNV]); current requirement specifies 10 and scalability to 20. Do not silently increase R1.
- Macro modulation semantics, especially whether modulation is per-voice or shared, are unclear (`MAC-04`, [UNV]).
- The XY performance pad replaces hardware position fader sources; define its coordinate range, normalization, persistence, and mapping behavior without altering the existing source list.
- Destination inventory should be reconciled against the 149 enumerated manual destinations, but only destinations corresponding to already-approved R1 parameters are R1 obligations.
- Modulation summing, source polarity, curve interpretation, amount scaling, destination clamping, and behavior when sources are unavailable need precise functional definitions.
- Define how modulation is displayed when several rows and direct LFO routes affect one parameter; preserve the existing summed-modulation display requirement.
- Define deterministic per-note random-source behavior and reset/retrigger semantics; do not change the existing requirement for independent streams.

**Benchmark references:** `GR_MEGA_FEATURE_MAP.md` §10.1–10.4; manual pp. 21, 23–24, 78–80, 137, 157–161.

### 3.2 Effects

**Already specified:** four reorderable serial FX slots per layer; per-slot dry/wet; audio-in and synth sends; R1 effect list (Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator, Compressor); automation/modulation; selectable tail behavior on preset change.

**Open or underspecified:**
- Full parameter-level specifications, ranges, units, defaults, taper/curve, stereo behavior, and bypass semantics for every R1 effect.
- Delay and Ping-pong Delay timing/sync behavior, feedback range, stereo routing, and transition behavior.
- Reverb algorithm controls and whether the R1 Reverb is intentionally the simple/low-CPU type documented by the benchmark.
- Chorus rate/feedback/centre/bandwidth semantics and valid ranges.
- EQ3 filter topology, crossover constraints, gain ranges, and interaction between bands.
- Saturator transfer curve, drive and post-gain ranges, and level compensation behavior.
- Compressor detector, knee, ratio, attack/release, metering, and gain-reduction behavior.
- FX slot state on bypass, reorder, replacement, preset recall and tail linger/cut.
- Audio-input versus synth send routing and gain staging must be stated without deciding plugin bus architecture.
- Duplicate effect instances are currently OPT; do not promote to R1 without approval.
- FX chain and single-effect preset storage are currently L2; do not promote to R1 without approval.
- Master-bus EQ/FX remains L3; do not change.

**Benchmark references:** `GR_MEGA_FEATURE_MAP.md` §12 and §1.2; manual pp. 32, 81–85, 138, 159–161.

### 3.3 Sampler

**Already specified:** one playback head per voice (up to 12 with chord mode); pitch changes playback speed; spray, pan spray, direction and scan; one-shot/loop/ping-pong; looped release; high-quality interpolation option; env-pile-up option.

**Open or underspecified:**
- Exact playback-head start-position and scan-offset semantics.
- Loop flag boundaries, inclusive/exclusive end behavior, wrap direction, ping-pong turnarounds and note-off behavior.
- Looped-release behavior is contradictory in the manual (`EDT-07`, `Q-M05`); retain as unresolved until evidence resolves it. Do not invent a definitive behavior.
- Voice reuse / env-pile-up behavior and interaction with polyphony, same-note retrigger and envelope release.
- Sampler interpolation modes, default quality, anti-aliasing behavior and quality/performance trade-off.
- Sampler behavior when sample source is mono/stereo, playback direction changes, scan reaches a boundary, or source is replaced while notes are active.
- Multisample/SFZ support is L3 in current requirements; do not promote to R1. The benchmark itself only documents loudest velocity layer in the mapped revision.
- Long samples up to 60 minutes are L2; do not promote or set a new R1 duration cap here.
- Crossfade and zero-crossing snap are L2 proposals; retain their tier.

**Benchmark references:** `GR_MEGA_FEATURE_MAP.md` §4 and §9.3; manual pp. 47, 56, 99–103, 113.

### 3.4 Presets, projects and library

**Already specified:** Sample → Patch → Multi → Project/Library conceptual hierarchy; preset browser with banks/tags/search/favourites; user-selectable init template; templates without samples; versioned human-readable state; sample portability via relative path/hash/relink/collect; DAW state restore including unsaved recordings; undo/redo; copy/paste; patch comments.

**Open or underspecified:**
- Resolve the library model in `Q-P07`: desktop searchable library versus performance-bank view and exact relationship between Patch, Preset, Multi/Performance Set and Project. The existing R1 product spec's independent Pattern Library and Performance Sets must remain intact.
- Define save boundaries: per-layer patch, four-layer multi, project, pattern, performance set, template and global preferences.
- Define which data is inherited versus copied when applying a template; ensure sample-specific markers/flags are not accidentally overwritten.
- Define preset recall behavior for active Pattern and FX tails; preserve the approved rule that changing Preset does not replace or stop the active Pattern.
- Resolve sample embedding/reference behavior and unsaved recording persistence consistently with `Q-P08` and `GS-PRS-006`; technical format remains Phase 3.
- Define missing/offline sample recovery, duplicate assets, content-hash mismatch and relink behavior.
- Define version migration, forward compatibility, atomic save/recovery after interrupted writes, and corruption handling.
- Patch comments (100-character benchmark) and sample-name-to-patch-name convenience are not yet explicit in product-level requirements; decide whether useful for R1 or later without assuming parity requires them.
- GR-1/GR-MEGA import remains optional and legally blocked by `Q-L01`; no proprietary file format reverse-engineering is authorized.

**Benchmark references:** `GR_MEGA_FEATURE_MAP.md` §13; manual pp. 52, 62, 94–95, 108–114.

### 3.5 MIDI

**Already specified:** per-layer MIDI channels; MPE; MIDI Learn for automatable parameters; 14-bit CC/NRPN input; soft takeover; Program Change preset recall; channel-mode messages; MIDI Clock input; generic class-compliant controller operation for essential live performance.

**Open or underspecified:**
- MPE zone configuration, member-channel allocation, master/member channel behavior, per-note pitch/pressure/timbre routing and pitch-bend range compatibility.
- MIDI channel assignment conflicts and Omni behavior across four layers.
- MIDI input filtering for notes, CC, Program Change, clock and channel-mode messages.
- Program Change mapping to the unbounded library / performance bank; exact mapping remains to be designed.
- CC/NRPN resolution, parameter scaling, 7-bit versus 14-bit precedence, NRPN timeout/termination and handling of incomplete messages.
- Soft-takeover crossing rules, pickup timeout/cancel behavior and mapping conflict resolution; preserve existing global/per-preset mapping design in `LIVE_PERFORMANCE_SPECIFICATION.md`.
- Panic / All Notes Off / All Sound Off semantics, including FX tails and Freeze state.
- MIDI Clock loss, start/continue/stop, song-position handling and clock stability are not yet specified at product behavior level.
- MIDI output from sequencer and MIDI clock/transport out are L3 in current requirements; do not promote to R1.
- Benchmark CC-map compatibility is L2; do not promote to R1.
- No R1 MIDI feedback, named-controller templates or auto-mapping may be introduced (DEC-021).

**Benchmark references:** `GR_MEGA_FEATURE_MAP.md` §11.1; manual pp. 52, 80, 92–93, 141–156.

### 3.6 Audio, I/O and recording

**Already specified:** real-time-safe audio; Standalone and plugin contexts; per-layer live input; per-layer and global Freeze; per-layer recording and stereo master-mix recording; one-shot/loop recording; 44.1–192 kHz and 16–4096 buffer proposals; multichannel outputs; no-dropout/click and memory requirements.

**Open or underspecified:**
- Create a dedicated audio behavior specification; current audio requirements are dispersed across baseline requirements, platform, sample management, signal flow and non-functional requirements.
- Standalone device selection, input/output device configuration, channel mapping, device change/reconnect behavior and default-device fallback.
- Input monitoring, dry/wet signal path, per-layer input routing and gain staging; do not pre-decide AU/VST3 bus/component architecture.
- Define product-visible recording modes and controls: one-shot/loop, buffer sizing, start position, scrolling record, record-head/play-head relationship, stop/keep/discard/load workflow.
- Clarify whether clocked recording, input-threshold arming, transport-start arming, resampling and synth-to-record feedback remain L2 as currently tiered; do not silently promote.
- Define mono-to-stereo duplication and stereo-to-mono handling; preserve source channel layout where supported.
- Define audio output routing for main stereo and four layer stereo outputs at the product behavior level; exact host buses remain Phase 3.
- Define device/host sample-rate changes, variable block sizes, zero-length/oversized blocks, and behavior on device loss as testable requirements; numeric performance budgets remain later-phase.
- Define overload indication and safe degradation, but do not invent measured voice/grain/recording limits before Phase 3/5.
- Freeze semantics remain explicitly deferred to Phase 3 architecture; this document must not specify what Freeze captures or suspends.

**Benchmark references:** `GR_MEGA_FEATURE_MAP.md` §1.2 and §9.1–9.3; manual pp. 32, 71–74, 89, 94–97, 127–129.

## 4. Cross-domain consistency checks

- Every modulatable or automatable parameter must have a stable identity, but the parameter-ID scheme and exact host exposure remain Phase 3 decisions.
- Modulation, MIDI Learn, host automation and sequencer parameter locks must target the same logical parameter model; no domain-specific duplicate value stores.
- Preset recall must preserve the R1 Pattern independence rule and apply the approved FX tail behavior.
- Recording buffers, saved samples, preset state and DAW state must have one consistent ownership/persistence model.
- All audio-thread changes must respect the existing real-time safety requirements; UI/file/library operations must not block audio processing.
- The four-layer model and shared transport/clock domain remain unchanged.
- No new requirement may contradict DEC-015…DEC-021 or the approved R1 product specification.

## 5. Required outputs to close Phase 2

1. Domain specifications for MOD, FX, SMP, PRS, MID and AUD, each with behavior, parameters, ranges/units where documented, defaults only where evidenced, edge cases, dependencies and explicit unresolved questions.
2. Updated requirement rows with unique GS IDs and source traceability. Existing IDs should be retained; create new IDs only for genuinely new requirements.
3. Updated `OPEN_QUESTIONS.md`: mark each question answered, evidence-resolved, or still open; do not erase historical evidence.
4. Release Scope Matrix with approved R1 versus later/optional tiers, clearly separating owner-approved scope from proposed scope.
5. R1 Acceptance Criteria for each approved requirement. Numerical thresholds that depend on profiling remain Phase 5/architecture inputs.
6. Consistency audit against `R1_PRODUCT_SPECIFICATION.md`, `DECISIONS_LOG.md`, `LIVE_PERFORMANCE_SPECIFICATION.md` and dependencies.

## 6. Exit criteria

Phase 2 is complete only when all six domain specifications are reviewed, all unresolved ambiguities are explicitly tracked, the requirement inventory and tier counts reconcile, acceptance criteria are testable, and the owner has approved the resulting PR. No implementation begins as part of this work.


## 7. Resolution status after the audit

The initial gaps were addressed as follows:

- Modulation: DEC-024 resolves LFO Poly/Mono scope; DEC-031 resolves macro modulation context; DEC-032 resolves XY normalization and saved recall position. Parameter-domain equations and host exposure remain Phase 3.
- Effects: DEC-023 approves the seven-effect R1 set. Parameter ranges/tapers and algorithm details remain Phase 3.
- Sampler: DEC-030 defines GRANULAR SYNTH loop-release behavior; DEC-037 sets Env Pile-up default OFF. Scan mathematics and interpolation implementation remain Phase 3.
- Presets/Library: DEC-022 approves the searchable library/performance-bank model; DEC-025 approves embedding unsaved recording buffers in plugin state. Serialization remains Phase 3.
- MIDI: DEC-026 confirms AU/VST3 delivery target; DEC-033/034/038/039 settle Program Change mapping, shared-channel routing, MPE bend range and clock-loss behavior. Zone negotiation and protocol implementation remain Phase 3.
- Audio/I/O: DEC-040 settles mono live-input behavior. Freeze semantics, host buses, signal tap points and measured resource budgets remain Phase 3/5.
- Scope: DEC-041 approves all 168 requirement tiers; GS-SEQ-010 is L2. Counts reconcile to 124 R1, 27 L2, 9 L3, 8 OPT and 0 Other.

This audit remains as a traceable record of the original gap-finding exercise. For current behavior, use the domain specifications and R1_PRODUCT_SPECIFICATION.md.

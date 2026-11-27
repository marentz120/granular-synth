# R1 Acceptance Criteria — Phase 2 Draft

**Status:** Draft for Phase 2 review; not owner-approved and not a substitute for Phase 5 test plans.  
**Purpose:** Define observable pass/fail behavior for R1 requirements without inventing unmeasured performance thresholds.  
**Authority:** R1_PRODUCT_SPECIFICATION.md; GRANULAR_SYNTH_REQUIREMENTS.md; DEC-015…021; domain specifications in this directory.

## 1. Acceptance conventions

- Each criterion must be verifiable by a repeatable test, inspection or scripted interaction.
- Numeric CPU, memory, latency and concurrency limits that depend on profiling are not invented here; Phase 3/5 must measure and approve them.
- “Pass” requires no contradiction with the approved R1 product specification.
- Host-specific plugin criteria are run only after Phase 3 confirms supported host/component behavior.
- Freeze tests remain blocked until Phase 3 defines Freeze semantics.

## 2. Modulation

| ID | Acceptance criterion | Verification |
|---|---|---|
| AC-MOD-01 | Each layer exposes four independently configurable LFOs. | Configure all four on one layer and verify other layers remain independent. |
| AC-MOD-02 | Each LFO supports free 0–50 Hz and tempo-sync operation, six documented waveforms, phase, amplitude quantization, four polarity modes and Off/On/Legato key sync. | Parameter boundary and waveform tests. |
| AC-MOD-03 | Matrix supports at least 50 rows and all required row fields. | Create/configure 50 rows and round-trip save/load. |
| AC-MOD-04 | Matrix sources and R1 destination inventory match the approved requirements; deferred-only parameters are not exposed as R1 destinations. | Inventory-based automated audit. |
| AC-MOD-05 | Multiple rows and direct LFO routes targeting one parameter produce one stable summed effective value. | Deterministic signal test and UI inspection. |
| AC-MOD-06 | Disabled rows contribute no modulation and remain saved. | Disable/save/reload/compare. |
| AC-MOD-07 | Macros support 10 per patch and multi-destination ranges, curve and invert. | Configure all macros, save/reload and test output. |
| AC-MOD-08 | Envelope follower can select live input and supported layer source. | Feed known signals and verify response. |
| AC-MOD-09 | Random1–4 are independent and reproducible under fixed seed. | Seeded offline render comparison. |
| AC-MOD-10 | Modulation display shows summed modulation and recent destinations without changing audio behavior. | UI test plus audio equivalence. |

## 3. Effects

| ID | Acceptance criterion | Verification |
|---|---|---|
| AC-FX-01 | Each layer has four reorderable serial FX slots. | Insert and reorder four effects independently on each layer. |
| AC-FX-02 | R1 exposes exactly the required effect set; later-tier effects are not required for R1. | Effect inventory audit. |
| AC-FX-03 | Each slot has independent dry/wet control and audio-input/synth send controls are independent. | Routing and gain tests. |
| AC-FX-04 | Every continuous R1 effect parameter is addressable by the shared modulation/automation model, subject to Q-T12 host exposure decision. | Parameter inventory and modulation test. |
| AC-FX-05 | Preset switching applies the selected Linger or Cut tail policy. | Repeatable tail test with each policy. |
| AC-FX-06 | Bypass, reorder, replacement and parameter changes do not introduce audible clicks under the approved test conditions. | Null/listening and transient tests; thresholds Phase 5. |

## 4. Sampler

| ID | Acceptance criterion | Verification |
|---|---|---|
| AC-SMP-01 | Each sampler voice has one playback head and pitch changes playback speed. | Render known sample at multiple pitches and verify duration/pitch relation. |
| AC-SMP-02 | Sampler supports forward/reverse direction, spray, pan spray and scan behavior. | Parameterized playback tests. |
| AC-SMP-03 | One-shot, loop and ping-pong playback obey active sample flags. | Boundary-condition render tests. |
| AC-SMP-04 | Looped-release behavior matches the owner-approved resolution of Q-M05. | Test after ambiguity resolution; blocked until then. |
| AC-SMP-05 | High-quality interpolation option is available and sample-rate behavior is consistent. | Frequency-sweep and cross-rate comparison. |
| AC-SMP-06 | Env Pile-up option follows its approved semantics and default. | Retrigger/overlap test after default decision. |
| AC-SMP-07 | Sample replacement, missing sample and invalid crop states fail safely. | Fault-injection and recovery tests. |

## 5. Presets, projects and library

| ID | Acceptance criterion | Verification |
|---|---|---|
| AC-PRS-01 | Patch, Multi, Pattern, Performance Set and Project data restore the required state without cross-object corruption. | Save/restart/reload fixtures. |
| AC-PRS-02 | Browser supports banks, tags, search, favourites and default template. | UI and query tests. |
| AC-PRS-03 | Applying a template does not replace sample asset or sample-specific flags unless explicitly requested. | Before/after state comparison. |
| AC-PRS-04 | Preset change does not replace or stop the active Pattern. | Live playback scenario. |
| AC-PRS-05 | Unsaved Pattern edits survive Pattern switching as temporary Drafts; only explicit Save persists latest version. | Edit/switch/return/restart tests. |
| AC-PRS-06 | Sample collect/relink verifies relative path and content hash; mismatch is never silently substituted. | Move project and hash-mismatch tests. |
| AC-PRS-07 | Corrupt/missing assets never crash Standalone or host; actionable error is shown. | Fault injection. |
| AC-PRS-08 | Plugin state restores fully, including recorded unsaved buffers, using the Phase 3-approved mechanism. | Host save/close/reopen fixture. |
| AC-PRS-09 | Preset format version migration preserves parameter meaning and sound within approved tolerances. | Golden fixture migration tests. |

## 6. MIDI

| ID | Acceptance criterion | Verification |
|---|---|---|
| AC-MID-01 | Four layers can be assigned MIDI channels and receive notes according to the approved channel-collision policy. | Channel routing matrix. |
| AC-MID-02 | MPE per-note pitch, pressure and timbre remain associated with the correct overlapping notes. | MPE conformance test. |
| AC-MID-03 | MIDI Learn and manual mapping support CC, channel and Note assignment for eligible parameters. | Mapping workflow tests. |
| AC-MID-04 | Soft takeover prevents jumps when controller and software values differ. | Mismatched-position test. |
| AC-MID-05 | Conflicting mappings are detected and surfaced, not silently overwritten. | Conflict matrix. |
| AC-MID-06 | Program Change recalls the mapped preset where supported and does not stop/replace active Pattern. | Standalone and supported-host tests. |
| AC-MID-07 | MIDI Clock input synchronizes central sequencer in Standalone; clock loss/restart behavior matches approved protocol spec. | Clock generator test; blocked on protocol decision. |
| AC-MID-08 | All Notes Off, All Sound Off and Reset Controllers follow their specified semantics. | MIDI message tests. |
| AC-MID-09 | Panic is reachable and effective with one controller gesture at all times. | Live performance test. |
| AC-MID-10 | Every essential live operation and all sequencer editing are controller-reachable without keyboard/mouse. | Operation-by-operation controller-only test. |
| AC-MID-11 | No R1 MIDI feedback or named-controller template is required. | Scope audit, not a feature test. |

## 7. Audio, I/O and recording

| ID | Acceptance criterion | Verification |
|---|---|---|
| AC-AUD-01 | Live input can be routed independently to each layer in Standalone and supported plugin contexts. | Input routing matrix. |
| AC-AUD-02 | Missing input bus/channel results in silence and clear status, not crash or loss of synthesis. | Host/device fault test. |
| AC-AUD-03 | Standalone device disconnect/reconnect is recoverable and does not corrupt project state. | Device hot-unplug test. |
| AC-AUD-04 | One-shot and loop recording create valid buffers; Granular and Sampler can play the buffer while recording. | Record/playback test. |
| AC-AUD-05 | Each layer can be recorded independently and stereo master mix can be recorded. | Routing/capture comparison. |
| AC-AUD-06 | Captured audio can be retained, exported and reloaded as a sample. | Export/re-import test. |
| AC-AUD-07 | Scrolling record maintains the specified playback-head distance behind record head. | Time-domain test after control range is finalized. |
| AC-AUD-08 | Recording storage failure is handled safely without corrupting other project state. | Disk-full/fault injection. |
| AC-AUD-09 | Audio callback performs no blocking I/O, routine allocation, locks or unbounded work. | Static/runtime instrumentation. |
| AC-AUD-10 | Reference load meets no-dropout/no-click requirement at the Phase 5-approved machine/rate/buffer/load profile. | Performance stress suite; profile TBD. |
| AC-AUD-11 | Per-layer and global Freeze work exactly as defined by the Phase 3-approved semantics. | Blocked until Freeze semantics approved. |

## 8. Cross-domain and release gates

- Standalone and plugin produce equivalent output for identical state/input/MIDI/sample rate, excluding device I/O (GS-PLT-004).
- Preset state, sample references, recorded buffers and host state restore coherently.
- Parameter IDs remain stable after first public release (GS-NFR-006).
- AU validation and pluginval strictness 10 pass for each release build (GS-NFR-007).
- Missing/corrupt presets or samples never crash the host (GS-NFR-012).
- Numeric CPU, memory, latency, voice/grain and maximum-recording limits are approved only after profiling.
- Freeze acceptance is incomplete until Phase 3 semantics are approved.

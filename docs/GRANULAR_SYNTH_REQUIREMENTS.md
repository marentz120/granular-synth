# GRANULAR SYNTH Requirements

**Status:** Phase 2 requirement inventory and release-tier classification approved by DEC-041. Technical feasibility, numeric resource budgets and architecture remain subject to Phase 3/5 validation.
**Version:** 1.0 (Phase 2 scope and specification completion, 2026-11-27)
**Inputs:** GR_MEGA_FEATURE_MAP.md (manual 1.8.d + fw 1.8.x release notes), PRODUCT_VISION.md, CLAUDE.md.

## Change summary (v0.5 → v1.0)

- Approved the release-tier classification for all 168 requirement rows under DEC-041.
- Resolved product-level questions Q-P02, Q-P07, Q-P08, Q-P11, Q-P15, Q-P16 and relevant manual ambiguities through DEC-022…DEC-040.
- Moved GS-SEQ-010 (Patch spray/bias) to L2; benchmark semantics remain unverified and it is not an R1 dependency.
- Added six detailed domain specifications, the consolidated R1 Release Scope Matrix and product-level R1 Acceptance Criteria.
- Reconciled requirement totals: 124 R1, 27 L2, 9 L3, 8 OPT, 0 Other (168 total).
- Numeric technical targets, architecture choices, measured resource limits and implementation remain gated to later phases.

## Change summary (v0.4 → v0.5)

Corrections only — no decision status or R1 scope changed beyond what DEC-017…DEC-021 already approved:
- GS-UI-007: removed the controller-feedback clause from its R1 text (DEC-021 excludes MIDI feedback from R1); the requirement itself is unchanged otherwise, still Approved.
- Added **GS-SMG-021** (Freeze, per-layer and global) to complete DEC-020's already-approved scope — the only new requirement added. Per PR #4 review, its wording is deliberately neutral: it states per-layer and global availability only, and defers freeze semantics to Phase 3.
- GS-ARC-001: fixed a stale citation (cited Q-P04 as open; it is ANSWERED via DEC-017) and cross-referenced GS-ARC-005.
- GS-MID-007 and GS-SEQ-005 cross-reference each other instead of independently restating MIDI-Clock sync — no ID removed, no tier changed.
- §6 Requirement summary fully recounted against §3/§4 and restructured per PR #4 review into an explicit counting method (§6.1), a plain-integer table that reconciles row-by-row and column-by-column (§6.2), and footnotes listing the split-tier, "Other" and Approved rows (§6.3). Corrects stale Slice/Sequencer/MIDI counts and the grand total (164 → 168).

## Change summary (v0.3 → v0.4)

- Added the authoritative R1 product scope baseline in docs/R1_PRODUCT_SPECIFICATION.md; recorded DEC-017…DEC-021.
- R1 scope now includes four independent layers, Granular + Sampler engines (Granular Slice deferred), a full central step sequencer, live input/Freeze/recording and generic controller-only performance.
- Reclassified sequencer requirements and MIDI Clock input to R1; sequencer topology and behavior are specified in the R1 product specification.
- Detailed DSP limits, acceptance thresholds and format-specific plugin input implementation remain for later phases.

## Change summary (v0.2 → v0.3)

- GS-MID-008 and GS-UI-007 remain the explicitly approved controller-operability requirements. DEC-017…DEC-021 additionally approve product scope for layers, engines, sequencer, live input and MIDI control; detailed acceptance criteria and engineering feasibility remain later-phase work.
- Updated §1/§2 to record that both studio and live-performance use cases are now confirmed primary (DEC-015), and updated §6/§7 counts and dependencies accordingly.
- No other requirement, tier, or wording changed.

## Change summary (v0.1 → v0.2)

- **Kept unchanged:** all existing IDs REQ-PLAT-001…003, REQ-AUD-001…005 and QA-001…005 (§0). Nothing was removed or reworded.
- **Added:** detailed functional requirements `GS-<AREA>-NNN` derived from the GR-MEGA feature map, each with a proposed tier and a trace to feature-map IDs (§3), non-functional requirements GS-NFR-* that refine (not replace) REQ-AUD/QA (§4), exclusions (§5), and dependencies between areas (§7).
- **Deferred (gap):** per-requirement acceptance criteria are a Phase 5 deliverable (see DEVELOPMENT_ROADMAP.md). The Traceability rule in §8 is retained.

---

## 0. Baseline requirements (carried over verbatim from v0.1)

### Platform
- REQ-PLAT-001: Target macOS.
- REQ-PLAT-002: Provide a standalone application.
- REQ-PLAT-003: AU and VST3 are proposed plugin formats, pending validation.

### Audio engine
- REQ-AUD-001: Audio processing must be real-time safe.
- REQ-AUD-002: The audio callback must not perform blocking I/O, unpredictable waits, or routine heap allocation.
- REQ-AUD-003: Resource limits and overload behavior must be explicit and testable.
- REQ-AUD-004: Parameter changes must avoid audible zipper noise and discontinuities where appropriate.
- REQ-AUD-005: Supported sample rates and buffer sizes must be specified during architecture/QA planning.

### Quality attributes
- QA-001: Prevent dropouts, clicks, and denormal-related performance problems.
- QA-002: Make DSP components independently testable.
- QA-003: Preserve consistent parameter meaning across standalone and plug-in builds.
- QA-004: Provide reproducible builds and automated tests.
- QA-005: Measure CPU, memory, latency and voice/grain capacity under defined conditions.

---

## 1. Product definition

GRANULAR SYNTH is a professional, sample-based granular instrument for macOS, delivered as:

- **Standalone macOS application** (own audio/MIDI device handling, live audio input).
- **Audio plugins:** AU and VST3 (target formats subject to confirmation — see OPEN_QUESTIONS Q-P02).

**Use case (DEC-015, Approved 2026-11-27):** both studio sound design and live performance are primary. The resulting approved R1 layer, engine, sequencer, live-input and controller scope is recorded in DEC-017…DEC-021 and docs/R1_PRODUCT_SPECIFICATION.md.

Functional benchmark: Tasty Chips GR-MEGA (manual 1.8.d) — DEC-001. We reproduce *documented behavior and workflows*, not firmware, code, UI artwork or file formats, and we add improvements where software allows (marked **[PROP]** in the feature map).

## 2. Priority tiers (approved by DEC-041)

| Tier | Meaning |
|---|---|
| **R1** | Required for first release. The product is not shippable without it. |
| **L** | Later development. Planned, sequenced after R1. Sub-tagged **L2** (next wave) / **L3** (following wave) as a *suggested* order only. |
| **OPT** | Optional improvement; no commitment. |

**Approved R1 product scope (DEC-017…DEC-021):** four independent layers; Granular + Sampler engines; live input, Freeze and recording; a full central performance step sequencer; controller-only standalone live operation. Granular Slice is deferred; Tape and Spectral remain later engines. See docs/R1_PRODUCT_SPECIFICATION.md.

Requirement IDs: `GS-<AREA>-<NNN>`. "Source" refers to feature-map IDs.

---

## 3. Functional requirements

### 3.1 Platform & formats (PLT)

| ID | Requirement | Approved tier | Source / notes |
|---|---|---|---|
| GS-PLT-001 | The product shall ship as a standalone macOS app and as AU and VST3 plugins built from one shared DSP/state core. | R1 | Refines REQ-PLAT-001…003; DEC-026; exact component/bus design Phase 3 |
| GS-PLT-002 | Native Apple Silicon and Intel (Universal 2) binaries. | R1 | Q-T03 |
| GS-PLT-003 | Operate at any host sample rate 44.1–192 kHz and any buffer size 16–4096, including variable block sizes. | R1 | Proposed values for REQ-AUD-005 (Q-T17); GR-MEGA is fixed 48 kHz (ARCH-09) |
| GS-PLT-004 | Plugin and standalone shall produce bit-identical output for identical state, input, MIDI and sample rate (excluding device I/O). | R1 | Refines QA-003 |
| GS-PLT-005 | CLAP format. | OPT | — |
| GS-PLT-006 | AUv3 (sandboxed extension; also enables iPad later). | OPT | — |

### 3.2 Architecture: layers, voices, grains (ARC)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-ARC-001 | 4 independent layers per instance, each with its own patch (sample + parameters), voices, modulation, FX chain and MIDI channel; per-layer engine selection per GS-ARC-005. | R1 | ARCH-04, ARCH-11; **DEC-017** (Q-P04 **ANSWERED** — layer count and per-layer Granular/Sampler selection) |
| GS-ARC-002 | Per-layer polyphony 1–20 voices (patch parameter). | R1 | ARCH-05, VOI-01 |
| GS-ARC-003 | Up to 128 simultaneous grains per voice. | R1 | ARCH-06 |
| GS-ARC-004 | A global, configurable grain budget across all layers with graceful (click-free), deterministic degradation when exceeded. | R1 | ARCH-07; refines REQ-AUD-003; [PROP] — budget value set by CPU benchmark (Q-T07) |
| GS-ARC-005 | Engine selectable per layer: Granular and Sampler in R1; Granular Slice is deferred (L2 candidate), Tape L2, Spectral L3. | R1 | ARCH-01; DEC-018 |
| GS-ARC-006 | Multi-output: stereo main + 4 stereo layer outputs (plugin busses / standalone channel routing). | R1 | SIG-07 |
| GS-ARC-007 | Signal chain per voice: engine + sub-osc → LPF → HPF; per layer: FX×4 → M-S → patch volume → layer mix → wet; dry live-input path summed before master. | R1 | SIG-01…04 |
| GS-ARC-008 | Default voice gain and grain-count normalization designed so 4 dense layers do not clip at default settings. | R1 | SIG-08, SIG-09 |
| GS-ARC-009 | Optional master safety limiter (default OFF, to preserve benchmark behavior). | OPT | SIG-10 [PROP] |
| GS-ARC-010 | "Control all layers" edit mode. | L2 | ARCH-12 |

### 3.3 Granular engine (GRN)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-GRN-001 | Grain rate 0.1–1000 grains/s per voice. | R1 | GRN-01 |
| GS-GRN-002 | Grain size range 0.1–5000 ms. | R1 | GRN-02, C-01; Q-M01 resolved by DEC-035 |
| GS-GRN-003 | Live display of density = rate × size. | R1 | GRN-03 |
| GS-GRN-004 | Position with ≥ 14-bit resolution (we will use double-precision internally). | R1 | GRN-04 |
| GS-GRN-005 | Spray (0 → full sample) with bipolar / +unipolar / −unipolar polarity. | R1 | GRN-05, GRN-06 |
| GS-GRN-006 | Pan, pan spray and pan-spray distribution (at least uniform and a centre-weighted shape). | R1 | GRN-07…09; distributions [INT] |
| GS-GRN-007 | Mid-side width per layer. | R1 | GRN-10 |
| GS-GRN-008 | Direction probability (all reverse ↔ 50/50 ↔ all forward). | R1 | GRN-11 |
| GS-GRN-009 | Scan −2× … +2× with fine and ±0.25-snap modes; respects stop and loop flags. | R1 | GRN-12 |
| GS-GRN-010 | Pitch ±1 oct coarse / ±1 st fine, pitch spray with polarity, snapping to semitones/chord notes. | R1 | GRN-13, GRN-14, CHD-07 |
| GS-GRN-011 | Ratio modes: Free, DensitySize, DensityRate, ScanRate, ScanOverlap, TimeStretch. Behavior at scan = 0 defined and documented. | R1 | GRN-20…26 |
| GS-GRN-012 | Window families PowAR (sides, tilt, curve) and Raised Cosine (tilt, curve, AM). | R1 | WIN-01…05 |
| GS-GRN-013 | Window changes, including fast modulation, shall never click on grains already playing. | R1 | WIN-08 → WIN-P1 [PROP] |
| GS-GRN-014 | Grain key trigger (Off / On / Legato), grain clock sync with straight/triplet/dotted divisions 1/64…64/1, voice clock sync. | R1 | SCH-01…04, C-11 |
| GS-GRN-015 | Spawn modes Gradual, Direct, Direct reset, Recycle. | R1 | SCH-05 |
| GS-GRN-016 | Local pitch (grains keep birth pitch) ON/OFF. | R1 | SCH-07 |
| GS-GRN-017 | Grain stealing with threshold 0 (off) … max; click-free. | R1 | SCH-08 |
| GS-GRN-018 | Grain-count normalization ("compression") ON/OFF. | R1 | SIG-09 |
| GS-GRN-019 | Sub-sample-accurate grain onset timing. | R1 | SCH-P1 [PROP] |
| GS-GRN-020 | Interpolation quality selectable per layer: at least Linear and a higher-quality mode (cubic Hermite minimum) usable in granular at full polyphony on reference hardware. | R1 | SCH-09 → SCH-P2 [PROP] |
| GS-GRN-021 | Grain probability (per-grain spawn skip). | L2 | GRN-15, RN-01 |
| GS-GRN-022 | Granular filter (spectral-content-guided spawning using LPF/HPF as selector) with on-waveform highlighting. | L2 | SCH-10 |
| GS-GRN-023 | Anti-aliased upward transposition (band-limited source / mip-maps). | L2 | SCH-P2 [PROP] |

### 3.4 Chord, arpeggiator, quantization (CHD)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-CHD-001 | Chord mode in all engines; 12 chord slots with fine tune; duplicates allowed; lowest note = played note. | R1 | CHD-01…03 |
| GS-CHD-002 | Granular engines distribute chord notes across grains at constant level; other engines stack. | R1 | CHD-04 |
| GS-CHD-003 | Granular arp modes: up, down, down-up, up-down, random, shuffle, forward, backward, fwd-bwd, bwd-fwd. | R1 | CHD-05 |
| GS-CHD-004 | Chord recording from played notes. | R1 | CHD-06 |
| GS-CHD-005 | Scale quantization with preset scales and user scale; per-voice or per-grain; sub-osc quantize. | R1 | PQ-01, PQ-03, PQ-04 |
| GS-CHD-006 | Microtonal scales (n-TET > 12). | L2 | PQ-02 |
| GS-CHD-007 | Scala (.scl/.kbm) import and MTS-ESP support. | OPT | PQ-06 [PROP] |

### 3.5 Granular Slice engine (SLC) — deferred from R1

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-SLC-001 | Slice engine: granular playback confined to a slice; chromatic mapping from a configurable root (default C-2 per benchmark). | L2 | SLC-01, SLC-02; root note configurable = [PROP] |
| GS-SLC-002 | Manual add / delete / delete-all / drag slices (mouse and keyboard). | L2 | SLC-03, SLC-05, SLC-06 |
| GS-SLC-003 | Auto-slice by transient detection with sensitivity, previewed before commit, non-blocking for audio. | L2 | SLC-04 (benchmark is offline and may crackle → we must not) |
| GS-SLC-004 | Filter cutoff per slice option. | L2 | SLC-07 |
| GS-SLC-005 | WAV cue/marker import as slices. | L2 | SLC-08 |
| GS-SLC-006 | Slice count ≥ 128. | L2 | SLC-11 [UNV] — our spec |

### 3.6 Sampler engine (SMP)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-SMP-001 | One play head per voice (up to 12 with chord), pitch = speed; spray, pan spray, direction and scan behave as documented. | R1 | SMP-01…05 |
| GS-SMP-002 | Loop flags confine playback; one-shot / loop / ping-pong; looped release: ON continues looping through the voice release; OFF exits at the next valid loop boundary and continues toward Stop. | R1 | SMP-06, EDT-07; product behavior DEC-030 (not a claim about verified benchmark behavior) |
| GS-SMP-003 | High-quality (polyphase/windowed-sinc) interpolation option. | R1 | SMP-09 |
| GS-SMP-004 | Env pile-up option, default OFF for Sampler; user may enable per layer. | R1 | SMP-10; DEC-037 |
| GS-SMP-005 | Long samples up to 60 min (disk-streamed or RAM, per Q-T05). | L2 | SMP-07 |
| GS-SMP-006 | Loop crossfade and zero-crossing snapping. | L2 | EDT-P1 [PROP] |
| GS-SMP-007 | SFZ multisample import (incl. velocity layers — beyond benchmark). | L3 | SMP-11 |

### 3.7 Tape engine (TAP)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-TAP-001 | Play head chasing target position with adjustable slew; speed/pitch derived from head motion; full position/scan modulation (scratching). | L2 | TAP-01…03 |
| GS-TAP-002 | Chord heads: one tracking head + pitch-bent copies (tracking note per Q-M10). | L2 | TAP-04 |

### 3.8 Spectral engine (SPC)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-SPC-001 | Phase-vocoder engine with FFT 256–8192; ≤ 3 voices per layer by default. | L3 | SPC-02…04 |
| GS-SPC-002 | Controls: amplitude threshold, phase smear, amp smear, scan, pan, bend (2 algorithms), pitch, pitch blend, freeze, formant shift + lock, fold, spectral window shaping, spectral contrast. | L3 | SPC-05…15 |
| GS-SPC-003 | Chord mode ×12 at low CPU. | L3 | SPC-18 |
| GS-SPC-004 | Transient-preserving high-quality mode (phase-gradient heap integration class algorithm). | L3 | SPC-17 |
| GS-SPC-005 | Stereo at all FFT sizes (benchmark 8192 is mono). | OPT | SPC-04 [PROP] |
| GS-SPC-006 | Report latency to host when spectral engine processes live input. | L3 | Q-T10 |

### 3.9 Voices, envelopes, filters, sub-osc (VOI)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-VOI-001 | Voice stealing preferring released, non-sustained, oldest voices; click-free (fast fade) rather than abrupt. | R1 | VOI-02 [PI: click-free] |
| GS-VOI-002 | Sustain/hold CC64. | R1 | VOI-03 |
| GS-VOI-003 | Glide 0–2000 ms + "always" mode. | R1 | VOI-04 |
| GS-VOI-004 | Velocity sensitivity 0–100 %; velocity curve. | R1 | VOI-05, MID-11 |
| GS-VOI-005 | Pitch-bend range 0–48 st; transpose semitone + cents. | R1 | VOI-06, VOI-07 |
| GS-VOI-006 | 4 ADSR envelopes (pitch, filter, amp, aux), 0.1 ms–45 s per stage, per-stage curves, bipolar amount, invert. | R1 | ENV-01…04 |
| GS-VOI-007 | Envelope loop modes. | L2 | ENV-05 [UNV] |
| GS-VOI-008 | LPF and HPF per voice, 0–20 kHz, resonance, slopes 6/12/18/24 dB/oct, routing (LP / LP+HP / HP) and bypass. | R1 | FLT-01, FLT-02, FLT-04 |
| GS-VOI-009 | Filter key tracking. | R1 | FLT-05 |
| GS-VOI-010 | Sub-oscillator per voice: −1/−2 octave, level, poly balance, quantize option. | R1 | SUB-01, SUB-02 |

### 3.10 Sample management & recording (SMG)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-SMG-001 | Load WAV (8/16/24/32-bit int, 32/64-bit float), AIFF, FLAC; mono or stereo; any sample rate (resampled on load or rate-compensated at playback). | R1 | SMG-01…03; Q-T04 |
| GS-SMG-002 | Drag-and-drop from Finder and from DAW; in-app browser with audition preview. | R1 | SMG-07 [PI: drag & drop] |
| GS-SMG-003 | Loading never blocks or glitches audio; previous sample keeps playing until the new one is ready. | R1 | Non-functional; benchmark warns of crackle on offline ops |
| GS-SMG-004 | Optional normalize on import. | R1 | SMG-06 |
| GS-SMG-005 | R1 waveform view with zoom/scroll, all voice/grain positions, flags and record head; spectrogram view is deferred to L2. | R1 | EDT-12, EDT-13, SMP-08 |
| GS-SMG-006 | Flags: start, stop, loop start, loop end; scan modes one-shot / loop / bounce; scan key trigger Off/On/Legato/Poly; scan sync; loop clock sync; looped release. | R1 | EDT-01…07 |
| GS-SMG-007 | Crop (to view, to recorded, to flags), soft-cut crossfade, normalize; non-destructive with undo. | R1 | EDT-08…10, EDT-P2 [PROP] |
| GS-SMG-008 | Record live input into the layer buffer: one-shot or loop recording; record start modes; input gain ±12 dB; mono duplication LL/RR. | R1 | REC-03, REC-04, REC-06, REC-07 |
| GS-SMG-009 | Granulate/play the buffer while recording (R1 Granular and Sampler engines). | R1 | REC-13 |
| GS-SMG-010 | Scrolling record (play head locked at adjustable distance behind record head). | R1 | REC-08 |
| GS-SMG-011 | Buffer size AUTO or fixed (seconds or bars), init mono/stereo. | R1 | REC-10, REC-11 |
| GS-SMG-012 | Live-input path with input level, wet, dry controls (granular effect use) in Standalone and plug-in contexts; format-specific bus/component design remains Phase 3 work. | R1 | REC-14, SIG-04 |
| GS-SMG-013 | Arm/trigger: on transport start, or input threshold −80…0 dB. | L2 | REC-05 |
| GS-SMG-014 | Clocked (bar-length) recording. | L2 | REC-09 |
| GS-SMG-015 | Resample instrument output to a new sample / WAV file. | L2 | REC-12, SIG-11 |
| GS-SMG-016 | Synth → record-buffer feedback level. | L2 | SIG-13 |
| GS-SMG-017 | Export / save edited or recorded buffer as WAV. | R1 | SMG-01 |
| GS-SMG-018 | Tuning reference tone. | L2 | EDT-11 |
| GS-SMG-019 | Maximum granular sample length ≥ 6 minutes stereo at 48 kHz per layer (exceeds benchmark 5m49s). | R1 | SMG-08, C-02 |
| GS-SMG-020 | Record each layer independently and record the stereo master mix; save/export captured audio for later sample use. | R1 — product scope approved | DEC-020; R1_PRODUCT_SPECIFICATION.md §3 |
| GS-SMG-021 | Freeze: provide an independently switchable per-layer Freeze and a global Freeze across all layers. MIDI-controller-reachable per GS-MID-008. | R1 — product scope approved | DEC-020; R1_PRODUCT_SPECIFICATION.md §3. **Exact freeze semantics are a Phase 3 architecture decision** — this row fixes only the product-level requirement (per-layer + global availability) and its tier, and deliberately makes no claim about what Freeze holds, suspends or continues. |

### 3.11 Modulation (MOD)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-MOD-001 | 4 LFOs per layer: 0–50 Hz free or tempo-synced; waveforms sine/tri/saw/−saw/square/random (S&H); phase; amplitude quantization; polarity (4 modes); key sync Off/On/Legato. | R1 | LFO-01…08 |
| GS-MOD-002 | Each LFO has a selectable Poly/Mono phase mode: Mono shares phase per layer (default); Poly has independent phase per voice. | R1 — approved product behavior | LFO-11 [UNV] → Q-M11; DEC-024; MODULATION_SPECIFICATION.md §2.1 |
| GS-MOD-003 | Modulation matrix with ≥ 50 rows: enable, source, curve, amount, polarity, destination. | R1 | MTX-01…03 |
| GS-MOD-004 | R1 sources: LFO1–4, mod wheel, key track, velocity, pitch bend, channel/poly aftertouch, MPE timbre, release velocity, aux env, Random1–4 (per note, independent streams), XY performance pad (replaces "Pos X/Pos Y"), sequencer mod lanes and envelope follower (input and layers). CV inputs and Draw X/Y are not R1. | R1 | MTX-04, MTX-11, RN-13, RN-19 |
| GS-MOD-005 | Destinations: every eligible continuous parameter belonging to an R1 engine, envelope, filter, LFO, FX or macro; benchmark-only and deferred-feature destinations are not required in R1. | R1 | MTX-05 |
| GS-MOD-006 | Hold-and-turn style quick assignment (UI: modifier + drag on any control), and direct per-LFO destination routing. | R1 | MTX-07, LFO-09, LFO-10 |
| GS-MOD-007 | Copy/paste/delete matrix rows. | R1 | MTX-09 |
| GS-MOD-008 | 10 macros per patch in R1; multi-destination with start/end range, curve, invert; macros themselves modulatable; exposed to host automation. 20 macros are not an R1 requirement. | R1 | MAC-01…07; Q-M06 resolved by DEC-036 |
| GS-MOD-009 | Display summed modulation on controls (ring/arc) and a "recent destinations" monitor. | R1 | MTX-14 [PI: on-control display] |
| GS-MOD-010 | Drawn trajectory modulator (Draw X/Y). | L3 | MTX-12 [UNV] |
| GS-MOD-011 | CV-in via DC-coupled audio input channels. | OPT | HW-03 [PROP] |
| GS-MOD-012 | Patch randomizer with section selection and per-knob list. | L2 | GEN-06 |

### 3.12 Sequencer & performance (SEQ)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-SEQ-001 | One central step sequencer controls all four layers; variable pattern length and step/edit resolution; up to 8 simultaneous events per step, freely assigned across layers; event-level pitch and granular/layer parameter data. | R1 — scope approved | SEQ-01…04; DEC-017/019 |
| GS-SEQ-002 | Sequencer modulation lanes and parameter-lock/event overrides with live-edit semantics. | R1 | SEQ-05/06; DEC-019 |
| GS-SEQ-003 | Pattern timing tools: selectable straight grids, triplets, per-pattern quantization, microtiming, Swing and Humanize. | R1 | SEQ-07…10; DEC-019 |
| GS-SEQ-004 | Pattern management: multiple patterns, save/recall, end-of-bar pattern switching, chaining and ordered Performance Sets; no fixed product-level count cap. | R1 | SEQ-01…10; DEC-019 |
| GS-SEQ-005 | Synchronization to internal BPM, external MIDI Clock, Ableton Link and DAW/host transport. | R1 | MID-08; DEC-019. This is the authoritative statement of the full synchronization scope; MIDI Clock input itself is also tracked as GS-MID-007 (standalone MIDI-input requirement) — see that row. |
| GS-SEQ-006 | MIDI out of sequencer notes; MIDI clock/transport out (standalone; plugin MIDI out where the format allows). | L3 | SEQ-13, MID-09 |
| GS-SEQ-007 | Record MIDI into sequencer. | L2 | SEQ-11 |
| GS-SEQ-008 | Play/drone mode per layer (latching continuous note) and panic. | R1 | MID-13 |
| GS-SEQ-009 | Layer mix and per-layer position performance controls. | R1 | PRF-01 |
| GS-SEQ-010 | Patch spray / bias equivalent. | L2 | PRF-02; Q-M12 resolved for R1 tier by DEC-029; benchmark semantics remain unverified |
| GS-SEQ-011 | Automatic latched granular Fill with continuous intensity control, plus immediate layer and step/event Mute. | R1 | DEC-019; R1_PRODUCT_SPECIFICATION.md §4.3 |
| GS-SEQ-012 | Pattern editing during playback; edits apply at bar end; explicit save, temporary draft on pattern switch, full Undo/Redo and latest-saved-version-only persistence. | R1 | DEC-019; R1_PRODUCT_SPECIFICATION.md §4.5 |

### 3.13 MIDI (MID)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-MID-001 | Per-layer MIDI channel (standalone and plugin). | R1 | MID-02 |
| GS-MID-002 | MPE (per-note pitch, pressure, timbre; PB range 48). | R1 | MID-03; architecture-level, hard to retrofit |
| GS-MID-003 | MIDI learn for any automatable parameter; 14-bit CC and NRPN input; soft-takeover (pickup) option. | R1 | MID-04, MID-18 |
| GS-MID-004 | Default CC map documented; benchmark CC map compatibility (same CC numbers for common parameters). | L2 | MID-05; Q-L02 |
| GS-MID-005 | Program change → preset recall (standalone; plugin where host allows). | R1 | MID-06 |
| GS-MID-006 | Channel-mode messages: all notes off, all sound off (incl. FX tails), reset controllers. | R1 | MID-12 |
| GS-MID-007 | MIDI clock in (standalone), as a MIDI-input/protocol requirement. | R1 | MID-08; DEC-019. Refines the MIDI-Clock element of GS-SEQ-005's broader synchronization requirement (which also covers BPM, Ableton Link and DAW/host transport) — not an independent synchronization spec; kept as its own ID because it is also a MIDI-protocol/input-handling requirement (§3.13 MID), distinct from sequencer behavior (§3.12 SEQ). |
| GS-MID-008 | Standalone operation shall be fully operable via MIDI controller alone for all *essential live-performance* operations, without requiring a computer keyboard or mouse. "Essential" excludes one-time setup/configuration tasks (sample loading, patch editing detail, initial mapping). | **R1 — Approved** | DEC-016 (owner decision, PR #2, 2026-11-27). Detailed in docs/LIVE_PERFORMANCE_SPECIFICATION.md. Generic class-compliant controller is the R1 target; no named-controller template or MIDI feedback is required (DEC-021). |

### 3.14 Effects (FX)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-FX-001 | 4 reorderable FX slots per layer; each slot dry/wet; audio-in send and synth send (−100…+24 dB). | R1 | FX-01…03, FX-10 |
| GS-FX-002 | R1 effect set: Delay, Ping-pong delay, Reverb, Chorus, EQ3, Saturator, Compressor. | R1 — approved | FX-T01…04, T06, T08, T13; Q-P11 answered by DEC-023; EFFECTS_SPECIFICATION.md §2 |
| GS-FX-003 | L effect set: Large reverb, Resonator (MBR), Flanger, Vibrato, Ring mod (incl. AM), Reducer, Distortion. | L2 | FX-T05, T07, T09–T12, T14 |
| GS-FX-004 | All FX parameters automatable and modulatable. | R1 | FX-07 |
| GS-FX-005 | FX chain presets and single-FX presets. | L2 | FX-05, FX-06 |
| GS-FX-006 | FX tails on preset switch: linger / cut option. | R1 | FX-08 |
| GS-FX-007 | Allow the same effect type more than once per layer. | OPT | FX-01 [PROP] |
| GS-FX-008 | Master bus EQ / FX. | L3 | SIG-12 |

### 3.15 Presets, projects, state (PRS)

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-PRS-001 | Data model: Sample → Patch (sample reference + all parameters, incl. flags, matrix, macros, FX) → Multi (4 layers + mix) → Project/Library; Pattern Library remains independent. | R1 | PRS-01, PRS-02; DEC-022 |
| GS-PRS-002 | Searchable desktop library with banks/tags/search, favourites, init patch from user-selectable default template and optional performance-bank recall view. | R1 — approved product behavior | PRS-03, PRS-07 [PI: tags/search]; DEC-022; PRESET_LIBRARY_SPECIFICATION.md §§2–4 |
| GS-PRS-003 | Templates (parameter set without sample) and "apply template" to current patch. | R1 | PRS-07 |
| GS-PRS-004 | Human-readable, versioned preset format with forward migration; parameter scaling versioned so presets never change sound across releases. | R1 | PRS-13, RN-10 [PROP]; Q-T15 |
| GS-PRS-005 | Portable sample referencing (relative path + content hash + relink dialog); "collect samples" export bundle. | R1 | PRS-12 → PRS-P1 [PROP] |
| GS-PRS-006 | Plugin state restores fully in the DAW, including unsaved recording buffers embedded in state; existing library samples may remain external references with relink/collect support. | R1 — approved product policy; technical serialization Phase 3 | DEC-025; PRESET_LIBRARY_SPECIFICATION.md §6 |
| GS-PRS-007 | Undo/redo of parameter and edit actions (deeper than the benchmark's revert-to-save). | R1 | PRF-05 [PROP] |
| GS-PRS-008 | Copy/paste of patches, layers, matrix rows, FX slots. | R1 | PRF-04 |
| GS-PRS-009 | Comments per patch. | R1 | PRS-08 |
| GS-PRS-010 | Import GR-1 / GR-MEGA patches. | OPT — blocked by Q-L01 | PRS-11 |

### 3.16 UI / UX (UI) — high-level only; detailed in Phase 4

| ID | Requirement | Proposed tier | Source / notes |
|---|---|---|---|
| GS-UI-001 | Main view centred on the sample waveform with live grain visualisation (benchmark philosophy: "focus on the central screen"). | R1 | p.12; EDT-13 |
| GS-UI-002 | Eight-knob engine section whose functions remap per engine, consistent with the benchmark table, so hardware MIDI controllers map 1:1. | R1 | ARCH-02, GRN-16 |
| GS-UI-003 | Window-shape display with density equation. | R1 | WIN-07 |
| GS-UI-004 | Resizable, HiDPI, keyboard-navigable UI; tooltips; value readout on hover. | R1 | PRF-03 [PROP] |
| GS-UI-005 | CPU meter by category (voices / FX / housekeeping). | R1 | PRF-07 |
| GS-UI-006 | Plugin UI and standalone UI are the same component. | R1 | GS-PLT-004 |
| GS-UI-007 | The UI shall provide a MIDI Learn / mapping system — global and per-preset mappings, conflict detection and recovery — sufficient to satisfy GS-MID-008. Controller feedback (motorized faders, LED rings/displays) is explicitly **not** an R1 requirement; it is deferred beyond R1. | **R1 — Approved** | DEC-016; scope narrowed by **DEC-021** (2026-11-27): no R1 MIDI feedback or named-controller template. Mapping architecture and conflict handling detailed in docs/LIVE_PERFORMANCE_SPECIFICATION.md; its acceptance criteria (§9 there) are **proposed**, not approved — final criteria are a Phase 5 deliverable per §8 below. Does not decide parameter-exposure scope (Q-T12, still OPEN). Q-P12 is **ANSWERED** by DEC-021: generic MIDI Learn/manual mapping is the R1 baseline, and no named-controller template or specific-controller auto-mapping is required in R1. |

---

## 4. Non-functional requirements

| ID | Requirement | Tier |
|---|---|---|
| GS-NFR-001 | (refines REQ-AUD-001/002) **Real-time safety:** no heap allocation, locks, file/network I/O, logging, Objective-C messaging or unbounded loops on the audio thread. Enforced by automated detection in debug/test builds. | R1 |
| GS-NFR-002 | (refines QA-001, QA-005) **No dropouts** at the reference load (defined in TESTING_STRATEGY.md after Q-T07) at 48 kHz / 128-sample buffer on the reference machine. | R1 |
| GS-NFR-003 | (refines REQ-AUD-004, QA-001) **No clicks** from: parameter changes (smoothed), voice/grain stealing, window changes, loop wrap with crossfade enabled, sample swap, preset change. | R1 |
| GS-NFR-004 | **Zero added latency** for granular, slice, sampler (and tape). Spectral latency = analysis block, reported. | R1 |
| GS-NFR-005 | (supports QA-002, QA-004) **Determinism:** all randomness from seedable per-instance RNGs; an offline render with fixed seed is bit-reproducible (enables golden-file tests). | R1 |
| GS-NFR-006 | (refines QA-003) **Parameter stability:** host-visible parameter IDs never change or get reused after first public release. | R1 |
| GS-NFR-007 | **Validation:** pass `auval` and pluginval (strictness 10) for every release build; tested in Logic Pro, Ableton Live, Bitwig, Reaper (list per Q-P09). | R1 |
| GS-NFR-008 | (refines REQ-AUD-003, QA-005) **Memory:** documented worst-case RAM per layer; no unbounded growth during recording (AUTO mode capped). | R1 |
| GS-NFR-009 | **Load time:** a 5-minute stereo sample loads (decode + analysis) without UI freeze; waveform visible progressively. | R1 |
| GS-NFR-010 | **Sample-rate independence:** musical results (pitch, timing, filter response, window shapes) identical in character across 44.1–192 kHz. | R1 |
| GS-NFR-011 | (refines QA-002, QA-004) **Automated DSP tests** for every DSP component (see TESTING_STRATEGY.md). | R1 |
| GS-NFR-012 | **Crash resilience:** corrupted preset or missing sample never crashes the host; user is told what is missing. | R1 |
| GS-NFR-013 | **macOS minimum version** per Q-T03; notarized, hardened-runtime signed builds. | R1 |

---

## 5. Explicit exclusions (benchmark features we will not replicate)

| Benchmark feature | Reason | Ref |
|---|---|---|
| Hardware I/O (CV/gate, HDMI, LAN, USB host, power management) | Hardware-only; replaced by host/OS facilities | HW-01…11 |
| Internal file manager, SMB file sharing, firmware updater | macOS Finder/updater replace them | HW-08, HW-10, HW-11 |
| 50 fixed project slots × 128 fixed preset slots | Desktop software uses an unbounded library; we keep bank/slot recall as a performance view only | PRS-03, PRS-06 |
| Button-combination UX, text entry with encoder | Desktop keyboard/mouse | UI-02 |
| Proprietary HLAC format | Proprietary | SMG-05 |
| Reproducing GR-MEGA firmware, code, graphics, factory content, or naming in a way that implies affiliation | IP — see Q-L01/Q-L03 | — |

---

## 6. Requirement summary

### 6.1 Counting method

The table in §6.2 is derived mechanically from the requirement rows in §3 and §4, not maintained by hand:

1. **Unit of count:** one table row whose first cell is exactly a `GS-<AREA>-<NNN>` id. Prose mentions, dependency-table references (§7) and exclusion-table rows (§5) are not counted.
2. **One row → exactly one bucket.** A row's tier cell is normalised by its leading token: `R1…` → **R1**, `L2…` → **L2**, `L3…` → **L3**, `OPT…` → **OPT**, anything else → **Other**. Trailing qualifiers do not change the bucket (`R1 — Approved`, `R1 — product scope approved`, `R1 (for R1 features)` all count as one R1 row).
3. **Split-tier rows are counted once**, under their primary (R1) tier, and are listed explicitly in §6.3 instead of being written as an inline "+1" annotation. No row is counted in two buckets, so **every area's buckets sum to that area's row count, and the area rows sum to the grand total.**
4. **Zero is written `0`**, not `—`, so each row and column can be added up directly.

**Reconciliation:** 124 + 27 + 9 + 8 + 0 = **168**, and the 17 area row-counts (6+10+23+7+6+7+2+6+10+21+12+12+8+8+10+7+13) also total **168**. The two agree, and both match the number of `GS-*` rows actually present in §3/§4.

### 6.2 Requirement counts

| Area | R1 | L2 | L3 | OPT | Other | Rows |
|---|---|---|---|---|---|---|
| Platform (PLT) | 4 | 0 | 0 | 2 | 0 | 6 |
| Architecture (ARC) | 8 | 1 | 0 | 1 | 0 | 10 |
| Granular (GRN) | 20 | 3 | 0 | 0 | 0 | 23 |
| Chord/quantization (CHD) | 5 | 1 | 0 | 1 | 0 | 7 |
| Slice (SLC) | 0 | 6 | 0 | 0 | 0 | 6 |
| Sampler (SMP) | 4 | 2 | 1 | 0 | 0 | 7 |
| Tape (TAP) | 0 | 2 | 0 | 0 | 0 | 2 |
| Spectral (SPC) | 0 | 0 | 5 | 1 | 0 | 6 |
| Voice/env/filter (VOI) | 9 | 1 | 0 | 0 | 0 | 10 |
| Samples/recording (SMG) | 16 | 5 | 0 | 0 | 0 | 21 |
| Modulation (MOD) | 9 | 1 | 1 | 1 | 0 | 12 |
| Sequencer/performance (SEQ) | 9 | 2 | 1 | 0 | 0 | 12 |
| MIDI (MID) | 7 | 1 | 0 | 0 | 0 | 8 |
| Effects (FX) | 4 | 2 | 1 | 1 | 0 | 8 |
| Presets/state (PRS) | 9 | 0 | 0 | 1 | 0 | 10 |
| UI (UI) | 7 | 0 | 0 | 0 | 0 | 7 |
| Non-functional (NFR) | 13 | 0 | 0 | 0 | 0 | 13 |
| **Total** | **124** | **27** | **9** | **8** | **0** | **168** |

### 6.3 Footnotes to the counts

- **Split-tier rows (3), each counted once under R1:** `GS-ARC-005` (R1/L — Granular + Sampler in R1, Slice/Tape/Spectral later), `GS-SMG-005` (R1 waveform / L2 spectrogram), `GS-MOD-004` (R1 sources; CV and Draw X/Y later).
- **"Other" (0):** every requirement row now has an approved tier; `GS-SEQ-010` is L2 by DEC-029/DEC-041.
- **Rows whose tier cell is explicitly marked Approved (2):** `GS-MID-008`, `GS-UI-007` (DEC-016, narrowed by DEC-021). DEC-041 approves the tier classification for all 168 rows; numeric technical values tied to Q-Txx remain proposals until their architecture/validation gates.
- **Phase 2 reconciliation (v1.0):** all 168 requirement rows have exactly one approved tier; `GS-SEQ-010` is L2 (DEC-029). Totals reconcile to 124 R1 + 27 L2 + 9 L3 + 8 OPT + 0 Other = 168. Split-tier rows are counted once under their primary R1 tier per §6.1.

## 7. Dependencies between requirement areas

| Area | Depends on | Why |
|---|---|---|
| All DSP areas | GS-NFR-001, parameter registry (Arch §9), Q-T01/Q-T12 | Real-time model and stable parameter IDs must exist first |
| GRN, SLC, SMP | SMG-001/003 (sample loading), ARC-002/003 (voice & grain pools) | Engines read immutable sample buffers via voices |
| SLC | GRN | Slice engine is the granular engine constrained to slices |
| TAP | SMP | Tape reuses sampler play-head, loop and interpolation code |
| SPC | Arch §7 FFT layer, Q-T10/Q-T11 | Separate STFT subsystem with latency policy |
| CHD | GRN scheduler, VOI | Chord distribution happens at grain spawn / voice level |
| MOD | Parameter registry, VOI, LFO/env DSP | Matrix writes to registered destinations per voice |
| MOD-008 macros | MOD-003 | Macros are matrix sources and destinations |
| SEQ | Transport abstraction (Arch §11), MOD (mod lanes, p-locks) | p-locks override registered parameters |
| SMG-008…012 (live input) | Q-P06 (plugin input bus), ARC-006/007 | Bus layout and AU component type |
| GS-SMG-021 (Freeze) | ARC-005 (engine/voice state), GS-MID-008 (controller reachability) | Freeze is switchable per layer and globally; its semantics are a Phase 3 architecture decision and are not specified at requirement level |
| FX | Parameter registry, ARC-007 | Slot host + registered FX params |
| PRS | Parameter registry, SMG (sample refs), Q-P07/Q-P08 | Serialization of every registered parameter + sample references |
| MID | Parameter registry, VOI (MPE voice mapping) | MIDI learn binds to parameter IDs; MPE binds to voices |
| GS-MID-008, GS-UI-007 (live performance) | GS-MID-003 (MIDI learn), parameter registry, UI | Controller mapping binds to registered parameters (controller feedback is not R1 — DEC-021); see docs/LIVE_PERFORMANCE_SPECIFICATION.md |
| UI | Everything above | UI reads state snapshots only |

## 8. Traceability

Each detailed requirement must have a unique ID, source/evidence classification, priority, rationale, dependencies and acceptance criteria (rule carried over from v0.1).

Current coverage:

| Attribute | Where it lives now |
|---|---|
| Unique ID | `GS-<AREA>-NNN` column |
| Source / evidence class | "Source / notes" column → feature-map IDs; the evidence class (Manual-confirmed / Interpretation / Proposal / Unverified) is the one recorded on that feature-map row; [RN] is a supplemental source marker only. Requirements without a benchmark source are marked [PROP]. |
| Priority | "Proposed tier" column (not approved) |
| Rationale | Notes column where non-obvious; otherwise parity with the cited benchmark feature |
| Dependencies | Area-level in §7; requirement-level in Phase 5 |
| Acceptance criteria | **Gap — Phase 5 deliverable** (TESTING_STRATEGY.md + per-milestone acceptance tests) |

*End of GRANULAR_SYNTH_REQUIREMENTS.md*


## 9. Phase 2 domain specifications and acceptance

The following documents expand behavior, edge cases and acceptance coverage without changing the GS requirement inventory or its §6 count:

- `MODULATION_SPECIFICATION.md` — GS-MOD-001…012; DEC-024 resolves Q-M11.
- `EFFECTS_SPECIFICATION.md` — GS-FX-001…008; DEC-023 resolves Q-P11.
- `SAMPLER_SPECIFICATION.md` — GS-SMP-001…007; Q-M05 remains open pending evidence.
- `PRESET_LIBRARY_SPECIFICATION.md` — GS-PRS-001…010; DEC-022 resolves Q-P07 and DEC-025 resolves the product policy in Q-P08.
- `MIDI_SPECIFICATION.md` — GS-MID-001…008; preserves DEC-016/021 and the controller-only operation set.
- `AUDIO_IO_SPECIFICATION.md` — GS-SMG-001…021 and relevant GS-PLT/GS-NFR requirements; Freeze semantics remain Phase 3.
- `R1_RELEASE_SCOPE_MATRIX.md` — consolidated tier view; pending owner approvals are called out explicitly.
- `R1_ACCEPTANCE_CRITERIA.md` — testable product-level pass conditions; numeric performance thresholds and implementation-level test design remain Phase 3/5.

No GS requirement IDs were added or removed by this Phase 2 specification pass; §6 totals remain based on the existing 168 requirement rows.

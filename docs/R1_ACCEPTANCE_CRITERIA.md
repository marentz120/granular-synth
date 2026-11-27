# R1 Acceptance Criteria — Phase 2 Draft

**Status:** Testable product-level criteria drafted in Phase 2; test harness, numeric thresholds and architecture-dependent tests are Phase 5/3 work.
**Authority:** GRANULAR_SYNTH_REQUIREMENTS.md, R1_PRODUCT_SPECIFICATION.md, domain specifications in this branch.
**Important:** A criterion for a proposed feature does not make that feature approved R1 scope.

## 1. Modulation

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-MOD-001 | Four LFOs per layer | Each of four layers exposes four independently configurable LFOs; changing one layer does not alter another | GS-MOD-001 |
| AC-MOD-002 | LFO functions | Supported rate modes, waveforms, phase, amplitude quantization, polarity and key-sync modes produce defined repeatable output; Poly/Mono is selectable per LFO and defaults to Mono | GS-MOD-001, GS-MOD-002; DEC-024 |
| AC-MOD-003 | Matrix capacity | At least 50 rows can be created, enabled/disabled, edited, copied, pasted and deleted | GS-MOD-003, GS-MOD-007 |
| AC-MOD-004 | Source/destination coverage | All specified R1 sources and eligible continuous R1 destinations can be selected; unsupported structural parameters cannot | GS-MOD-004, GS-MOD-005 |
| AC-MOD-005 | Quick assignment | User can assign a destination using the documented quick-assignment gesture | GS-MOD-006 |
| AC-MOD-006 | Macro behavior | Exactly ten R1 macros per patch support multiple destinations, ranges, curve and invert; macro values can be modulated | GS-MOD-008; DEC-036 |
| AC-MOD-007 | Modulation display | Controls show summed modulation and recent-destination monitor reflects active destinations | GS-MOD-009 |
| AC-MOD-008 | Random determinism | Random 1–4 are independent per note/layer and reproducible under fixed test seeds | GS-MOD-004 |
| AC-MOD-009 | Envelope follower | Follower responds to selected input/layer source and attack/release/gain settings | GS-MOD-004 |
| AC-MOD-010 | XY pad | X and Y operate as independent normalized 0.0–1.0 sources; new patch defaults to (0.5, 0.5) and saved patch recalls its stored position | GS-MOD-004; DEC-032 |

## 2. Effects

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-FX-001 | Four-slot chain | Every layer has four independent serial slots | GS-FX-001 |
| AC-FX-002 | Reorder | Reordering moves the complete effect state and does not reset parameters | GS-FX-001 |
| AC-FX-003 | Slot mix and sends | Each slot's dry/wet and layer synth/audio-input sends operate independently | GS-FX-001 |
| AC-FX-004 | Effect inventory | Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator and Compressor are available and produce their documented behavior | GS-FX-002; DEC-023 |
| AC-FX-005 | Modulation/automation | Every eligible continuous parameter responds to modulation and automation within its legal range | GS-FX-004 |
| AC-FX-006 | Tail behavior | Preset switch follows selected Linger or Cut mode | GS-FX-006 |
| AC-FX-007 | Safe transitions | Bypass, replacement and preset recall do not crash or produce uncontrolled discontinuities | GS-FX-006; GS-NFR-003 |

## 3. Sampler

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-SMP-001 | Engine selection | Each layer can independently select Sampler | DEC-017/018; GS-ARC-005 |
| AC-SMP-002 | Pitch/playback | Pitch changes playback speed and duration as specified; no time-stretch is implied | GS-SMP-001 |
| AC-SMP-003 | Playback modes | One-shot, loop and ping-pong respect valid playback/loop boundaries | GS-SMP-002 |
| AC-SMP-004 | Scan and spray | Scan, spray, pan spray and direction affect new note starts/playback as specified | GS-SMP-001 |
| AC-SMP-005 | Interpolation | High-quality interpolation option operates without out-of-bounds reads or unstable output | GS-SMP-003 |
| AC-SMP-006 | Env pile-up | Toggle changes same-note retrigger behavior as documented; Sampler default is OFF | GS-SMP-004; DEC-037 |
| AC-SMP-007 | Loop release | ON continues looping through voice release; OFF exits at the next valid loop boundary and continues toward Stop | GS-SMP-002; DEC-030 |
| AC-SMP-008 | Missing source | Empty/missing sample produces safe silence/status and never crashes | GS-NFR-012 |

## 4. Presets, projects and library

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-PRS-001 | Browser | User can browse by bank, tag, search and favourite state | GS-PRS-002 |
| AC-PRS-002 | Data boundaries | Patch, Multi, Pattern, Performance Set, Template and Project save/recall preserve their respective state | GS-PRS-001, GS-PRS-003 |
| AC-PRS-003 | Pattern independence | Recalling a Preset does not replace the active Pattern | R1_PRODUCT_SPECIFICATION.md §4.4 |
| AC-PRS-004 | Template application | Applying a sample-free template does not overwrite sample-specific state unless explicitly requested | GS-PRS-003 |
| AC-PRS-005 | Portable assets | Collect/export includes all referenced samples; relative paths and hashes resolve correctly | GS-PRS-005 |
| AC-PRS-006 | Relink | Missing or changed samples are detected and can be relinked; filename alone does not silently substitute content | GS-PRS-005 |
| AC-PRS-007 | Plugin state | DAW state restores full patch state and recorded unsaved buffers under the approved persistence policy | GS-PRS-006; Q-P08 |
| AC-PRS-008 | Version migration | Supported older preset schema migrates without unintended parameter/sound change | GS-PRS-004 |
| AC-PRS-009 | Undo/copy | Undo/Redo and copy/paste preserve complete relevant state | GS-PRS-007, GS-PRS-008 |
| AC-PRS-010 | Corrupt data | Corrupt preset or missing asset never crashes host and produces actionable status | GS-NFR-012 |

## 5. MIDI

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-MID-001 | Layer channels | Four layers independently receive on configured MIDI channels; if multiple layers share a channel, all matching layers receive the event | GS-MID-001; DEC-034 |
| AC-MID-002 | MPE | Per-note pitch (48-semitone range), pressure and timbre remain associated with the correct note/voice | GS-MID-002; DEC-038 |
| AC-MID-003 | High-resolution control | 14-bit CC/NRPN input updates mapped parameter correctly; malformed/incomplete messages are ignored safely | GS-MID-003 |
| AC-MID-004 | Soft takeover | Mapped control does not jump before physical value crosses/picks up current software value | GS-MID-003 |
| AC-MID-005 | Program Change | Program Change recalls the explicitly assigned item in the active performance bank; unmapped values are ignored | GS-MID-005; DEC-033 |
| AC-MID-006 | Channel-mode | All Notes Off, All Sound Off and Reset Controllers have distinct documented effects | GS-MID-006 |
| AC-MID-007 | MIDI Clock | Start resets, Continue resumes and Stop halts; loss of pulses for two expected beat intervals stops clock-dependent progression at the current position; SPP is ignored in R1 | GS-MID-007; GS-SEQ-005; DEC-039 |
| AC-MID-008 | Controller-only | Every essential live operation in LIVE_PERFORMANCE_SPECIFICATION.md §1 is reachable without keyboard/mouse | GS-MID-008 |
| AC-MID-009 | Feedback exclusion | R1 acceptance does not require MIDI feedback or named-controller templates | DEC-021 |

## 6. Audio, I/O and recording

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-AUD-001 | Device setup | Standalone can select input/output devices and map available channels | GS-PLT-001; audio spec §2 |
| AC-AUD-002 | Device loss | Device removal/change leads to safe silent/stopped state, user notification and recoverable reconnect | audio spec §2 |
| AC-AUD-003 | Layer input | Live input can be assigned independently to each of four layers | DEC-020; GS-SMG-012 |
| AC-AUD-004 | Input controls | Each layer's input level and wet/dry controls operate independently | GS-SMG-012 |
| AC-AUD-005 | Freeze controls | Per-layer and global Source Snapshot Freeze controls exist and are controller-reachable; global action targets all four layers at one logical boundary | GS-SMG-021; DEC-042 |
| AC-AUD-015 | Freeze capture | Freeze captures a stable copy of the current valid source region, respecting active sample/crop/loop bounds; rolling buffers use only committed valid frames | DEC-042 |
| AC-AUD-016 | Frozen playback | Existing grains/voices continue; new MIDI notes and sequencer events use the frozen snapshot | DEC-042 |
| AC-AUD-017 | Engine continuity | Transport, envelopes, LFOs, modulation, MIDI and sequencer continue normally while frozen | DEC-042 |
| AC-AUD-018 | FX continuity | Layer effects and effect tails continue processing while frozen | DEC-042 |
| AC-AUD-019 | Recording independence | Independent recording continues at its defined tap; Freeze does not implicitly start/stop recording and does not create a read/write race | DEC-042 |
| AC-AUD-020 | Thaw | Thaw restores the prior live/rolling source using a click-safe crossfade | DEC-042 |
| AC-AUD-021 | Global Freeze | All four layers transition at the same logical boundary; partial failure is reported accurately and never presented as full success | DEC-042 |
| AC-AUD-022 | Static sample | An already immutable library sample reports “already static” or disables redundant Freeze capture without allocating a duplicate full copy | DEC-042 |
| AC-AUD-023 | Failure handling | Missing/disconnected source or memory exhaustion leaves the prior valid source intact, reports failure and remains recoverable | DEC-042 |
| AC-AUD-024 | Persistence | Freeze state and frozen snapshot restore with plugin state; snapshot audio is embedded under DEC-025 policy | DEC-025; DEC-042 |
| AC-AUD-025 | Panic behavior | All Sound Off silences voices/effect tails but does not silently thaw the frozen source | DEC-042 |
| AC-AUD-026 | RT safety | Freeze/thaw audio-thread transition performs no allocation, blocking, file I/O or unbounded work; snapshot preparation and retirement occur off-thread | DEC-042; GS-NFR-001 |
| AC-AUD-027 | Rolling-source retention | During ongoing recording and forced ring-wrap pressure, Freeze copies exactly the committed source region under a retention lease; the recorder continues in a separate writable region; if retention cannot be reserved, Freeze fails before source switch and preserves valid source/capture without a data race | DEC-042; GS-NFR-001, GS-NFR-008 |
| AC-AUD-028 | Global Freeze transaction | **Proposed pending owner review:** with mixed per-layer preparation success/failure, all ready layers switch at the same logical boundary, failed layers remain unchanged, transaction publication failure changes none, and per-layer/partial status is accurate; confirm successful-subset behavior versus all-or-nothing before implementation | DEC-042 |
| AC-AUD-006 | Layer recording | Each layer can be recorded independently and saved/exported as reusable audio | GS-SMG-020 |
| AC-AUD-007 | Master recording | Stereo master mix can be recorded and exported | GS-SMG-020 |
| AC-AUD-008 | Record modes | One-shot, loop, scrolling record, AUTO/fixed buffer and mono/stereo initialization operate as specified | GS-SMG-008, GS-SMG-009, GS-SMG-010, GS-SMG-011 |
| AC-AUD-009 | Monitor while recording | Granular and Sampler can play/process the recording buffer while capture continues | GS-SMG-009 |
| AC-AUD-010 | Keep/discard/export | User can retain, discard and export valid recorded audio; discarded buffers do not remain hidden dependencies | GS-SMG-017, GS-SMG-020 |
| AC-AUD-011 | Resource exhaustion | Full buffer/resource exhaustion stops safely, preserves valid capture and informs user | GS-NFR-008 |
| AC-AUD-012 | File formats | WAV, AIFF and FLAC load; WAV export is valid | GS-SMG-001, GS-SMG-017 |
| AC-AUD-013 | RT safety | Audio callback performs no allocation, lock, file/network I/O, logging or blocking wait | GS-NFR-001 |
| AC-AUD-014 | Sample-rate behavior | Musical behavior remains consistent across the sample-rate matrix validated and approved in Phase 3/5 | GS-NFR-010; Q-T17 |

## 7. Cross-domain release gates

- Host-visible parameter IDs must be stable once publicly released (GS-NFR-006); parameter exposure and ID scheme are Phase 3 decisions.
- Plugin/Standalone shared DSP/state behavior must be consistent for identical input/state (GS-PLT-004).
- No audio-thread allocation, lock, I/O or unbounded wait (GS-NFR-001).
- No clicks from parameter changes, sample swaps, preset changes, voice/grain stealing and window changes (GS-NFR-003).
- No-dropout reference load and exact CPU/RAM/latency thresholds are defined only after Phase 3 profiling and Phase 5 test planning (GS-NFR-002/008).
- AU validation and pluginval/host compatibility tests follow GS-NFR-007 and Q-P09.

## 8. Phase 5 items still required

- Reference machine and exact OS/host versions.
- Numeric CPU, RAM, latency and load-time thresholds.
- Test fixture samples, deterministic seeds and golden reference renders.
- Stress profiles for four layers, voice/grain load, FX chains, recording and modulation.
- Host-specific plugin state and live-input test matrix.
- Freeze semantic tests follow the approved contract in `FREEZE_BEHAVIOR_SPECIFICATION.md` v1.0 (DEC-042), including rolling-ring wrap/retention and global transaction failure injection; numeric snapshot/resource thresholds remain Phase 5 validation.

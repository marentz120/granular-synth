# R1 Acceptance Criteria — Phase 2 Draft

**Status:** Testable product-level criteria drafted in Phase 2; test harness, numeric thresholds and architecture-dependent tests are Phase 5/3 work.
**Authority:** GRANULAR_SYNTH_REQUIREMENTS.md, R1_PRODUCT_SPECIFICATION.md, domain specifications in this branch.
**Important:** A criterion for a proposed feature does not make that feature approved R1 scope.

## 1. Modulation

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-MOD-001 | Four LFOs per layer | Each of four layers exposes four independently configurable LFOs; changing one layer does not alter another | GS-MOD-001 |
| AC-MOD-002 | LFO functions | Supported rate modes, waveforms, phase, amplitude quantization, polarity and key-sync modes produce defined repeatable output | GS-MOD-001 |
| AC-MOD-003 | Matrix capacity | At least 50 rows can be created, enabled/disabled, edited, copied, pasted and deleted | GS-MOD-003/007 |
| AC-MOD-004 | Source/destination coverage | All specified R1 sources and eligible continuous R1 destinations can be selected; unsupported structural parameters cannot | GS-MOD-004/005 |
| AC-MOD-005 | Quick assignment | User can assign a destination using the documented quick-assignment gesture | GS-MOD-006 |
| AC-MOD-006 | Macro behavior | Ten macros per patch support multiple destinations, ranges, curve and invert; macro values can be modulated | GS-MOD-008 |
| AC-MOD-007 | Modulation display | Controls show summed modulation and recent-destination monitor reflects active destinations | GS-MOD-009 |
| AC-MOD-008 | Random determinism | Random 1–4 are independent per note/layer and reproducible under fixed test seeds | GS-MOD-004 |
| AC-MOD-009 | Envelope follower | Follower responds to selected input/layer source and attack/release/gain settings | GS-MOD-004 |
| AC-MOD-010 | XY pad | X and Y operate as independent modulation sources | GS-MOD-004 |

## 2. Effects

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-FX-001 | Four-slot chain | Every layer has four independent serial slots | GS-FX-001 |
| AC-FX-002 | Reorder | Reordering moves the complete effect state and does not reset parameters | GS-FX-001 |
| AC-FX-003 | Slot mix and sends | Each slot's dry/wet and layer synth/audio-input sends operate independently | GS-FX-001 |
| AC-FX-004 | Effect inventory | Every effect in the owner-approved R1 set is available and produces its documented behavior | GS-FX-002; Q-P11 |
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
| AC-SMP-006 | Env pile-up | Enabled/disabled modes produce the documented same-note retrigger behavior | GS-SMP-004 |
| AC-SMP-007 | Loop release | ON/OFF behavior passes only after Q-M05 is resolved and evidence is attached | GS-SMP-002; Q-M05 |
| AC-SMP-008 | Missing source | Empty/missing sample produces safe silence/status and never crashes | GS-NFR-012 |

## 4. Presets, projects and library

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-PRS-001 | Browser | User can browse by bank, tag, search and favourite state | GS-PRS-002 |
| AC-PRS-002 | Data boundaries | Patch, Multi, Pattern, Performance Set, Template and Project save/recall preserve their respective state | GS-PRS-001/003 |
| AC-PRS-003 | Pattern independence | Recalling a Preset does not replace the active Pattern | R1_PRODUCT_SPECIFICATION.md §4.4 |
| AC-PRS-004 | Template application | Applying a sample-free template does not overwrite sample-specific state unless explicitly requested | GS-PRS-003 |
| AC-PRS-005 | Portable assets | Collect/export includes all referenced samples; relative paths and hashes resolve correctly | GS-PRS-005 |
| AC-PRS-006 | Relink | Missing or changed samples are detected and can be relinked; filename alone does not silently substitute content | GS-PRS-005 |
| AC-PRS-007 | Plugin state | DAW state restores full patch state and recorded unsaved buffers under the approved persistence policy | GS-PRS-006; Q-P08 |
| AC-PRS-008 | Version migration | Supported older preset schema migrates without unintended parameter/sound change | GS-PRS-004 |
| AC-PRS-009 | Undo/copy | Undo/Redo and copy/paste preserve complete relevant state | GS-PRS-007/008 |
| AC-PRS-010 | Corrupt data | Corrupt preset or missing asset never crashes host and produces actionable status | GS-NFR-012 |

## 5. MIDI

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-MID-001 | Layer channels | Four layers independently receive on configured MIDI channels; shared-channel behavior is deterministic | GS-MID-001 |
| AC-MID-002 | MPE | Per-note pitch, pressure and timbre remain associated with the correct note/voice | GS-MID-002 |
| AC-MID-003 | High-resolution control | 14-bit CC/NRPN input updates mapped parameter correctly; malformed/incomplete messages are ignored safely | GS-MID-003 |
| AC-MID-004 | Soft takeover | Mapped control does not jump before physical value crosses/picks up current software value | GS-MID-003 |
| AC-MID-005 | Program Change | Configured Program Change recalls its assigned item; unmapped values do not recall arbitrary content | GS-MID-005 |
| AC-MID-006 | Channel-mode | All Notes Off, All Sound Off and Reset Controllers have distinct documented effects | GS-MID-006 |
| AC-MID-007 | MIDI Clock | Standalone MIDI Clock synchronizes the shared transport; clock loss does not create runaway timing | GS-MID-007; GS-SEQ-005 |
| AC-MID-008 | Controller-only | Every essential live operation in LIVE_PERFORMANCE_SPECIFICATION.md §1 is reachable without keyboard/mouse | GS-MID-008 |
| AC-MID-009 | Feedback exclusion | R1 acceptance does not require MIDI feedback or named-controller templates | DEC-021 |

## 6. Audio, I/O and recording

| Test ID | Requirement | Pass condition | Trace |
|---|---|---|---|
| AC-AUD-001 | Device setup | Standalone can select input/output devices and map available channels | GS-PLT-001; audio spec §2 |
| AC-AUD-002 | Device loss | Device removal/change leads to safe silent/stopped state, user notification and recoverable reconnect | audio spec §2 |
| AC-AUD-003 | Layer input | Live input can be assigned independently to each of four layers | DEC-020; GS-SMG-012 |
| AC-AUD-004 | Input controls | Each layer's input level and wet/dry controls operate independently | GS-SMG-012 |
| AC-AUD-005 | Freeze controls | Per-layer and global Freeze controls exist and are controller-reachable; semantic tests follow Phase 3 definition | GS-SMG-021 |
| AC-AUD-006 | Layer recording | Each layer can be recorded independently and saved/exported as reusable audio | GS-SMG-020 |
| AC-AUD-007 | Master recording | Stereo master mix can be recorded and exported | GS-SMG-020 |
| AC-AUD-008 | Record modes | One-shot, loop, scrolling record, AUTO/fixed buffer and mono/stereo initialization operate as specified | GS-SMG-008…011 |
| AC-AUD-009 | Monitor while recording | Granular and Sampler can play/process the recording buffer while capture continues | GS-SMG-009 |
| AC-AUD-010 | Keep/discard/export | User can retain, discard and export valid recorded audio; discarded buffers do not remain hidden dependencies | GS-SMG-017/020 |
| AC-AUD-011 | Resource exhaustion | Full buffer/resource exhaustion stops safely, preserves valid capture and informs user | GS-NFR-008 |
| AC-AUD-012 | File formats | WAV, AIFF and FLAC load; WAV export is valid | GS-SMG-001/017 |
| AC-AUD-013 | RT safety | Audio callback performs no allocation, lock, file/network I/O, logging or blocking wait | GS-NFR-001 |
| AC-AUD-014 | Sample-rate behavior | Musical behavior remains consistent across the approved sample-rate matrix | GS-NFR-010; Q-T17 |

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
- Freeze semantic tests after Phase 3 defines Freeze behavior.

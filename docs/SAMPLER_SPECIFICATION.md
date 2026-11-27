# Sampler Specification

**Version:** 0.1 — Phase 2 working specification  
**Status:** R1 product behavior specification; product scope decisions approved by DEC-030/037. No DSP implementation authorized.  
**Authority:** GRANULAR_SYNTH_REQUIREMENTS.md GS-SMP-001…007 and GS-SMG-001…021; DEC-017…020, DEC-030 and DEC-037.  
**Benchmark evidence:** GR_MEGA_FEATURE_MAP.md §4 and §9.3.

## 1. R1 role and boundary

Sampler is one of exactly two R1 engines. Each of the four independent layers may select Granular or Sampler independently. Sampler is a conventional sample playback engine: one playback head per voice, with pitch changing playback speed. It is not time-stretched. Chord mode can create up to 12 playback heads per voice according to the benchmark behavior, subject to the product's later measured resource limits.

Sampler shares the layer's sample asset, sample flags, recording buffer, voice/envelope model, modulation, MIDI channel and layer FX chain. It does not inherit granular-only parameters as active sampler controls.

## 2. Playback behavior

- A note-on creates a playback head at the selected scan/spawn position.
- Pitch changes playback rate and therefore duration, preserving the conventional sampler model rather than time-stretching.
- Direction control selects forward/reverse playback behavior as supported by the engine.
- Spray randomizes playback start position per note; Pan Spray randomizes stereo placement; their distribution details remain subject to the source's under-documented items.
- Scan offsets the central spawn position for new notes and respects the sample's active crop/loop bounds.
- Every active voice's playhead is visible on the waveform.
- Scan mode supports One-shot, Loop and Bounce/Ping-pong, as specified by GS-SMP-002 and GS-SMG-006.
- Sample Start/Stop and Loop Start/Loop End flags confine sampler playback heads, unlike granular/spectral engines where flags confine scan position.

## 3. Loop and release behavior

The R1 requirement includes one-shot, loop, ping-pong and looped release. The manual's descriptions of the looped-release ON/OFF semantics conflict between pages 58–59 and 102–103. This specification deliberately does not choose one interpretation.

Required product-level outcomes:
- One-shot playback stops at the Stop boundary.
- Loop playback wraps within Loop Start and Loop End.
- Ping-pong playback reverses direction at the loop boundaries.
- Note-off follows the configured looped-release policy.
- A loop boundary change takes effect on the next note, consistent with the mapped benchmark description.
- Loop boundary transitions must avoid clicks under GS-NFR-003.

The exact meaning of looped-release ON and OFF must be settled through source verification or an explicit product decision before implementation.

## 4. Voice and envelope behavior

- One sampler playback head is associated with each active voice.
- Chord mode may expand playback to up to 12 heads per voice as benchmark reference behavior.
- Env Pile-up controls same-note retrigger behavior. When enabled, same-note retrigger reuses the existing voice; R1 default is OFF for Sampler (DEC-037).
- Voice stealing follows GS-VOI-001, not a sampler-specific replacement policy.
- Sustain/hold, glide, velocity, pitch bend and envelopes follow the shared R1 voice requirements.
- Polyphony and chord head limits are ultimately subject to measured resource ceilings; no new global cap is invented here.

## 5. Interpolation and audio quality

- R1 includes a high-quality polyphase/windowed-sinc interpolation option (GS-SMP-003).
- The benchmark documents polyphase anti-aliasing for sampler and tape.
- The exact interpolation modes, default mode, filter length, transition behavior and CPU trade-off are implementation decisions to be made in Phase 3 and tested in Phase 5.
- Pitch and playback behavior must be sample-rate independent in musical character under GS-NFR-010.
- Sample replacement and parameter changes must be click-safe under GS-NFR-003.

## 6. Sample assets and recording buffers

Sampler uses the common sample management model:
- Load WAV, AIFF and FLAC as required by GS-SMG-001.
- Support mono and stereo samples.
- Sample loading must not block or glitch audio; old sample remains active until the replacement is ready (GS-SMG-003).
- Use common waveform, crop flags and recording buffer controls.
- Play the active recording buffer while recording (GS-SMG-009).
- Per-layer recording and stereo master-mix recording are R1 (GS-SMG-020).
- Per-layer and global Freeze are R1 product requirements (GS-SMG-021), but Freeze semantics are intentionally deferred to Phase 3. This document does not claim Freeze is a sampler loop, hold, buffer latch or playback stop.

## 7. Persistence

- Sampler engine selection and all sampler-specific parameters are stored in the layer Patch.
- Sample reference, content hash and relink behavior follow GS-PRS-005.
- Recorded buffer persistence and plugin state follow GS-PRS-006 and Q-P08.
- The sample asset and patch must be restored coherently; missing assets must not crash the host (GS-NFR-012).

## 8. Edge cases

- Playback reaching Stop in One-shot mode ends the voice according to its envelope/release policy.
- Playback reaching a loop boundary follows Loop or Bounce mode deterministically.
- Reverse playback must honor the same active bounds.
- A zero-length or invalid crop region must be rejected safely and surfaced to the user.
- Missing sample data must not crash the host.
- Replacing a sample while voices are active must follow the common click-safe sample-swap behavior.
- No behavior is inferred for unsupported multisample/velocity-zone formats.

## 9. Unresolved questions and deferred items

| Item | Status |
|---|---|
| Looped release ON/OFF meaning | **ANSWERED — DEC-030:** ON continues looping through voice release; OFF exits at next valid loop boundary toward Stop. This is GRANULAR SYNTH behavior, not a verified benchmark claim. |
| Env Pile-up default | **ANSWERED — DEC-037:** OFF by default for Sampler; user may enable per layer |
| Scan offset exact mathematical behavior | Partially documented; refine in DSP specification |
| Spray and Pan Spray distribution | Q-I08 / source map under-documented |
| Interpolation mode choices/default | Phase 3/5 |
| Maximum sample length 60 min | L2 (GS-SMP-005), not R1 |
| Zero-crossing snap and loop crossfade | L2 (GS-SMP-006), not R1 |
| SFZ multisample import with velocity layers | L3 (GS-SMP-007), not R1 |
| Granular Slice engine | Deferred; not a Sampler variant for R1 |

## 10. Acceptance intent

Phase 5 tests must cover pitch-as-speed, forward/reverse playback, scan and spray, pan spray, all loop modes, loop boundaries, Loop Release ON/OFF behavior per DEC-030, env pile-up, voice stealing, interpolation quality, mono/stereo sources, sample swap, buffer playback while recording, persistence and missing-sample recovery. Numerical CPU and memory limits are not set here.

# Audio, I/O and Recording Specification

**Version:** 0.1 — Phase 2 working specification  
**Status:** Proposed; subject to owner review. No device API, plugin bus or DSP architecture decision authorized.  
**Authority:** R1_PRODUCT_SPECIFICATION.md §3; GRANULAR_SYNTH_REQUIREMENTS.md GS-PLT, GS-SMG, GS-NFR and signal-flow requirements; DEC-015…021.  
**Benchmark evidence:** GR_MEGA_FEATURE_MAP.md §1.2 and §9.1–9.3.

## 1. Scope

This document consolidates user-visible audio input, monitoring, recording, output and device behavior for Standalone and plugin contexts. It does not decide AU/VST3 component types, host bus layout, audio APIs, thread boundaries, sample-buffer architecture, or numerical resource budgets; those belong to Phase 3/5.

R1 product scope includes live audio input per layer, independent per-layer Freeze and global Freeze, per-layer recording, stereo master-mix recording, and saving/exporting captured audio for later sample use (DEC-020). Exact Freeze semantics remain a Phase 3 architecture decision.

## 2. Product audio paths

- Each of the four layers can receive the product's live audio input path.
- Each layer has its own engine selection, voice path, modulation and FX chain.
- Live input has input-level, wet and dry controls in Standalone and plugin contexts (GS-SMG-012).
- Synthesized layer signal and live-input signal can be routed to the layer FX chain as defined in GS-FX-001.
- The product has a stereo master output and four stereo layer outputs at the product routing level (GS-ARC-006); exact plugin buses and standalone device channel assignment remain Phase 3.
- Per-layer recording captures the selected layer output; master recording captures the stereo master mix.
- Captured audio can be saved/exported and later loaded as a sample (GS-SMG-017, GS-SMG-020).

## 3. Standalone device behavior

The Standalone application must let the user select audio input and output devices and configure available channel routing. Device selection UI and platform API are Phase 3/4. Product behavior requirements:
- Show the active device and its availability.
- Handle device disconnect/reconnect without crashing.
- If the selected device disappears, stop using invalid I/O resources, notify the user and provide a recoverable device-selection path.
- Device changes must not corrupt saved patches or recordings.
- Input channels that are unavailable are treated as silent; synthesis and other available paths remain functional where possible.
- Sample-rate changes must preserve musical pitch/timing behavior as required by GS-NFR-010.

## 4. Plugin behavior

- Plugin live-input behavior must be supported where the host and plugin format provide the required input channels (GS-SMG-012).
- The product must clearly communicate when the host does not expose a usable input bus; the absence of host input must not crash or disable synthesis.
- Main stereo output and four stereo layer outputs are required at the product level (GS-ARC-006); exact AU/VST3 bus declarations and host compatibility are Phase 3.
- Plugin state must restore completely, including recorded unsaved buffers (GS-PRS-006); embedding versus reference is Q-P08/Phase 3.
- Host transport synchronization follows GS-SEQ-005.

## 5. Recording modes and controls

R1:
- Record into the current layer buffer.
- One-shot recording and continuous circular/loop recording.
- Configurable buffer size: AUTO or fixed duration; fixed size can be expressed in seconds or bars where supported by the product's timing model (GS-SMG-011).
- Initialize an empty mono or stereo recording buffer (benchmark-derived behavior in REC-10/11).
- Input gain and mono channel duplication are benchmark-derived controls currently represented by REC-03/04 and GS-SMG-008.
- Granular and Sampler engines can play the buffer while recording (GS-SMG-009).
- Scrolling record is included in R1 (GS-SMG-010): playback head can remain at an adjustable distance behind the record head; exact latency control range is TBD.
- Each layer can be recorded independently; stereo master mix can be recorded independently (GS-SMG-020).
- Captured audio can be retained/exported for later sample use.

The following remain later-tier according to current requirements and must not be silently promoted:
- Input-threshold and transport-start arming (GS-SMG-013, L2).
- Clocked bar-length recording (GS-SMG-014, L2).
- Resampling instrument output as a new sample/WAV (GS-SMG-015, L2).
- Synth-to-record-buffer feedback (GS-SMG-016, L2).

## 6. Record lifecycle

Product-visible record lifecycle:
1. Select target layer or master stereo mix.
2. Select/initialize buffer configuration.
3. Arm and start recording using the R1-supported start behavior.
4. Monitor/play the buffer while recording where supported.
5. Stop recording.
6. Retain or discard the captured take; retained audio can be saved/exported.
7. The retained sample becomes available for later sample loading/use.

The exact UI, record protection modes, background-file behavior and take-management workflow are Phase 4 details. Do not assume benchmark-specific UI controls are required.

## 7. Channel layout and format

- R1 sample import supports WAV, AIFF and FLAC; mono or stereo; arbitrary sample rate handled by resampling or playback rate compensation (GS-SMG-001).
- Mono input may be duplicated to stereo according to the selected channel policy; the benchmark documents LL/RR duplication. The final R1 default remains to be decided.
- Stereo input to mono buffer behavior must be explicitly defined; no silent channel discard.
- Sample rate support proposal is 44.1–192 kHz and buffer sizes 16–4096, including variable block sizes (GS-PLT-003). These values are proposed and require technical confirmation in Phase 3.
- Audio processing must be real-time safe and must not perform blocking I/O or routine heap allocation in the audio callback (REQ-AUD-001/002; GS-NFR-001).

## 8. Freeze

R1 includes independently switchable Freeze per layer and global Freeze across all layers (GS-SMG-021; DEC-020). Freeze must be controller-reachable under GS-MID-008.

This specification intentionally does not define what Freeze captures, holds, suspends, continues, or how it interacts with input, recording, sequencer, voices, FX tails or transport. Those semantics are explicitly reserved for Phase 3 architecture and must be decided before implementation.

## 9. Failure and overload behavior

- Missing input channel: treat as silence and show unavailable input state.
- Device loss: fail safely, notify, allow recovery; no host/app crash.
- Recording storage failure: stop or suspend recording safely, preserve any valid completed data, notify the user; never corrupt unrelated project state.
- Buffer exhaustion: deterministic, bounded behavior with visible warning; no unbounded memory growth (GS-NFR-008).
- CPU overload: safe deterministic degradation according to measured budgets, with no clicks/dropouts at the agreed reference load (GS-NFR-002/003). Actual budgets are not specified here.
- Sample loading and recording file operations must not block the audio callback.
- Invalid channel configurations must be rejected or fall back explicitly, never route arbitrary channels.

## 10. Unresolved questions and later gates

| Question | Status |
|---|---|
| Q-P06 / AU live-input host behavior | Phase 3 host/format validation |
| Q-P08 | Embed versus reference recorded buffers in plugin state; Phase 3 |
| Mono duplication default and stereo-to-mono policy | Product behavior to finalize |
| Scrolling-record distance range and latency semantics | Exact range TBD; no benchmark cap assumed |
| Device loss/reconnect UX | Phase 3/4 |
| Four layer output routing and bus declarations | Product requirement exists; host implementation Phase 3 |
| Freeze semantics | Phase 3; explicitly unresolved |
| Recording max duration and memory budget | Phase 3/5 profiling |
| Threshold/transport arming, clocked record, resampling, feedback | L2 as currently specified |

## 11. Acceptance intent

Tests must cover Standalone device selection/loss/reconnect, plugin live-input availability/fallback, mono/stereo channel mapping, supported sample-rate and variable-block behavior, per-layer input routing, per-layer and master recording, buffer playback during recording, scrolling-record behavior, save/export/reload, storage failure recovery, bounded memory growth, real-time safety and full plugin-state restoration. Freeze acceptance must be written only after its Phase 3 semantics are approved.

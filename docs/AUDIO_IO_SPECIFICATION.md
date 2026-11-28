# Audio, I/O and Recording Specification

**Status:** Phase 2 product behavior specification; host/device implementation remains Phase 3.
**Authority:** DEC-020 and DEC-040; R1_PRODUCT_SPECIFICATION.md §3; GS-SMG-001…021; GS-PLT-003/006; GS-NFR-001…013; GR_MEGA_FEATURE_MAP.md §1.2 and §9.

## 1. Product behavior and scope

- Standalone owns audio-device selection and device I/O.
- Plugin builds receive audio/MIDI and expose audio outputs according to host-format support; exact AU/VST3 component and bus layout is Phase 3.
- Live input is available independently to each of the four layers in Standalone and plugin contexts (DEC-020).
- R1 supports recording each layer independently and recording the stereo master mix.
- Each layer has an independently switchable Freeze and there is a global Freeze control. Source Snapshot capture, continuation, thaw, recording independence and failure behavior are approved by DEC-042 and specified in FREEZE_BEHAVIOR_SPECIFICATION.md; this document leaves only implementation/resource/host validation to Phase 3/5.
- Main stereo output and four stereo layer outputs are in the product requirements; exact host buses/device routing are Phase 3.
- Audio processing is real-time safe: no blocking I/O, heap allocation, locks, logging or unbounded waits in the audio callback.

## 2. Standalone device I/O

Product-level requirements:
- User can select input and output audio devices.
- User can configure available input/output channel mapping.
- Input and output device sample rates and buffer sizes are displayed.
- Device removal, sleep/wake or sample-rate change must not crash the application.
- On device loss, audio processing transitions to a safe stopped/silent state and informs the user; reconnect allows explicit recovery.
- The application must not silently switch to an unintended input device and record private/unwanted audio.

Exact device API, aggregate-device support, hot-swap behavior and macOS minimum version remain Phase 3.

## 3. Live input routing and monitoring

- Live input can be assigned to each layer independently.
- Each layer has input level plus wet/dry controls for live-input processing (GS-SMG-012).
- Live input feeds the layer engine/FX path and also follows the parallel dry path summed before the master required by GS-ARC-007. Per-layer wet/dry controls and gain staging must preserve this approved routing.
- Input monitoring behavior must be explicit: monitor through the selected layer processing path; no hidden direct-monitor loopback is assumed.
- Input channel mapping supports mono and stereo sources where available.
- Mono live input is centered and available to both sides of stereo layer processing; explicit LL/RR duplication is not the default (DEC-040).
- Feedback prevention and gain staging must prevent accidental unbounded routing loops. Synth-to-record-buffer feedback remains L2 (GS-SMG-016).

## 4. Recording model

R1 recording:
- Per-layer recording.
- Stereo master-mix recording.
- One-shot recording and continuous loop recording.
- Recording buffers can be retained and exported as reusable samples.
- Granular and Sampler engines can play/process the layer buffer while recording (GS-SMG-009).
- Buffer sizing can be AUTO or fixed (seconds or bars) and can be initialized as mono/stereo (GS-SMG-011).
- Scrolling record is R1 (GS-SMG-010): playhead follows record head at an adjustable distance, with a defined buffer relationship. Exact latency range is not specified by the source.
- Record start modes represented in the benchmark: current position/resume, Wiper position, Start at zero, Off/protect. R1 UI and exact semantics must be validated before final acceptance.
- Input gain reference ±12 dB (REC-04).
- Recording must not cause unbounded memory growth; AUTO buffer has a finite safe cap established by Phase 3/5 profiling, not invented here.

Not R1 by current tiering:
- Input-threshold or transport-start arm/trigger (GS-SMG-013 L2).
- Clocked bar-length recording (GS-SMG-014 L2).
- Resampling instrument output to a new file (GS-SMG-015 L2).
- Synth-to-record-buffer feedback (GS-SMG-016 L2).

## 5. Record lifecycle

- User can initialize a recording buffer, arm/start, stop, and choose whether to keep/export or discard the capture.
- A kept capture becomes a sample asset available to the current layer and library.
- A discarded capture must not remain as a hidden dependency in a saved patch.
- A recording in progress must be represented distinctly from a committed sample asset.
- If a buffer is full or a resource limit is reached, recording stops safely, preserves valid captured audio, and informs the user.
- Save/export to WAV is R1 (GS-SMG-017).
- Per-layer recordings and stereo master recordings are distinct assets with their own identity and metadata.

## 6. Sample format and sample-rate behavior

- R1 input formats: WAV, AIFF and FLAC; mono or stereo; sample rates supported by the application/host are handled by the shared sample subsystem (GS-SMG-001).
- WAV save/export is supported.
- The current requirements propose 44.1–192 kHz and buffer sizes 16–4096 with variable block sizes (GS-PLT-003); these are proposed technical values, not yet architecture-approved.
- Musical behavior should remain consistent across sample rates (GS-NFR-010).
- No sample-rate conversion or decoding may occur on the audio callback.
- Decoder choice and resampling strategy remain Q-T04/Q-T16 Phase 3 decisions.

## 7. Outputs and routing

- Product output model includes stereo master and four stereo layer outputs.
- In Standalone, user can route these outputs to available device channels where supported.
- In plugin builds, exact output bus exposure and host compatibility are Phase 3 decisions.
- Recording the stereo master mix captures the post-layer-mix stereo output at the defined master tap point, including the parallel dry live-input path required by GS-ARC-007; host/Standalone tap equivalence is a Phase 3 validation item.
- Per-layer capture records that layer's signal at a consistent documented tap point, independent of other layers' mix level unless the product spec explicitly says otherwise.
- No master-bus EQ/FX is R1; GS-FX-008 remains L3.

## 8. Freeze boundary

- R1 requires four independently switchable layer Freeze controls and one global Freeze control.
- Global Freeze applies to all four layers as a single user action.
- Controller reachability is mandatory under GS-MID-008.
- Freeze product behavior is defined by DEC-042 and FREEZE_BEHAVIOR_SPECIFICATION.md: it snapshots the valid source region while voices, new notes, sequencer, MIDI, modulation, transport, FX and independent recording continue; thaw crossfades back to the prior live/rolling source. This document does not redefine that contract. Implementation, memory admission and host validation remain Phase 3/5 work.

## 9. Real-time, overload and recovery

- No disk/file I/O, memory allocation, mutex locks, logging or blocking waits on the audio callback.
- Sample loading/decoding, recording-file writes and export are performed outside the real-time callback with bounded handoff.
- Parameter/sample changes must avoid clicks and zipper noise.
- Resource exhaustion must degrade deterministically and safely; no invented maximum voice/grain/record duration is specified here.
- Audio-device loss, missing input, malformed sample file, full recording buffer and unavailable output channel must each have an explicit safe state and user-visible status.
- Exact CPU, RAM, latency and maximum recording limits remain Phase 3/5 measurements.

## 10. Acceptance coverage

- Standalone device selection, channel mapping and device-loss recovery.
- Live input can be routed independently to each layer.
- Input level and wet/dry controls function per layer.
- Per-layer and global Freeze controls exist and are controller-reachable; behavior is tested against the approved DEC-042 contract.
- Per-layer capture and stereo master capture work independently.
- One-shot, loop and scrolling recording work; AUTO/fixed buffer and mono/stereo initialization work.
- Recording can be played/processed while active and safely retained/exported/discarded.
- Buffer-full/resource-exhaustion behavior preserves valid audio and informs user.
- WAV/AIFF/FLAC load and WAV export are covered.
- Audio callback remains free of allocation, locks, I/O and blocking operations.
- No unbounded memory growth or host crash under malformed/missing input/device state.

## 11. Open items

- Q-P06 format-specific AU/VST3 live-input behavior and bus layout — Phase 3.
- Q-T04/Q-T16 sample-rate conversion and decoder choices — Phase 3.
- Q-T17 sample-rate/buffer support values and latency — proposed, require technical validation.
- Q-P08 unsaved recording embedding in plugin state — product policy approved by DEC-025; serialization/size policy remains Phase 3.
- Exact recording start modes and scrolling-record distance controls.
- Mono input behavior is set by DEC-040; exact channel mapping implementation remains Phase 3.
- Exact signal tap points for per-layer and master recording.
- Freeze implementation, memory admission, crossfade and host validation — Phase 3/5; product semantics are approved by DEC-042.
- Numeric resource limits and overload thresholds — Phase 3/5.

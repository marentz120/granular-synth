# Plugin Component and Bus Specification

**Version:** 0.1 — Phase 3 proposal  
**Status:** Proposed; host validation and owner approval required. No plugin implementation is authorized.  
**Authority:** DEC-020/026, GS-PLT-001/004/005, GS-ARC-006, GS-SMG-020, Q-P06/Q-P09/Q-T01/Q-T03.

## 1. R1 product commitments

- macOS Standalone, AU and VST3 are R1 formats (DEC-026).
- Standalone is a complete instrument with audio/MIDI device selection and live input.
- Every layer can receive live external audio; this applies to Standalone and plugin contexts (DEC-020).
- Output model is stereo master plus four stereo layer outputs (GS-ARC-006).
- Four layers share the same engine/state implementation and one transport/sequencer.
- CLAP is optional, not an R1 release requirement.
- Host limitations must be reported honestly; unsupported input/output configurations cannot be silently represented as supported.

## 2. Shared core boundary

All formats instantiate the same framework-independent engine and state model. Format adapters translate host lifecycle, audio buses, MIDI events, transport, parameter automation and state serialization into the shared core contract.

Proposed layers:
- `core/`: real-time primitives and DSP utilities, no plugin-framework types.
- `engine/`: voices, Granular/Sampler engines, modulation, FX, transport and sequencer.
- `sample/`: immutable prepared samples and recording buffers.
- `state/`: parameter registry, patch/multi/library state, serialization and migration.
- `host/`: JUCE wrapper or direct AU/VST3 adapters plus Standalone device host.
- `ui/`: editor and Standalone presentation using snapshots/commands.

The same core must be buildable and renderable headlessly.

## 3. Proposed bus model

### 3.1 VST3

Candidate instrument component:
- Main event input: MIDI event bus.
- Main stereo audio output: master mix, default active.
- Four stereo auxiliary audio outputs: Layer 1–4, optional/deactivatable where the host permits.
- Stereo audio input: live input source, optional; define whether it is main or auxiliary/sidechain and how channels map to layers.

The VST3 API supports multiple dynamic audio input/output buses and event buses. That API capability does not guarantee identical host UI/routing behavior.

### 3.2 Audio Unit

Candidate primary instrument component:
- `aumu` instrument with MIDI input and stereo master output.
- Validate whether target hosts permit the required live-input routing to this component and whether output buses are exposed as needed.

Alternative:
- Separate `aufm` music-effect component for MIDI-controlled audio processing/live-input workflows.
- If both `aumu` and `aufm` are shipped, they must share the same core and state format, and the product must explain their distinct host insertion workflows.

Do not ship both components solely because the API has both types. Choose based on tested host workflows, user discoverability, state interchange and maintenance burden.

### 3.3 Standalone

- User selects input/output device and channel configuration.
- Default master output is stereo.
- Four stereo layer outputs can be routed to available device channels when the device supports them.
- Mono input is centered and available to both sides of stereo layer processing (DEC-040).
- Device loss/reconnect, unavailable input, sample-rate changes and channel-count changes have explicit safe states.

## 4. Required host capability matrix

The release host matrix proposed in Q-P09 is Logic Pro, Ableton Live, Bitwig and Reaper. Exact versions and minimum supported versions remain to be set.

For every host × format, test:

| Capability | Required evidence |
|---|---|
| Scan and instantiate | Host scans, loads and identifies the component correctly |
| MIDI input | Note, CC, pitch bend, channel pressure, poly pressure and MPE behavior |
| Host transport | Tempo, PPQ, play/stop, loop/seek and sample-accurate event offsets where exposed |
| Live audio input | Correct routing to each layer, mono/stereo mapping and no feedback |
| Main output | Stereo master output |
| Layer outputs | Four independent stereo layer outputs where supported; documented fallback where not |
| Parameter automation | Stable IDs, gestures, read/write and automation playback |
| State | Save session, close host, reopen, restore embedded recordings and relink external samples |
| Offline render | Deterministic render with documented host/offline differences |
| Bypass/latency | Correct bypass and latency reporting; R1 Granular/Sampler zero-added-latency |
| Lifecycle | Suspend/resume, sample-rate and buffer-size changes, project close and shutdown |

A host/format cell is not considered supported until all required tests for that cell pass or a specific non-supported capability is documented and approved.

## 5. Transport and MIDI adapter contract

- Plugin formats consume host transport when valid and available.
- Standalone supports internal clock, external MIDI Clock and Ableton Link.
- Host transport and external MIDI Clock arbitration must not create two active clock masters in plugin contexts.
- MIDI Clock output and sequencer MIDI output are not R1 commitments.
- Program Change is handled only where the host delivers it; mapping behavior follows DEC-033.
- MPE member-channel and per-note expression routing follows DEC-038; host negotiation remains a validation item.

## 6. Parameter and state adapter contract

- Host numeric IDs map to canonical string IDs via the approved parameter manifest.
- No adapter may assign IDs based on registration order.
- Host state serialization must implement DEC-025: embed unsaved recordings; reference library files with relink/collect.
- AU/VST3 save/restore callbacks must not cause disk I/O or unbounded waiting on the audio thread.
- Host parameter gestures and automation must preserve identical semantic values across Standalone/AU/VST3.
- Host-specific unsupported operations must be surfaced rather than silently ignored.

## 7. Required prototype and validation work

Before choosing the final AU/VST3 component topology:
1. Build a minimal instrument prototype with MIDI input, stereo output and one optional stereo input.
2. Test AU instrument live-input routing in Logic Pro and at least one additional target host.
3. Test whether the AU music-effect alternative supports the required MIDI/live-input workflow and compare the insertion UX.
4. Test VST3 input plus five stereo outputs (master + four layers) in each proposed host.
5. Test bus activation/deactivation and channel arrangements after session save/reopen.
6. Test host state save/restore with a deliberately sized embedded recording and external sample reference.
7. Test parameter automation and MPE delivery.
8. Record host/version/OS, component identifiers, bus arrangement and observed limitations.

This prototype is an architecture validation spike. It is not production DSP implementation. It requires explicit owner authorization before code is written.

## 8. Decisions still open

| Item | Recommendation | Status |
|---|---|---|
| R1 formats | AU + VST3 + Standalone | Approved by DEC-026 |
| VST3 live-input bus | Optional stereo input on instrument component | Proposed; validate |
| VST3 outputs | Stereo master + four stereo layer outputs | Required product output; host exposure validate |
| AU component | Start with `aumu`; test `aufm` as alternative/companion | Open |
| AU live input | Must satisfy DEC-020 through a tested host workflow | Open |
| Host matrix | Logic, Live, Bitwig, Reaper | Proposed Q-P09; exact versions open |
| MIDI/transport behavior | Adapter-specific mapping to shared transport | Open details |
| Offline rendering equivalence | Define exact deterministic boundary | Open |

**Status:** This proposal translates the approved format and live-input requirements into testable architecture options. It does not select final AU component types or claim host certification.


## 9. Recommended output and live-input routing semantics

The product output model remains one stereo master plus four optional stereo layer outputs. Recommended semantic tap positions:

- Each Layer 1–4 output is post-layer-FX and pre-layer-fader/mute/solo. This provides stable stems for DAW mixing and aligns the layer output with its recording tap.
- The stereo master output is the sum of the four layer signals after layer gain, mute and solo logic.
- Master recording captures the same stereo master signal immediately before it is handed to the host/device output.
- Each layer's live input enters that layer before its FX chain. Mono input is centered across the stereo layer path (DEC-040).
- There is no separate direct/dry input-to-master bypass by default. This avoids duplicate monitoring paths and keeps layer FX, stems, Freeze source and master recording semantics consistent.
- When a host does not expose all optional layer buses, the main stereo master remains usable. The adapter must not sum layer buses into the master a second time.

These are recommended signal semantics, not a decision to select AU instrument or music-effect component types, nor proof that every host exposes all five outputs. AU/VST3 validation must confirm whether the semantic tap model can be preserved in each target host.

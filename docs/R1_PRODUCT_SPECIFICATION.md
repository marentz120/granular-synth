# R1 Product Specification

**Status:** Product scope baseline approved under the owner's explicit delegation to ChatGPT as product manager on 2026-11-27. This document records product decisions only. It does not approve DSP architecture, framework selection, implementation, or release.
**Version:** 1.0 (product decisions consolidated, 2026-11-27)
**Authority:** Owner decisions in this conversation plus delegated product-management decisions explicitly identified below.
**Reference:** GR-MEGA is a functional benchmark only. GRANULAR SYNTH must use original implementation, interaction design, presets and assets; do not copy proprietary firmware, source code, artwork or protected trade dress.

## 1. Product intent

GRANULAR SYNTH is a macOS sample-based granular instrument for both studio sound design and live performance. Standalone operation is a complete instrument and performance environment, not merely a plug-in wrapper. R1 must be usable as a live instrument without requiring a computer keyboard or mouse for essential performance operations.

Both studio sound design and live performance are primary use cases (DEC-015). Controller-only standalone live operation is mandatory (DEC-016).

## 2. R1 engine and layer scope

### 2.1 Layers
- Exactly **four independent Granular layers** are exposed in R1.
- Each layer has independent sample/source, synthesis parameters, voice allocation, modulation, effects and layer output/mix controls.
- All four layers share the instrument's transport and central sequencer.
- The product does not promise more than four user-facing layers in R1.

### 2.2 Engines
- R1 engines: **Granular and Sampler**.
- The engine is selectable per layer.
- Granular Slice is not an R1 engine; defer it to a later release for separate scoping.
- Tape is later (proposed L2); Spectral is later (proposed L3).
- This engine selection does not prescribe DSP implementation or engine-internal limits.

## 3. Live audio, Freeze and recording

- Every layer can receive live external audio as a source for granular processing.
- The product imposes no fixed product-level cap on the number of live input channels/sources. Actual availability is bounded by the audio device, host routing and measured system resources.
- Live input is a product requirement in both Standalone and plug-in contexts. AU/VST3 component and bus implementation remains a Phase 3 architecture decision.
- Freeze is available independently per layer and globally across the instrument.
- R1 supports live recording into layer buffers and granular playback/processing while recording.
- **Delegated PM decision:** R1 recording supports independent capture per layer and a stereo master-mix recording. Captured material can be retained/exported as samples subject to the sample/state persistence design.
- One-shot and loop recording are in scope, consistent with the existing GS-SMG-008 requirement.
- No unmeasured maximum duration or channel count is promised; engineering budgets and safe overload behavior are defined in Phase 3/5.

## 4. R1 step sequencer

The full performance-oriented step sequencer is part of R1. The interaction inspiration is the direct, immediate, step-based performance feel of the Roland TB-3; this is a functional reference only, not a request to copy Roland's interface, sounds, firmware or protected design.

### 4.1 Structure
- One **central sequencer** controls all four layers; there is not a separate sequencer per layer.
- Patterns have variable musical length and variable step/edit resolution. No fixed product-level maximum is specified for pattern length, step resolution, number of patterns, chain length or patterns in a Performance Set.
- Resource limits, memory limits and safe real-time behavior still apply and must be measured; “no product-level cap” does not mean technically infinite.
- A Pattern can contain multiple events at one step. A maximum of **eight simultaneous events per step** is a fixed R1 product rule.
- The eight events can be assigned freely across the four layers; all eight may target one layer or be distributed across layers.
- Each event can independently carry its event-level and granular control data, including pitch and relevant grain/layer parameters. The central sequencer can trigger grains/events and modulate layer parameters.
- Patterns can be saved, recalled, switched and chained. Pattern chaining has no fixed product-level maximum.

### 4.2 Timing and synchronization
- Internal BPM, external MIDI Clock, Ableton Link and DAW/host transport synchronization are required.
- Pattern switching is quantized to the end of the current bar.
- Changes to BPM, Swing, Humanize, Quantization and Microtiming controls take effect immediately during playback. Event scheduling must remain deterministic and click-safe; implementation details belong to architecture.
- Quantization supports selectable straight divisions, triplets, Swing, Humanize and per-pattern settings.
- Microtiming allows events to be placed off the selected grid.
- Step resolution and pattern length are variable, with technical ceilings established by profiling rather than arbitrary product caps.

### 4.3 Performance controls
- **Fill:** automatic, latched until turned off; affects all active layers; changes granular characteristics (e.g. Position, Pitch, Grain Size and Density), not the underlying rhythmic structure. Fill intensity is continuously controllable via a knob or MIDI CC.
- **Mute:** immediate mute/unmute for complete layers and for individual steps/events.
- For an individual muted step/event, the user can choose whether unmute takes effect immediately when still relevant in the current cycle or on its next occurrence.
- Pattern changes requested during playback take effect at the end of the bar.

### 4.4 Patterns, presets and performance sets
- Patterns can be stored inside a Preset and also in an independent Pattern Library.
- A Pattern can be reused across different Presets.
- Changing Preset does **not** replace or stop the currently active Pattern; the Pattern continues while the sound changes.
- R1 includes Performance Sets: ordered performance lists of Presets and Patterns for a complete set.
- No fixed product-level cap is set for Patterns per library, Patterns per chain or entries per Performance Set; technical limits are resource-bound.

### 4.5 Editing, drafts and history
- Pattern editing is possible while playback continues.
- Edits made during playback become musically active at the end of the current bar.
- Pattern changes are not written to the saved Pattern automatically; explicit Save is required.
- When leaving a Pattern with unsaved edits, the system preserves an automatic temporary Draft so the user can return without losing work.
- Full Undo/Redo is required for editing.
- R1 retains only the latest explicitly saved version of a Pattern; no persistent version-history or snapshot archive is required.

## 5. MIDI controller and live operation

- Standalone R1 must be fully operable for all essential live operations through MIDI controller alone, with no computer keyboard or mouse required during performance.
- Support generic class-compliant MIDI controllers; no named controller is a prerequisite.
- All Sequencer functions, including editing, must be accessible through MIDI, subject to the physical controls and feedback/display capabilities of the connected controller. The product must not claim identical ergonomics on every controller.
- Mapping supports MIDI Learn and manual assignment of CC, MIDI channel and Note.
- MIDI feedback to controller LEDs, displays or motorized faders is not an R1 requirement; defer it.
- Every operation realistically needed during a performance remains controller-reachable even if it could also be prepared in advance. Initial device selection and detailed mapping construction may be setup operations; recalling mappings or changing a live setup when needed mid-set is a performance operation.
- MIDI clock input is required in Standalone. Host/DAW sync applies in plug-in contexts. Ableton Link support is part of the product synchronization requirement; exact platform/API integration is architectural work.

## 6. Decisions delegated to the product manager

The owner explicitly delegated resolution of remaining open product questions to ChatGPT as product manager, provided decisions do not contradict prior owner decisions. The following decisions were made under that delegation:

1. R1 live recording includes both per-layer capture and stereo master-mix capture.
2. R1 should use a single shared transport/clock domain for the four layers and central sequencer; independent per-layer sequencer clocks are out of scope.
3. Fill is a latched global performance mode over all active layers, with continuous intensity control.
4. Named-controller templates and controller feedback are deferred; generic MIDI Learn/manual mapping is the R1 baseline.
5. Product-level limits remain flexible where the owner requested no fixed product cap; actual safe limits must be explicit, deterministic, measured and tested.

## 7. Explicitly not decided by this document

The following remain technical, legal, or later-phase decisions and must not be inferred from this product scope:
- C++/JUCE or any other framework; DSP core boundaries and threading model.
- Exact AU/VST3 component types, bus layout, host-specific live-input support details and MIDI output behavior.
- macOS minimum version, CPU baseline, sample-rate/buffer matrix, measured grain budget, memory budget and maximum safe recording duration.
- Parameter-ID scheme and exact host automation exposure.
- Preset serialization, embedded versus linked sample policy and migration format.
- Detailed UI layouts, screen workflows and controller mapping UX.
- Legal review, product name, branding, factory content and distribution model.
- Numeric acceptance thresholds and test plans (Phase 5).

*End of R1_PRODUCT_SPECIFICATION.md*

# GRANULAR SYNTH — UI/UX Specification

**Version:** 0.1  
**Status:** Phase 4 discovery draft — owner review required  
**Authority:** Approved product behavior remains in `R1_PRODUCT_SPECIFICATION.md`, `LIVE_PERFORMANCE_SPECIFICATION.md` and `DECISIONS_LOG.md`. This document specifies presentation and interaction proposals only. It does not change DSP, product scope, architecture, or acceptance thresholds.

## 1. Design intent

The interface must support both studio sound design and live performance as primary use cases (DEC-015). Essential standalone live operations must be operable from a generic MIDI controller without computer keyboard/mouse (DEC-016/021). The hardware concept uses a large central touchscreen, four multifunction encoders, and a low-profile white enclosure.

### Owner-confirmed UI direction
- One selected layer is shown in the main workspace; L1–L4 selection remains readily accessible.
- A 16-step sequencer strip remains fixed at the bottom of the display.
- Four physical multifunction encoders sit below the display.
- Encoder rotation changes the currently assigned parameter; encoder press advances/selects the parameter context.
- Main Granular workspace permanently exposes waveform/region editing and Size, Density, Position, Pitch and Spread.
- Touchscreen is enabled. Touch and encoder interaction remain synchronized.
- Visual direction: flat white enclosure, dark blue/near-black UI, restrained cyan accents.

These are UI decisions, not changes to approved DSP/product requirements.

## 2. Global screen frame

1. **Header:** layer selection (L1–L4), engine selection (Granular/Sampler), preset context, tempo/clock status and transport.
2. **Workspace:** current screen's primary editing surface.
3. **Fixed sequencer strip:** 16 visible steps, pattern/step status and compact timing controls. It remains visible across workspaces.
4. **Hardware encoder strip:** four encoders with contextual labels and current values.

Exact pixel dimensions, touch target sizes and hardware dimensions are not specified; establish them from the target display and industrial-design constraints.

## 3. Navigation model

Top-level workspaces:
- **Granular:** waveform/source region and granular controls.
- **Sampler:** sample playback, direction, loop/one-shot/bounce and loop-release controls.
- **Sequencer:** expanded editing surface while preserving the fixed strip.
- **Modulation:** envelopes, LFOs, modulation routing and macros.
- **FX:** four serial slots per layer, selecting from the seven approved R1 effect types.
- **Mix:** four-layer levels, pan, mute/solo and master recording controls.

Layer selection is global and persists while changing workspaces. Workspace navigation must not implicitly change the active Pattern or replace it when a Preset changes (R1 product behavior).

## 4. Granular workspace

Persistent elements:
- Waveform for the selected source.
- Draggable source/selection region with visible boundaries.
- Sample/source identity and relevant source status.
- Five primary parameters: Size, Density, Position, Pitch and Spread.
- Fixed 16-step sequencer strip.

Freeze, Reverse, recording, grain shape/window, and other advanced controls are available through contextual/secondary controls. They must not displace the five primary controls. Freeze follows DEC-042; UI must communicate preparation/failure visibly and must not imply Freeze is an engine pause.

Touch proposals:
- Drag selection handles to adjust source region.
- Drag within the selected region to reposition it, clearly distinguished from resizing.
- Pinch/plus-minus zoom and fit-to-source action.
- Tap a parameter to focus it on an encoder; touch and rotary changes share one value model.
- Provide numeric value display and reset affordance where safe.
- Avoid hidden gesture-only operations; visible controls must exist for essential actions.

## 5. Sampler workspace

Expose source waveform and playback configuration:
- Forward/reverse.
- Loop, one-shot and bounce.
- Loop Release ON/OFF, following DEC-030 exactly.
- Voice allocation and Env Pile-up toggle, default OFF (DEC-037).
- Common source navigation and sample status.

The fixed sequencer strip remains visible. Sampler controls must not silently alter Granular settings.

## 6. Sequencer workspace and fixed strip

The fixed strip shows 16 step positions at all times. It is an overview/direct-action surface, not a product-level limit on Pattern length or edit resolution.

- Tap a step to toggle its enabled state.
- Selecting a step exposes its event editor.
- Drag gestures may edit displayed continuous values, with numeric feedback and a non-drag alternative.
- Event editor supports up to eight simultaneous events per step, assigned among four layers.
- Provide access to Velocity, Pitch and applicable event parameter locks.
- Pattern length, edit resolution, quantization, triplets, Swing, Humanize and Microtiming remain available in expanded Sequencer workspace.
- Pattern edits during playback follow approved bar-boundary application behavior.
- Save/Draft/Undo/Redo follows R1 product specification.
- Fill is latched globally across active layers and changes granular character, not rhythmic structure.

The detailed event visualization and handling of more than 16 steps are open design work. The 16-step strip may page/scroll or show a moving window, but must preserve variable Pattern length without an arbitrary cap.

## 7. Modulation workspace

Provide distinct views for Envelopes, LFOs, modulation matrix and ten macros per patch. LFO Poly/Mono phase mode defaults to Mono (DEC-024). Distinguish shared per-layer macro base values from per-voice modulation contributions (DEC-031).

## 8. FX workspace

Each layer has four serial FX slots. R1 effect catalog:
- Delay
- Ping-pong Delay
- Reverb
- Chorus
- EQ3
- Saturator
- Compressor

Make slot order explicit and provide bypass/enable state. No additional effect type or master-FX chain is implied.

## 9. Mix and performance workspace

Provide four-layer overview and access to layer level/pan, Mute/Solo, master output/recording status and per-layer recording status.

Performance-critical actions (transport, Freeze, Fill, Mute, Pattern recall, Panic and other essential operations in LIVE_PERFORMANCE_SPECIFICATION.md) must remain reachable without deep navigation. A compact performance-access area may be persistent; exact placement remains a Phase 4 review item.

## 10. Four multifunction encoders

Owner-confirmed behavior:
- Rotation changes the focused parameter.
- Press advances/selects the parameter context.

Proposed contract:
- Each encoder has persistent label/value display.
- Touching a control focuses it and assigns it to an appropriate encoder.
- Encoder rotation and touchscreen editing update one shared parameter value.
- Press advances within a deterministic, visible focus order.
- Layer changes update contextual assignments without changing unrelated parameter values.
- Long press is reserved for secondary actions; no essential operation may require it.
- Exact acceleration/fine-adjustment curves are UI/engineering tuning, not decided here.

The precise four-encoder assignment matrix per workspace remains a design deliverable. Essential live operations must also be independently mappable to generic MIDI controllers.

## 11. Accessibility and performance safety

- Touch targets must be usable on a performance desk; exact dimensions await display selection.
- High-risk actions (delete, clear, replace source, discard unsaved Pattern) require confirmation or undo/recovery.
- Panic remains directly accessible and is never hidden behind a modal.
- Show transport/clock source and loss state clearly.
- Show Freeze pending/success/failure states clearly.
- Never report success until the engine confirms it.
- Do not communicate state through color alone.
- UI animation must not obscure timing-critical status.

## 12. State and persistence

- Workspace navigation does not change the active Pattern.
- Preset changes do not replace the active Pattern.
- Layer selection is UI focus, not an instruction to stop other layers.
- Distinguish UI state (workspace, focus, zoom) from saved sound state.
- Persistence of view/focus state is a proposal requiring a product decision if it affects preset recall or host state.

## 13. Open design questions

1. Exact four-encoder assignment/focus order for each workspace.
2. Fixed sequencer strip behavior when Pattern length exceeds 16 steps.
3. Persistent performance-action placement and touch target layout.
4. Exact screen dimensions, resolution, aspect ratio and viewing distance.
5. Whether Mix needs a compact four-layer overview while one layer remains selected.
6. Detailed MIDI Learn editor and mapping conflict UX (see `MIDI_MAPPING_UX_SPECIFICATION.md`).

## 14. Non-goals and constraints

This document does not approve a framework, public parameter IDs, DSP behavior, audio routing, host buses, memory budgets or CPU limits. It does not replace R1 requirements or Phase 5 acceptance criteria and does not authorize implementation.

## 15. Review checklist

- [ ] Confirm owner-approved UI choices are represented accurately.
- [ ] Validate every essential live action against LIVE_PERFORMANCE_SPECIFICATION.md.
- [ ] Resolve open design questions or record explicit deferrals.
- [ ] Map UI elements to 124 R1 requirements without treating candidate API symbols as approved interfaces.
- [ ] Prepare screen-by-screen wireframes after review.

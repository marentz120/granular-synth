# GRANULAR SYNTH — Screen State and Navigation Specification

**Version:** 0.1  
**Status:** Phase 4 discovery draft — owner review required  
**Authority:** R1 product behavior and DECISIONS_LOG.md remain authoritative. This document describes proposed UI state boundaries and navigation.

## 1. State domains

### 1.1 Instrument/product state
- Four layer configurations and each layer's selected engine.
- Source/sample references and recording buffers.
- Granular/Sampler parameters, modulation and FX.
- Pattern data and active Pattern identity.
- Preset/Performance Set data.
- Transport/clock state.
- Freeze state and snapshot lifecycle (DEC-042).

### 1.2 UI navigation state
- Selected workspace and focused layer.
- Selected control/encoder focus.
- Waveform zoom and visible source region.
- Sequencer viewport/page.
- Open editor panel/popover.
- MIDI mapping editor selection/filter.

UI navigation state must not accidentally mutate instrument state.

### 1.3 Transient interaction state
- Active touch/drag gesture.
- Encoder focus/press state.
- Pending confirmation.
- MIDI Learn armed state.
- Unsaved Pattern Draft state.
- Pending asynchronous operation indicator.

Transient state must be cancelled or safely completed when its owning view closes; it must not leave hidden operations running.

## 2. Navigation rules
- Layer selection L1–L4 is global and stable across workspaces.
- Workspace changes do not stop transport or alter audio.
- Workspace changes do not change the active Pattern.
- Preset recall does not replace the active Pattern.
- Fixed sequencer strip remains visible in every primary workspace.
- Selecting a sequencer step opens event editing without leaving the current engine workspace; expanded Sequencer workspace is available for detailed editing.
- Returning to a workspace restores view context where safe, but invalidates stale references to replaced/deleted sources or controls.
- Modal dialogs are reserved for destructive or blocking confirmation; routine editing stays inline.

## 3. Layer focus semantics
Selected layer is UI editing focus only. It does not mute/solo other layers, stop their sequencers/events, change MIDI channels, or redirect mappings explicitly targeting another layer. Layer changes update displayed source/contextual controls without overwriting other layer values.

## 4. Sequencer viewport
The fixed strip presents 16 visible steps. Patterns longer than 16 steps require full-pattern navigation in expanded Sequencer workspace. Candidate strategies:
- Horizontal scrolling window.
- 16-step pages.
- Follow-playhead window with manual hold/override.

No strategy is selected in this draft. The chosen method must preserve direct access, live editing, bar-boundary application and variable Pattern length without a product-level cap.

## 5. Encoder focus state
- Each encoder has one current target and visible label/value.
- Touching a supported control may assign it to an encoder.
- Pressing an encoder advances to the next target in a visible deterministic order.
- Current target is never implicit/invisible.
- Layer/workspace changes update target lists without causing a parameter jump.
- Touch and encoder rotation write to the same logical parameter.
- Long press is secondary only and never the sole route to an essential operation.

Exact focus ordering per workspace remains open.

## 6. Unsaved changes and recovery
- Pattern edits follow explicit Save, temporary Draft on leaving an unsaved Pattern, and Undo/Redo behavior in R1 product specification.
- Workspace changes must not discard a Draft.
- Preset switching must not silently replace active Pattern.
- Destructive sample replacement, Pattern clear, mapping delete or scope reset require Undo or explicit confirmation.
- Pending operations show Pending/Success/Failure; success is not shown before engine confirmation.

## 7. Transport and asynchronous state
The UI displays the active transport/clock authority and status. It does not infer authority from the open screen.

For Freeze, follow DEC-042:
- Show preparation/capture pending state.
- Show success only when snapshot is ready and active.
- Show safe visible failure if preparation fails.
- Recording continues independently.
- Thaw crossfades to prior live/rolling source.
- Panic does not thaw Freeze.

## 8. Recovery and safety
- Panic remains accessible and immediate; navigation never blocks it.
- Device disconnection or missing sample shows actionable status without crash or silent substitution.
- Stale UI references are invalidated when underlying parameters/sources are replaced.
- Distinguish unavailable, disabled, pending and active states.

## 9. Open questions
- Sequencer viewport strategy for Patterns longer than 16 steps.
- Persistence of workspace/focus/zoom: per user, per preset, or not persisted.
- Transport-source status/conflict presentation.
- Modal/undo policy for each destructive operation.
- UI state restoration when loading a Preset with a different Layer/Workspace focused.
- Host/plugin-specific UI constraints and resizability depend on Phase 3 technical decisions.

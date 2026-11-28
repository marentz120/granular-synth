# GRANULAR SYNTH — Encoder, Sequencer Viewport & Performance UX

**Version:** 0.1  
**Status:** Phase 4 discovery proposal — owner review required  
**Authority:** UI behavior proposals only. Approved product behavior remains in `R1_PRODUCT_SPECIFICATION.md`, `LIVE_PERFORMANCE_SPECIFICATION.md`, and `DECISIONS_LOG.md`. Nothing here changes R1 scope, DSP behavior, MIDI protocol requirements, architecture, or acceptance thresholds.

## 1. Purpose and design constraints

This document closes three Phase 4 UI design gaps:
1. A deterministic four-encoder assignment model for each workspace.
2. A scalable viewport for variable-length Patterns while retaining the owner-confirmed fixed 16-step strip.
3. A persistent performance-action area that exposes critical operations without deep navigation.

Owner-confirmed constraints retained:
- Four multifunction encoders below the touchscreen.
- Rotation changes the assigned parameter.
- Press advances/selects parameter context.
- Touch and encoder interaction remain synchronized.
- The 16-step strip remains fixed at the bottom of the display.
- Essential standalone live operations must be operable from a generic MIDI controller alone; no keyboard/mouse dependency.
- No R1 MIDI feedback or named-controller template.

All detailed assignments and screen behavior below are **proposals**, not owner-approved decisions.

## 2. Encoder interaction contract

### 2.1 Common behavior

- Each encoder always displays a parameter name, current value and (where useful) unit.
- Rotation changes only the currently focused parameter.
- A short press advances that encoder's focus to the next item in its visible, deterministic assignment list.
- Touching a parameter focuses it on the nearest appropriate encoder; the UI visibly confirms the assignment.
- Touch edits and encoder rotation update the same parameter value.
- Changing layer or workspace refreshes contextual assignments; it must not modify parameter values merely because focus changed.
- Encoder focus is UI state, not saved sound state, unless a later owner decision explicitly makes it persistent.
- Long-press and double-tap are not required for any essential operation. They may be considered optional shortcuts only after the short-press behavior is validated.
- No destructive action (delete, clear, replace source, discard draft) is triggered by encoder rotation or a single focus-advance press.

### 2.2 Assignment matrix — proposed defaults

| Workspace | Encoder 1 | Encoder 2 | Encoder 3 | Encoder 4 |
|---|---|---|---|---|
| Granular | Grain Size | Grain Rate | Position | Pitch |
| Sampler | Scan | Pitch / Playback Rate | Sample Start | Sample Stop |
| Sequencer | Event Velocity | Event Pitch | Event Layer Assignment | Selected Event Parameter / Lock |
| Modulation | Envelope Attack | Envelope Decay | Envelope Sustain | Envelope Release |
| FX | Selected Slot | Effect Parameter 1 | Effect Parameter 2 | Next exposed parameter in selected effect |
| Mix | Selected Layer Level | Selected Layer Pan | Layer Mute / Solo state | Record target / status selection |

**R1-backed assignment rule:** the first four encoder defaults above are concrete proposals grounded in the R1 parameter inventory, not a claim that all four controls exhaust each engine's eight-knob section. The complete A–H engine assignments are specified in §2.5. 
- Sampler Loop Crossfade is L2 (GS-SMP-006) and is deliberately absent.
- Sequencer event parameter-lock availability and the exact set of lockable parameters must be validated against R1 event requirements.
- FX parameters resolve from the selected approved effect's actual parameter list; no universal Wet/Dry control is assumed.
- Mix encoders do not create new routing or gain stages. Record target/status is a selector/status context, not a new master-record gain parameter.

### 2.3 Focus paging and parameter reachability

- Each workspace exposes a visible ordered focus list; short press advances one item and wraps only at the end of that list.
- The four encoders may each have their own focus cursor. This preserves four simultaneous assignments rather than forcing one global cursor.
- Touching a control assigns it to one encoder and updates its label immediately.
- A compact “More”/context page exposes additional parameters without hiding the primary four.
- No parameter may be reachable only through an undocumented gesture.
- The complete R1 parameter inventory must be audited against touch controls, encoder focus pages and MIDI Learn targets before Phase 4 UI sign-off. This document does not claim that the matrix alone covers every R1 parameter.

### 2.5 Eight-knob engine assignment — R1 proposal

GS-UI-002 requires eight logical controls A–H and per-engine remapping. The assignments below use only R1 controls present in the approved requirements and domain specifications. They are product-UX defaults proposed for owner review; they do not define stable parameter IDs, scaling curves or MIDI CC values.

| Slot | Granular engine | Requirement | Sampler engine | Requirement |
|---|---|---|---|---|
| A | Grain Size | GS-GRN-002 | Scan | GS-SMP-001 |
| B | Grain Rate | GS-GRN-001 | Pitch / Playback Rate | GS-SMP-001 |
| C | Position | GS-GRN-004 | Sample Start | GS-SMP-002; sample flag model |
| D | Pitch | GS-GRN-010 | Sample Stop | GS-SMP-002; sample flag model |
| E | Scan | GS-GRN-009 | Spray | GS-SMP-001 |
| F | Spray | GS-GRN-005 | Pan Spray | GS-SMP-001 |
| G | Pan | GS-GRN-006 | Direction | GS-SMP-001 |
| H | Pan Spray | GS-GRN-006 | Loop Mode (One-shot / Loop / Ping-pong) | GS-SMP-002 |

Rules:
- Scan and Pitch retain their meaning across engines.
- Granular Density is displayed as the derived value (Rate × Size) per GS-GRN-003; it is not a second independent encoder target in this default bank.
- Sampler Loop Start and Loop End remain directly editable in the waveform/flag editor. They are not substituted for Sample Start/Stop, and no L2 Loop Crossfade control is exposed.
- If the current sample/engine state makes a control inapplicable, show it disabled with its reason; do not silently rebind the physical encoder to another parameter.
- The eight logical slots are distinct from the four physical encoders. The four encoders page/focus across A–H in a stable order; the current A–D or E–H bank is visible. A short press changes focus only; it does not execute a toggle or destructive action.
- Exact parameter ranges, taper, fine/coarse modifiers and stable IDs remain Phase 3/Q-T12/Phase 5 work.

### 2.4 Granular primary-control exception

The Granular workspace permanently exposes Size, Density, Position, Pitch and Spread on screen, as required by the owner-confirmed UI direction. The four physical encoders default to Size, Density, Position and Pitch. Spread remains directly touch-editable and is reachable through the encoder focus sequence/context page. No primary Granular control is removed to make the four-encoder count fit.

## 3. Sequencer viewport for Patterns longer than 16 steps

### 3.1 Model

- A Pattern retains its approved variable musical length and variable edit resolution; no arbitrary product-level maximum is introduced.
- The fixed strip always displays 16 consecutive step positions.
- The 16 positions are a **viewport**, not the Pattern length.
- The expanded Sequencer workspace displays the same viewport with additional event editing controls.
- Pattern length and edit resolution remain separate settings.

### 3.2 Proposed navigation

- Provide explicit Previous Window and Next Window actions.
- Window 1 displays steps 1–16; window 2 displays 17–32; window 3 displays 33–48, and so on.
- The current window is visibly identified (for example, “17–32 of 64”).
- A direct “Go to step” control is available in the expanded Sequencer workspace; it must be accessible by touch and mappable through generic MIDI.
- A playhead-follow option may automatically move the viewport when playback leaves the visible 16-step range. It is OFF by default in this proposal to prevent unexpected viewport movement while editing.
- When follow is OFF, playback may continue outside the visible window; transport and playhead status remain visible, and the current viewport does not silently jump.
- When Pattern length is shorter than 16, unused positions are visibly inactive and cannot receive events.
- When Pattern length changes, preserve the user's current edit context where possible; if the current viewport becomes invalid, move to the nearest valid window and show the change.
- Changing viewport never changes Pattern length, edit resolution, event data, or playback position.

### 3.3 Step and event interaction

- Tap a visible step to select it; a separate explicit action toggles its enabled state.
- The selected step can contain up to eight simultaneous events, freely assigned across the four layers, as required by R1.
- Event selection is distinct from step selection; show both selected step and selected event.
- Provide explicit add, duplicate, remove and layer-assignment actions in the expanded editor. Destructive actions require undo or confirmation consistent with the general UI safety policy.
- Event editing during playback follows the approved end-of-current-bar activation behavior.
- Undo/Redo, Save and temporary Draft behavior follow R1 product behavior; viewport navigation itself is not a Pattern edit and does not create a Draft.

### 3.4 Fixed strip compact controls

The fixed strip should expose:
- 16 visible step cells and current viewport range.
- Current Pattern name/slot and Pattern-change status.
- Previous/Next viewport actions.
- Selected step/event indication.
- A clear route into expanded event editing.

It should not attempt to display every event's full parameter set at once.

## 4. Persistent performance-action area

### 4.1 Proposed persistent controls

A persistent action row is proposed between the fixed step strip and workspace navigation (exact physical placement remains a wireframe decision). It should expose:

| Control | Persistent UI behavior | Notes |
|---|---|---|
| Play / Stop | Always visible; clear running/stopped state | Shared transport for all layers and central sequencer |
| Panic / All Sound Off | Always visible, one deliberate gesture, never behind a modal | Must remain available during error/recovery states |
| Global Freeze | Always visible; show pending/active/failure state | Applies across all four layers at one logical boundary per DEC-042 |
| Selected-layer Freeze | Available for the focused layer without entering a deep settings page | Per-layer Freeze remains independently switchable |
| Fill toggle | Always visible; latched state clearly shown | Global across active layers; changes granular character, not rhythm structure |
| Fill intensity | Visible compact value or immediate encoder/touch focus | Continuous control; do not hide intensity behind a modal |
| Layer Mute / Solo | Compact access for L1–L4 or selected layer | Mute is immediate; Solo must not be confused with layer focus |
| Pattern navigation | Current Pattern and previous/next or chain navigation | Actual switching during playback takes effect at bar end |
| Record status | Persistent indication for per-layer and master recording | Start/stop controls must be explicit; status must distinguish layer vs master capture |

The row is a UI proposal. It does not add a new transport mode, recording mode, Freeze behavior or Fill behavior.

### 4.2 Contextual but rapidly reachable actions

The following may live in the expanded Sequencer/Pattern or dedicated context panel, but must remain reachable without deep navigation and must be MIDI-controller-operable when needed during a set:
- Pattern Save.
- Draft return/restore.
- Undo and Redo.
- Pattern chain and Performance Set navigation.
- Step/event mute and unmute timing choice.
- Per-layer recording selection and one-shot/loop mode.
- Chord hold/record.
- Preset/patch recall and active-bank mapping recall.
- MIDI Learn entry/exit and mapping recall.

Do not crowd the persistent row with every editing action. Keep the highest-risk/high-frequency controls persistent and provide a single, visible route to the full performance command surface.

### 4.3 Freeze state presentation

UI must distinguish:
- Off / live source.
- Capture pending.
- Frozen snapshot active.
- Already static (library sample; redundant capture disabled/reported).
- Failure (safe state, visible reason/status).

Freeze does not pause existing grains/voices, sequencer, MIDI, modulation, transport, FX or independent recording. New notes use the immutable snapshot; thaw crossfades to the prior live/rolling source. These semantics are fixed by DEC-042 and must not be contradicted by labels or animation.

### 4.4 Panic safety

- Panic is always visible and never hidden behind a modal or secondary page.
- One deliberate gesture invokes All Sound Off; do not require a multi-step confirmation that makes recovery unusable.
- Panic must not be conflated with Stop, Mute, Freeze or transport reset.
- UI must show the resulting state only after the engine confirms it.

## 5. MIDI controller reachability

This is a UI reachability map, not a default CC layout. The product requires generic class-compliant controllers, MIDI Learn/manual CC, channel and Note mapping, and controller-only access to every essential live operation including full sequencer editing.

- Every persistent performance action has a mappable target.
- Every Sequencer editing action, including viewport navigation, step/event selection, event/layer assignment, edit, save, draft, undo and redo, must be reachable by MIDI.
- Every encoder-focused parameter can be MIDI-mapped where the underlying control is mappable.
- Pattern/Performance Set navigation and recall of an existing mapping set must be MIDI-reachable.
- Do not assume controller LEDs/displays receive feedback in R1.
- Do not prescribe a named controller or a universal default CC map.
- MIDI Learn construction can use the touchscreen as a setup workflow; a performer must not need to open a computer keyboard/mouse during a set to use existing mappings or change to a saved mapping set.

## 6. State and focus rules

- Workspace, selected layer, viewport window, selected step/event and encoder focus are UI/navigation state.
- Pattern data, sound parameters, transport, Freeze and recording are product state.
- Changing encoder focus or viewport never changes sound or Pattern data.
- Switching workspace does not stop transport or change the active Pattern.
- Changing Preset does not replace the active Pattern.
- Layer focus does not mute, solo or stop any other layer.
- If a performance-critical state change is pending (for example, Pattern switch at bar end or Freeze capture), show pending state and do not display success prematurely.

## 7. Review and validation checklist

- [ ] Owner approves or revises the proposed encoder defaults per workspace.
- [ ] Owner approves or revises the proposed A–H assignments in §2.5 against the authoritative R1 parameter inventory.
- [ ] Verify that no R1 parameter is inaccessible by touch/encoder focus/MIDI Learn.
- [ ] Owner approves viewport paging and playhead-follow default.
- [ ] Confirm all eight-event-per-step editing paths remain practical.
- [ ] Confirm persistent row placement and control grouping in wireframes.
- [ ] Verify every essential live operation in LIVE_PERFORMANCE_SPECIFICATION.md §1 has a visible route and MIDI-reachable action.
- [ ] Validate Freeze state labels against DEC-042.
- [ ] Create Phase 5 acceptance tests for the final owner-approved UI behavior.

## 8. Explicit non-goals

This document does not:
- Approve a hardware screen size, resolution, aspect ratio or touch-target dimensions.
- Select a framework, hardware controller, encoder part, MIDI mapping default or public parameter-ID scheme.
- Add FX types, Sampler features, mixer routing, master processing or new sound parameters.
- Set Pattern length, event-count or resource limits beyond approved R1 behavior.
- Authorize DSP, plugin shell, UI or hardware implementation.
- Close Phase 5 acceptance-criteria gaps.

# GRANULAR SYNTH — Consolidated Screen Layouts

**Version:** 0.1  
**Status:** Phase 4 wireframe-level proposal — owner review required  
**Scope:** Screen hierarchy, information placement and navigation behavior. Documentation only; no pixel dimensions, UI implementation or hardware dimensions are selected.  
**Authority:** `UI_UX_SPECIFICATION.md`, `SCREEN_STATE_AND_NAVIGATION_SPECIFICATION.md`, `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md`, `PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md`, R1 product specifications and DEC-015…DEC-042.

## 1. Layout principles

1. Keep one stable global frame across all six workspaces.
2. Keep layer focus, engine selection, active Pattern and transport visible; never hide state needed to interpret a live action.
3. Keep the 16-step strip fixed at the bottom of the touchscreen. It is a viewport, not a Pattern-length limit.
4. Keep the performance-action row persistent and compact. Panic must never be hidden by a modal or secondary workspace.
5. Keep the four physical encoders and their current targets visible in the lower hardware/control band. The engine's eight logical A–H targets are paged across four physical encoders.
6. Do not let layer focus redirect explicit L1–L4 MIDI mappings.
7. Separate browse/selection from recall/execute for Presets, Patterns, Performance Set entries and destructive actions.
8. Show Pending, Active/Completed, Already Static, Disabled and Failed as distinct states. Never infer success from a received MIDI message or a button press.
9. Preserve touch access and generic MIDI-controller access for every essential live-performance operation. No R1 MIDI feedback or named-controller template is assumed.
10. The layout must scale responsively; exact display dimensions, touch-target sizes and pixel coordinates remain industrial-design decisions.

## 2. Global frame

The following hierarchy is common to every primary workspace:

```
┌──────────────────────────────────────────────────────────────────────────┐
│ HEADER: L1 L2 L3 L4 | GRANULAR/SAMPLER | Preset | BPM/Clock | Transport │
├──────────────────────────────────────────────────────────────────────────┤
│ WORKSPACE NAV: Granular | Sampler | Sequencer | Modulation | FX | Mix   │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│                         WORKSPACE CANVAS                                │
│                                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ PERFORMANCE ROW: Panic | Play/Stop | Freeze | Fill | Mute | Record      │
├──────────────────────────────────────────────────────────────────────────┤
│ FIXED SEQUENCER: Pattern | Window 1–16 | 16 step cells | Prev/Next      │
└──────────────────────────────────────────────────────────────────────────┘
         PHYSICAL CONTROL BAND BELOW TOUCHSCREEN:
         Encoder 1       Encoder 2       Encoder 3       Encoder 4
         [target/value]  [target/value]  [target/value]  [target/value]
```

### 2.1 Header behavior

- Layer selectors L1–L4 show the current editing focus; they do not mute, solo or stop other layers.
- Engine selection is per layer. Changing engine changes the visible engine context and A–H assignment bank, not the stored values of unrelated parameters.
- Preset context identifies the current Patch/Multi recall target and dirty/pending state.
- Tempo/clock status identifies the current authority (internal, MIDI Clock, Ableton Link or host/DAW transport as applicable), running/stopped state and clock-loss condition.
- Transport controls affect the shared transport; they are distinct from audio recording Stop.
- Program Change/Performance Bank status is visible without conflating it with the active Performance Set.

### 2.2 Performance row

Persistent compact controls:
- Panic / All Sound Off — one deliberate action; always available.
- Play / Stop — shared sequencer transport.
- Freeze — selected-layer action plus a distinct Global Freeze affordance; show capture/thaw state.
- Fill — latched On/Off and immediately visible intensity.
- Layer Mute/Solo — explicit layer identity; never use selection highlighting as mute indication.
- Recording — separate Layer L1–L4 and Stereo Master target/status access. The row may show a compact active-state summary and opens the recording control surface.

The row must not collapse the distinction among Panic, transport Stop, Mute, Freeze and recording Stop.

### 2.3 Fixed sequencer strip

- Shows exactly 16 consecutive step positions and a visible window label (e.g. 17–32 of 64).
- Shows active Pattern identity, pending Pattern change and selected step/event.
- Previous/Next Window and Go to Step are available by touch and generic MIDI mapping.
- A step tap selects the step; a separate explicit control toggles enable/mute state.
- If the Pattern is shorter than 16 steps, unused cells are visibly inactive.
- The strip never silently jumps while follow-playhead is Off.
- Detailed multi-event editing opens in the Sequencer workspace without losing the selected step context.

## 3. Workspace wireframes

### 3.1 Granular

```
┌──────────────────────────────────────────────────────────────────────────┐
│ SOURCE NAME / STATUS                                      ZOOM  FIT      │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│                         SAMPLE WAVEFORM                                  │
│        [crop/start/stop region] [playhead / scan / grain markers]        │
│                                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ PRIMARY CONTROLS:  SIZE | DENSITY (derived) | POSITION | PITCH | SPREAD  │
│ SECONDARY: Spray | Pan | Pan Spray | Direction | Ratio Mode | Window    │
├──────────────────────────────────────────────────────────────────────────┤
│ PERFORMANCE ROW                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ FIXED 16-STEP STRIP                                                      │
└──────────────────────────────────────────────────────────────────────────┘
```

- Waveform and source region are the visual center.
- Size, Density, Position, Pitch and Spread remain visible as primary controls.
- Density is displayed as Rate × Size; do not imply that it is an independent base parameter.
- The A–H engine bank follows the R1 proposal in `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md` §2.5.
- Freeze, recording and other secondary controls remain rapidly reachable and MIDI-mappable where essential.

### 3.2 Sampler

```
┌──────────────────────────────────────────────────────────────────────────┐
│ SAMPLE NAME / STATUS                                      ZOOM  FIT      │
├──────────────────────────────────────────────────────────────────────────┤
│                         SAMPLE WAVEFORM                                  │
│   [Sample Start/Stop] [Loop Start/End] [playhead / direction indicator] │
├──────────────────────────────────────────────────────────────────────────┤
│ PLAYBACK: Forward/Reverse | One-shot/Loop/Bounce | Loop Release ON/OFF   │
│ VOICE: Polyphony / Env Pile-up (OFF default)                              │
│ PERFORMANCE: Scan | Pitch/Rate | Spray | Pan Spray | Direction           │
├──────────────────────────────────────────────────────────────────────────┤
│ PERFORMANCE ROW                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ FIXED 16-STEP STRIP                                                      │
└──────────────────────────────────────────────────────────────────────────┘
```

- Sample Start/Stop and Loop Start/End are visibly distinct boundaries.
- One-shot, Loop and Ping-pong/Bounce modes are explicit.
- Loop Release ON/OFF follows DEC-030 exactly.
- Env Pile-up defaults OFF (DEC-037).
- Loop Crossfade is L2 and is not shown as an R1 control.
- The Sampler A–H bank is the proposal in the encoder specification; unavailable targets remain disabled with a reason rather than silently remapped.

### 3.3 Sequencer

```
┌──────────────────────────────────────────────────────────────────────────┐
│ PATTERN NAME | LENGTH | RESOLUTION | BPM | QUANTIZE | SWING | HUMANIZE   │
├──────────────────────────────────────────────────────────────────────────┤
│ WINDOW: 17–32 of 64      [Previous] [Next] [Go to Step] [Follow: OFF]    │
├──────────────────────────────────────────────────────────────────────────┤
│ STEP GRID: selected step                                                   │
│ ┌──────────────────────────────────────────────────────────────────────┐ │
│ │ Event 1: Layer 1 | Velocity | Pitch | Parameter Locks                │ │
│ │ Event 2: Layer 3 | Velocity | Pitch | Parameter Locks                │ │
│ │ ... up to eight events per step                                      │ │
│ └──────────────────────────────────────────────────────────────────────┘ │
├──────────────────────────────────────────────────────────────────────────┤
│ EDIT: Add | Duplicate | Remove | Assign Layer | Mute | Unmute Timing     │
│ HISTORY: Undo | Redo | Save Pattern | Restore Draft                      │
│ SET: Chain | Performance Set | Select/Recall                             │
├──────────────────────────────────────────────────────────────────────────┤
│ PERFORMANCE ROW                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ FIXED 16-STEP STRIP                                                      │
└──────────────────────────────────────────────────────────────────────────┘
```

- Step selection and event selection are separate.
- Up to eight simultaneous events are visible through a compact event list/stack.
- Editing while playing shows pending changes until the end-of-bar activation boundary.
- Save is explicit; leaving a dirty Pattern creates a temporary Draft.
- Undo/Redo and event/layer assignment remain controller-reachable.
- Performance Set navigation is distinct from Program Change bank selection.

### 3.4 Modulation

```
┌──────────────────────────────────────────────────────────────────────────┐
│ TABS: Envelopes | LFO 1–4 | Matrix | Macros 1–10                         │
├──────────────────────────────────────────────────────────────────────────┤
│ LEFT: Source / row list            RIGHT: Selected source/editor          │
│                                  Waveform / rate / phase / sync            │
│                                  Poly/Mono (Mono default)                  │
├──────────────────────────────────────────────────────────────────────────┤
│ MATRIX: Enable | Source | Curve | Amount | Polarity | Destination        │
│ SUMMED MODULATION / RECENT DESTINATIONS                                  │
├──────────────────────────────────────────────────────────────────────────┤
│ MACROS: 1–10 for the selected layer Patch; base value + assignments      │
├──────────────────────────────────────────────────────────────────────────┤
│ PERFORMANCE ROW                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ FIXED 16-STEP STRIP                                                      │
└──────────────────────────────────────────────────────────────────────────┘
```

- Macro base values are shared within the selected layer Patch (DEC-031).
- Per-note modulation may still affect per-voice destinations.
- LFO Poly/Mono is per LFO; Mono is default (DEC-024).
- The matrix must show summed modulation and distinguish base from effective value.
- The view must not imply support for deferred CV or drawn XY trajectories.

### 3.5 FX

```
┌──────────────────────────────────────────────────────────────────────────┐
│ LAYER FX CHAIN: [Slot 1] → [Slot 2] → [Slot 3] → [Slot 4]                │
├──────────────────────────────────────────────────────────────────────────┤
│ SELECTED SLOT: Type | Enable/Bypass | Reorder | Replace                  │
├──────────────────────────────────────────────────────────────────────────┤
│ EFFECT-SPECIFIC PARAMETER PANEL                                         │
│ Only parameters defined for the selected R1 effect are displayed.       │
│ No universal Wet/Dry or generic parameter pair is assumed.               │
├──────────────────────────────────────────────────────────────────────────┤
│ TAIL POLICY: Linger / Cut                                               │
├──────────────────────────────────────────────────────────────────────────┤
│ PERFORMANCE ROW                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ FIXED 16-STEP STRIP                                                      │
└──────────────────────────────────────────────────────────────────────────┘
```

- R1 effects: Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator and Compressor.
- Each layer has four reorderable serial slots.
- Controls are effect-specific. The encoder focus list is generated from the selected effect's approved parameter inventory.
- L2/L3 effects and master-bus FX are not shown as R1 options.

### 3.6 Mix

```
┌──────────────────────────────────────────────────────────────────────────┐
│ FOUR-LAYER OVERVIEW                                                      │
│ L1 [Level] [Pan] [Mute] [Solo] [Freeze] [Record Status]                  │
│ L2 [Level] [Pan] [Mute] [Solo] [Freeze] [Record Status]                  │
│ L3 [Level] [Pan] [Mute] [Solo] [Freeze] [Record Status]                  │
│ L4 [Level] [Pan] [Mute] [Solo] [Freeze] [Record Status]                  │
├──────────────────────────────────────────────────────────────────────────┤
│ MASTER: Output meter | Stereo Master Record Status | Record Target      │
│ RECORD TARGET: Layer L1–L4 or Stereo Master | Mode | Arm | Start | Stop │
├──────────────────────────────────────────────────────────────────────────┤
│ PERFORMANCE ROW                                                          │
├──────────────────────────────────────────────────────────────────────────┤
│ FIXED 16-STEP STRIP                                                      │
└──────────────────────────────────────────────────────────────────────────┘
```

- Mix provides a compact four-layer overview while retaining one selected layer for detailed editing.
- Level, Pan, Mute, Solo and Freeze are explicit per-layer states.
- Recording target selection distinguishes L1–L4 from Stereo Master.
- Arm, Start and Stop are separate. Do not imply simultaneous multi-target capture until explicitly validated.
- No new output-routing or master gain stage is implied by the layout.

## 4. Library, Performance Bank and Performance Set overlays

These are overlays/panels, not additional primary workspaces.

### 4.1 Preset Library

- Search, filter by bank/tag/favourite, browse and select a library Preset.
- Selection does not itself recall; a separate Recall action performs the transition.
- Show current Preset and unsaved/dirty state.
- Preset recall does not replace or stop the active Pattern.

### 4.2 Performance Bank editor

- Show PC values 0–127 and explicit Preset assignment status.
- Assign, Replace, Unassign and Browse Library are distinct actions.
- Show the currently active bank.
- Unmapped Program Change values are ignored (DEC-033); never default to an arbitrary Preset.

### 4.3 Performance Set editor

- Show ordered entries with index, Preset/Pattern type and current/selected/pending/active state.
- Previous/Next navigates; Recall activates.
- A Performance Set is not the Program Change lookup table.
- Pattern switching while playing follows the end-of-bar rule.

### 4.4 MIDI mapping editor

- Separate Global and Preset-specific mapping scopes.
- Show physical source (channel + CC/Note/NRPN where supported), target name/scope, conflict state and soft-takeover state.
- Provide Learn, manual assign, reassign, unmap, reset-scope, confirm and cancel.
- No silent conflict replacement; no R1 MIDI feedback assumptions.
- A performer can use existing mappings and recall saved mapping state without computer keyboard/mouse.

## 5. Global status and error presentation

Use text/icon plus color, never color alone:
- **Pending:** operation requested but not yet active (e.g. Pattern switch at bar end, Freeze capture).
- **Active:** engine confirms the new state.
- **Already Static:** source is already a static library sample; redundant Freeze is disabled/reported.
- **Disabled:** action is unavailable in current context, with a short reason.
- **Failed:** operation did not complete; preserve the prior safe state and expose recovery guidance.
- **Dirty/Draft:** unsaved Pattern/Patch state is clearly distinguished from saved state.

Panic remains available in all workspaces and overlays. Modal overlays must not block Panic.

## 6. Open review items

- Owner review of the A–H assignments for both engines.
- Owner review of the Chord Record Arm/Capture/Commit/Cancel interaction.
- Owner review of recording target selection and Arm/Start/Stop presentation; concurrency policy remains unclaimed.
- Owner review of Performance Bank editor and active-bank selection/persistence.
- Owner review of consolidated global frame and persistent performance row placement.
- Confirm screen-level target sizes and exact display dimensions only when hardware/display selection is available.
- Phase 3 defines stable parameter IDs, mapping protocol, parameter exposure and scaling.
- Phase 5 defines acceptance tests and measurable performance/usability thresholds.

## 7. Sign-off checklist

- [ ] Global frame is consistent across all six workspaces.
- [ ] Panic is always visible and remains usable during overlays/error states.
- [ ] Fixed 16-step strip is persistent and clearly labeled as a viewport.
- [ ] Every essential live operation has a visible path and controller target.
- [ ] Layer focus never redirects explicit per-layer mappings.
- [ ] Granular and Sampler A–H banks match the approved R1 parameter inventory.
- [ ] FX controls are effect-specific and R1-only.
- [ ] Chord Record and audio recording have distinct stateful workflows.
- [ ] Program Change uses explicit active Performance Bank assignments only.
- [ ] Performance Set navigation remains distinct from Performance Bank navigation.
- [ ] Pending/Active/Failed/Disabled/Already Static states are unambiguous.
- [ ] Phase 5 acceptance coverage is defined before implementation sign-off.

## 8. Non-goals

This document does not define pixel-perfect wireframes, touch-target measurements, display hardware, framework, stable parameter IDs, MIDI wire format, DSP, audio routing, resource limits, host integration, Phase 5 acceptance thresholds or production implementation. It does not authorize merge.

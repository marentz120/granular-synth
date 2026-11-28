# GRANULAR SYNTH — Controller Action & Target Catalogue

**Version:** 0.1  
**Status:** Phase 4 UX proposal — owner review required  
**Scope:** UI-level controller target names, scopes, action classes and performance workflows. Documentation only.  
**Authority:** `LIVE_PERFORMANCE_SPECIFICATION.md`, `R1_PRODUCT_SPECIFICATION.md`, `GRANULAR_SYNTH_REQUIREMENTS.md`, `MIDI_MAPPING_UX_SPECIFICATION.md`, and approved DEC-015…DEC-042. This catalogue does not define MIDI wire format, parameter IDs, implementation APIs or Phase 5 acceptance thresholds.

## 1. Purpose and governing rules

The Phase 4 accessibility audit found that a generic MIDI mapping concept is not enough: each essential operation needs an identifiable target, an explicit scope, a predictable input behavior and visible confirmation.

This catalogue closes that UI-level gap without claiming that controller-only operation has been implemented or tested.

Governing rules:

1. **Controller-only means no hidden computer dependency.** Once a performance is underway, every essential operation in `LIVE_PERFORMANCE_SPECIFICATION.md` §1 must be reachable with the connected generic MIDI controller alone.
2. **Target scope is explicit.** A target bound to Layer 3 remains Layer 3 even when the touchscreen's selected layer is Layer 1. UI focus must never retarget an existing mapping.
3. **Selection and execution are separate.** Navigation selects a layer, Pattern, step or event; a distinct action changes or executes it. This prevents an encoder focus change or navigation message from triggering a destructive operation.
4. **Action classes are not interchangeable.** Continuous values use absolute/relative control and optional soft takeover. Toggles, momentary actions, navigation and commands do not use continuous soft takeover.
5. **The instrument display is authoritative.** R1 does not require MIDI feedback to controller LEDs, rings or displays (DEC-021). Every essential operation therefore has visible on-screen state/confirmation.
6. **No success on message receipt alone.** Show Pending, Active/Completed or Failed according to the actual instrument state. A received MIDI message is not proof that the requested operation succeeded.
7. **No invented parameter inventory.** A target is included only when its underlying R1 behavior exists. Exact parameter IDs, exposed parameter count and implementation representation remain governed by Q-T12 and Phase 3.

## 2. Target classes

| Class | Input behavior | UI meaning | Examples |
|---|---|---|---|
| Continuous parameter | Absolute or relative value | Sets a value within the target's valid range | Macro value, Scan, Pitch, layer level, Fill intensity |
| Toggle | Alternates explicit Off/On state | Changes a persistent mode/state | Fill, Freeze, layer Mute, event Mute, Play/Drone |
| Momentary action | Executes on press; release behavior is target-specific | One-shot or held action | Panic; Chord Hold |
| Navigation | Next/previous or bounded increment/decrement | Moves current selection/window | Pattern next/previous, step window, event selection |
| Selection | Selects one explicit item | Sets focus without changing sound | Layer L1–L4, Pattern, step, event |
| Command / transaction | Explicit trigger with Pending/Success/Failure feedback | Performs a stateful operation | Save, Undo, Redo, record start/stop, mapping recall |
| Mode sequence | Enters/exits a temporary interaction mode | Controls a workflow state | MIDI Learn enter/exit, arm/confirm/cancel |

A single physical control may be mapped to one target at a time in a given scope. A mapping must not silently change class or target after a preset recall.

## 3. UI-level target catalogue

Names below are human-readable target labels, not stable IDs or MIDI assignments. “Per-layer” means an explicit L1, L2, L3 or L4 target exists; it does not mean “whatever layer is currently selected”.

| Domain | Target labels / actions | Class | Scope |
|---|---|---|---|
| Layer focus | Select Layer L1, L2, L3, L4 | Selection | Global |
| Engine | Select Granular / Sampler for focused layer | Selection / command | Explicit layer |
| Core engine controls | Eight logical engine controls, slots A–H, remapped by engine | Continuous parameter | Explicit layer |
| Granular performance | Scan/Position; Pitch; Size; Density; Spread; Spray; Pan Spray; Direction, where each is an approved R1 control | Continuous parameter | Explicit layer |
| Sampler performance | Scan; Pitch/playback rate; Spray; Pan Spray; Direction; approved sample/loop boundary controls | Continuous parameter or selection | Explicit layer |
| Layer mix | Layer Level; Layer Pan; Layer Mute; Layer Solo | Continuous / toggle | Explicit layer |
| Play/drone | Play/Drone mode | Toggle | Explicit layer |
| Panic | Panic / All Sound Off | Momentary command | Global |
| Macros | Macro 1–10 value; Macro 1–10 select/focus | Continuous / selection | Active patch; explicit layer context |
| XY performance pad | X value; Y value | Continuous parameter | Active patch/layer context as defined by R1 |
| Fill | Fill On/Off; Fill Intensity | Toggle / continuous parameter | Global, affects active layers |
| Freeze | Freeze L1, L2, L3, L4; Freeze All; Thaw L1…L4; Thaw All | Toggle / command | Explicit layer or global |
| Transport | Start; Stop; Continue where supported by the approved transport model | Command | Global |
| Pattern navigation | Previous Pattern; Next Pattern; Select/Recall Pattern; Pattern Chain Start/Stop; Previous/Next Chain Item | Navigation / command | Global |
| Performance Set | Previous/Next Item; Select/Recall Item | Navigation / command | Global |
| Sequencer viewport | Previous/Next 16-step window; Select step; Go to step; Follow Playhead On/Off | Navigation / selection / toggle | Active Pattern |
| Step/event editing | Select previous/next event; select event index; add event; remove selected event; assign selected event to Layer L1–L4 | Selection / command | Active Pattern / selected step |
| Event values | Event Velocity; Event Pitch; eligible event parameter-lock values | Continuous parameter | Selected event |
| Sequencer timing | BPM; Swing; Humanize; Quantization; Microtiming; straight/triplet grid selection; Pattern length and edit resolution | Continuous / selection | Global or active Pattern per R1 semantics |
| Mute editing | Mute/Unmute Layer; Mute/Unmute Step; Mute/Unmute Event; select unmute timing (immediate or next occurrence) | Toggle / selection | Explicit layer / active Pattern / selected event |
| Pattern history | Save Pattern; Restore Draft; Undo; Redo | Command | Active Pattern |
| Chord workflow | Chord Hold; Chord Record Start; Chord Record Stop/Commit | Momentary / command | Active layer/patch context |
| Recording | Arm/Start/Stop Layer Recording L1–L4; Arm/Start/Stop Master Recording; select One-shot/Loop mode | Command / selection | Explicit layer or global |
| MIDI mapping | Enter Learn; Exit Learn; Confirm; Cancel; Unmap; Reset Global Scope; Reset Preset Scope; Recall Global/Preset Mapping State | Mode / command | Global or active preset |

### 3.1 Explicit scope rules

- Layer-specific mappings are addressed directly to L1–L4. Changing the visible layer does not redirect them.
- Global Panic, transport, Fill, Pattern navigation and Performance Set navigation remain global.
- Event-level controls require a selected event. If no event is selected, the UI shows “No event selected” and does not silently apply the command to another event.
- Macro targets identify Macro 1–10, not an implicit “current macro”. A separate selection target may change focus, but it does not change the destination of an existing mapping.
- Freeze has distinct per-layer and global targets. A single selected-layer Freeze target is not sufficient.
- Layer recording and master recording are separate targets and must have separate status indicators.

### 3.2 R1 boundary corrections

- Sampler Loop Crossfade (GS-SMP-006) is L2 and is excluded from this R1 catalogue.
- The catalogue does not assume a universal Wet/Dry or parameter pair across the seven R1 effects. Each effect exposes its own approved parameter list; the effect inventory and parameter-level open items remain authoritative in `EFFECTS_SPECIFICATION.md`.
- Modulation destinations include only eligible continuous R1 parameters. Discrete/enum parameters are excluded unless an explicit R1 requirement permits them.
- No MIDI feedback, named-controller template, CV input, Slice engine, Tape engine, Spectral engine, or L2/L3 effect is implied.

## 4. Controller-only performance workflows

These are proposed interaction sequences. They describe UI behavior, not MIDI protocol or implementation.

### 4.1 Address a specific layer

1. Select Layer L1–L4, or invoke a mapping already bound to an explicit layer.
2. Confirm the selected layer in the instrument display.
3. Invoke the desired per-layer target.
4. Confirm the resulting value/state beside that layer.

A mapping explicitly bound to L3 bypasses step 1 and always affects L3. The currently selected touchscreen layer is never an override.

### 4.2 Edit a sequencer event

1. Select the active Pattern.
2. Navigate the 16-step viewport or go directly to a step.
3. Select the step.
4. Select one event from the events on that step; show its layer assignment and event index.
5. Invoke Add/Remove, Layer Assignment, Velocity, Pitch or an eligible parameter-lock target.
6. Show pending bar-boundary changes separately from the currently sounding state.
7. Confirm when the edit becomes musically active.

The UI must support up to eight simultaneous events per step. No operation in this sequence may depend on mouse position, keyboard focus or an undocumented gesture.

### 4.3 Pattern and Performance Set navigation

- Previous/Next changes the browse/selection cursor; Recall requests activation.
- During playback, Pattern activation is shown as Pending until the approved end-of-bar boundary.
- Pattern selection and Preset recall remain separate: recalling a Preset does not replace or stop the active Pattern.
- Performance Set navigation shows the current item and whether it is selected, pending or active.

### 4.4 Freeze and recording

- Freeze L1–L4 and Freeze All are separate targets; the display shows each layer's Off, Pending, Frozen, Already Static or Failed state in accordance with DEC-042.
- Thaw is distinct from Panic, Stop and Mute.
- Layer recording and master recording each expose their own Arm/Start/Stop and status. One-shot/Loop mode is explicit.
- Recording continues independently of Freeze as required by DEC-042.
- A command failure remains visible and recoverable; no optimistic “recording” or “frozen” state is shown before confirmation.

### 4.5 Chord hold and chord recording

- Chord Hold is a held/momentary operation and must not be confused with Chord Record.
- Chord Record has visible Idle, Recording and Completed/Cancelled states.
- Exact start/stop/commit gesture and whether recording is armed or immediately begins remain a Phase 4 owner-review item; the catalogue does not invent a new chord-recording product behavior.

### 4.6 MIDI Learn during a set

- Existing mappings are always usable without entering Learn.
- Enter/Exit Learn and Confirm/Cancel must be reachable through the controller if the performer needs to adjust mappings mid-set.
- Mapping creation may use a focused UI target plus a captured controller event, but the user must be able to complete or cancel the operation without a computer keyboard/mouse.
- A conflict presents Replace / Keep Existing / Cancel; no mapping is silently overwritten.
- A saved global or active-preset mapping state can be recalled with explicit confirmation and visible active-state indication.

## 5. Core eight-control engine section — evidence and decision boundary

R1 requires an eight-control engine section whose functions remap per engine (GS-UI-002). The benchmark feature map identifies the GR-MEGA's eight logical controls A–H and says Scan and Pitch retain their meaning across engines. The product baseline requires Granular and Sampler only.

The current Phase 4 repository documents do **not** contain a verified, complete R1 A–H parameter assignment table for both engines. The current encoder proposal's four-encoder matrix is not a substitute for GS-UI-002. The following rules therefore apply:

- Reserve eight visible/logical slots A–H for each selected engine and layer.
- Each slot must display its target name, value, unit/range where defined, and scope.
- Engine change replaces the slot assignment list; it must not change underlying parameter values.
- Scan and Pitch retain their cross-engine meaning as a benchmark-aligned design objective, subject to the approved R1 parameter definitions.
- Granular candidates such as Rate, Grain Size, Pan Spray, Spray, Direction, Scan, Pan and Pitch must be checked against the authoritative R1 Granular parameter inventory before being committed as the product mapping.
- Sampler must not expose granular-only Rate/Grain Size as active controls. Its candidate set must come from `SAMPLER_SPECIFICATION.md`: Pitch/playback rate, Direction, Spray, Pan Spray, Scan and approved sample/loop controls.
- Any unfilled slot is explicitly shown as “Unassigned” until the owner approves a real R1 parameter. Do not invent a parameter merely to fill all eight positions.
- The exact A–H assignments and any alternate modifier behavior are a blocking owner-review deliverable for Phase 4 sign-off.

This boundary is intentional: the benchmark's physical control table is reference evidence, not permission to add or rename R1 parameters. Exact parameter exposure and stable IDs remain a Phase 3/Q-T12 technical gate.

## 6. Open owner-review items

| Item | Why it remains open | Required closure |
|---|---|---|
| A–H mapping for Granular and Sampler | No complete approved R1 per-engine mapping table exists in the current product baseline | Review a requirement-backed assignment matrix; explicitly approve or defer each slot |
| Chord Record gesture | GS-CHD-004 requires chord recording but does not define the performance gesture/state sequence | Confirm Arm/Start/Stop/Commit behavior |
| Macro scope and selection | Ten macros are per patch; per-layer UI focus and explicit MIDI target semantics need final alignment | Confirm whether macro values are patch-global or layer-contextual in the live UI |
| Pattern/Performance Set recall | R1 defines recall and end-of-bar Pattern activation; detailed navigation and PC assignment remain Phase 4/5 work | Approve selection vs recall and Program Change mapping behavior |
| Recording arm semantics | R1 requires independent per-layer and stereo master recording; detailed controller workflow is not yet normative | Confirm whether Arm is a separate state or Start immediately begins capture |
| Mapping-state recall | Global and preset-specific mappings are described; a separate named mapping-set library is not an approved R1 feature | Keep recall within approved global/preset state unless separately approved |

## 7. Phase 4 sign-off checklist

- [ ] Every operation in `LIVE_PERFORMANCE_SPECIFICATION.md` §1 has a named target and explicit scope.
- [ ] Every target has one action class and visible state/feedback.
- [ ] Every per-layer target can address L1–L4 without relying on selected-layer focus.
- [ ] Eight A–H controls are specified for both R1 engines from approved parameter inventories.
- [ ] Sampler L2 Loop Crossfade is absent from R1 mappings.
- [ ] FX targets are resolved per effect; no universal Wet/Dry assumption is made.
- [ ] Up to eight events per step can be navigated and edited controller-only.
- [ ] Freeze, Panic, Mute, Stop and recording remain distinct.
- [ ] Chord Hold and Chord Record are distinct and have owner-approved workflows.
- [ ] Program Change, Pattern recall and Performance Set navigation are consistent with the preset model.
- [ ] All owner decisions are recorded in `DECISIONS_LOG.md`; technical gates remain in `OPEN_QUESTIONS.md`.
- [ ] Phase 5 acceptance tests are planned for every essential operation; this document does not claim test coverage.

## 8. Non-goals

This document does not select a framework, define stable parameter IDs, choose a MIDI mapping wire format, set mapping capacity, specify real-time implementation, approve new product behavior, close Phase 5 acceptance criteria, authorize production code, or authorize merge.

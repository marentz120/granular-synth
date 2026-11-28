# GRANULAR SYNTH — R1 Live-Performance Accessibility Audit

**Version:** 0.1  
**Status:** Phase 4 discovery audit — owner review required  
**Scope:** Controller-only performance reachability and visible UI access. Documentation only; no implementation or product-scope change.  
**Authority:** `LIVE_PERFORMANCE_SPECIFICATION.md` §1 and §8, `R1_PRODUCT_SPECIFICATION.md`, `GRANULAR_SYNTH_REQUIREMENTS.md`, DEC-016/019/020/021, and the Phase 4 UX proposals.

## 1. Executive finding

**Result: NOT READY FOR UI SIGN-OFF.** The current Phase 4 proposal establishes useful navigation and control surfaces, but it does not yet demonstrate end-to-end reachability for every operation in the approved R1 essential-performance set.

The principal issue is not the number of physical encoders. R1 explicitly supports generic MIDI controllers with user-created mappings, so controller ergonomics depend on the connected device. The unresolved issue is the complete action/target model and a deterministic way to navigate, select, execute and verify every essential operation using only that controller.

This audit distinguishes:
- **Visible proposal:** a plausible touchscreen location exists in Phase 4 documents.
- **MIDI target specified:** the operation is explicitly identified as a mappable action/parameter.
- **End-to-end proven:** the user can select the target, map it, invoke it, and observe the resulting state without keyboard/mouse. No operation is currently marked proven by this documentation audit; Phase 5 tests are required.

## 2. Coverage matrix — essential performance operations

| Essential operation (Live Performance §1) | Visible UI proposal | MIDI/controller-only status in current docs | Finding |
|---|---|---|---|
| Preset/patch recall and Program Change | Preset browser / performance surface; persistent row mentions recall | Program Change recalls explicit assignments in active performance bank; MIDI mapping list and recall UX remain incomplete | **Partial.** Define bank/slot assignment, next/previous navigation, unmapped-PC behavior display and active-bank recall path. |
| Layer mix level and per-layer performance controls | Mix workspace; selected-layer level/pan proposal | All mappable controls should be targetable, but exact layer-control target inventory and direct layer addressing are not fully enumerated | **Partial.** Layer selection must not redirect a mapping explicitly bound to another layer. Provide direct L1–L4 targeting. |
| Play/drone mode per layer | No explicit control in encoder/performance matrix | Not explicitly listed as a MIDI action target in the Phase 4 mapping UX | **Gap.** Add visible per-layer mode control and distinct MIDI action/parameter targets; do not conflate with transport Play. |
| Panic / All Notes Off / All Sound Off | Persistent Panic control | Required as one-gesture controller action; no concrete action-target definition or verification workflow | **Partial.** Specify one unambiguous action target and visible confirmation; Panic must remain available during recovery/error states. |
| Up to 10 macro controls | Modulation workspace mentions macros; persistent row does not define macro bank/selection | Macro control is explicitly essential, but mapping UX does not define macro bank navigation or complete macro target addressing | **Gap.** Expose Macro 1–10 values, layer/patch scope, bank/page navigation and controller targets. |
| Core 8-knob engine section per layer | Four encoders per workspace only; no complete eight-control engine section | Generic mapping principle exists, but eight primary controls per layer are not enumerated as a stable performance surface | **Gap.** Identify the actual eight R1 controls per engine/layer from authoritative parameter inventory. Do not assume the four physical encoders satisfy eight controls. |
| Granular scan/position and pitch | Granular workspace exposes Position and Pitch; Size/Density/Spread also visible | Encoder-focus proposal and general MIDI mapping cover parameters in principle | **Partial.** Explicitly include scan/position, pitch and per-layer scope in MIDI target inventory; verify Spread and remaining primary controls. |
| Chord hold/record | Chord/arp/scale subpanels are proposed in traceability audit; no persistent performance action | Live Performance §1 explicitly requires chord hold/record; Phase 4 encoder/performance document only mentions it in contextual actions | **Gap.** Define visible hold/record state, arm/record/finish actions and controller targets; distinguish hold from recording. |
| Sequencer transport | Persistent Play/Stop | Persistent actions are stated to be mappable; no end-to-end mapping/navigation contract | **Partial.** Define transport action targets, start/stop semantics and status confirmation. |
| Pattern selection and chain | Persistent Pattern navigation proposal | Controller-reachable in principle; mapping-set and action model unresolved | **Partial.** Define next/previous/recall/chain actions and pending-at-bar-end feedback. |
| Step/event editing and layer assignment | Fixed 16-step strip plus expanded editor; up to eight events per step | Explicitly required to be MIDI reachable, including editing; action/event mapping model remains open | **Gap.** No complete controller-only interaction sequence for selecting step, selecting event, add/remove/duplicate, edit values and assign layer. |
| Fill toggle and continuous intensity | Persistent Fill toggle/intensity | Explicitly mappable in Phase 4 proposal | **Partial.** Define discrete toggle vs continuous intensity targets and latched-state feedback. |
| Layer/event Mute | Persistent layer Mute/Solo proposal; event mute in contextual actions | Immediate layer/event Mute is required; unmute timing selectable for individual event | **Partial.** Define separate layer mute, step mute and event mute targets, plus explicit unmute timing selection. Solo is not a substitute for Mute. |
| Pattern Save, Draft, Undo, Redo | Contextual Sequencer/Pattern surface | Listed as controller-reachable in proposal, but no complete action-target schema or interaction sequence | **Partial.** Define Save, restore Draft, Undo and Redo as distinct discrete actions with dirty/pending/success feedback. |
| Per-layer and global Freeze | Persistent global and selected-layer Freeze | Both are required and mappable; Freeze state labels are specified | **Partial.** Selected-layer control alone is insufficient if the performer needs to target any of L1–L4 without changing focus. Define explicit L1–L4 Freeze targets or a controller-accessible layer-select-then-freeze sequence. |
| MIDI Learn entry/exit | MIDI Mapping workspace; shortcut remains open | Controller reachability is required where needed; shortcut and no-mouse workflow remain unresolved | **Gap.** Define a controller-only entry/exit and confirmation/cancel path. Detailed creation of new mappings may remain setup-time only where consistent with Live Performance §8. |
| Recall a saved mapping set | Mapping workspace / contextual action | Explicitly required when needed mid-set; mapping-set recall and persistence model remain open | **Gap.** Define controller target, active-set indication, safe switching behavior and failure feedback. |
| Per-layer recording and master recording | Persistent recording status; proposed record controls | Status is proposed, but independent arm/start/stop/select/mode actions are not fully mapped | **Gap.** Distinguish each layer's recording from stereo master capture; specify one-shot/loop selection, start/stop, active/armed/error state and controller targets. |

## 3. Additional R1 sequencer operations requiring explicit controller paths

The Phase 4 proposal must account for the complete R1 sequencer behavior, not only the visible 16-step strip.

| Operation | Current proposal | Required clarification |
|---|---|---|
| Select Pattern / navigate Pattern Library | Pattern navigation proposed | Define browse/next/previous/recall targets and distinguish library selection from active Pattern. |
| Pattern chain and Performance Set navigation | Listed as contextual actions | Define select/next/previous/start/stop chain actions and current item feedback. |
| Variable Pattern length and edit resolution | Viewport paging proposed | Define controller actions for changing length/resolution only if those are performance-time needs; at minimum, editing must not depend on mouse/keyboard. |
| >16-step viewport | Previous/Next window and Go to step proposed | Define MIDI targets for previous/next window, direct step selection and playhead-follow toggle if retained. |
| Up to eight events per step | Expanded editor proposed | Define event index selection and deterministic navigation when multiple events occupy one step. |
| Event parameter editing / locks | Encoder placeholder proposed | Validate the actual lockable parameter inventory; do not invent a universal event-lock parameter. |
| Event/layer assignment | Encoder placeholder proposed | Define discrete layer assignment actions and feedback for all four layers. |
| Quantization, triplets, Swing, Humanize, Microtiming | Sequencer workspace implied | Ensure all R1 controls have visible surfaces and MIDI Learn targets; exact performance-time subset needs review. |
| Immediate mute and unmute timing | Event mute mentioned | Define distinct mute/unmute action and timing choice; show pending vs active state. |
| End-of-bar activation | Pending state rule documented | Show queued Pattern/edit changes and confirm activation at bar boundary. |
| Draft behavior and Undo/Redo | Contextual actions listed | Define what happens when switching away from an unsaved Pattern, returning to Draft and undoing/redoing edits. |

## 4. Parameter-inventory corrections required before sign-off

The current four-encoder matrix must not be treated as an authoritative R1 parameter map.

1. **Sampler:** the proposed Loop Crossfade is L2 in `GRANULAR_SYNTH_REQUIREMENTS.md` (GS-SMP-006), not R1. Remove it from the R1 default matrix. R1 includes playback direction/mode, loop and looped-release behavior, flags and interpolation. Map only parameters confirmed by the R1 Sampler specification.
2. **FX:** R1 includes seven effect types and four serial slots per layer. There is no evidence that every effect shares a common pair of parameters or a universal Wet/Dry control. Resolve assignments per effect from the authoritative FX inventory.
3. **Modulation:** four ADSR encoder assignments are not a substitute for the complete R1 modulation/voice parameter inventory. R1 includes four LFOs per layer, a modulation matrix, XY performance pad, 10 macros and four voice envelopes. Ensure all essential live targets are reachable.
4. **Mix:** do not introduce a new output selector, master-record gain stage or routing control unless an authoritative R1 requirement defines it. The encoder proposal itself warns against inventing these controls.
5. **Core eight-knob section:** Live Performance §1 calls this out explicitly. The Phase 4 four-encoder proposal does not identify the eight controls per engine/layer or explain how a performer reaches all eight without hidden gestures.

## 5. MIDI action model — blocking UX dependency

The current MIDI Mapping UX document correctly notes that transport, Panic, Pattern recall and step editing are not continuous parameters. However, the complete action/event binding model is still open.

Before UI sign-off, specify a UI-level target taxonomy (not an implementation/API decision):

- **Continuous parameter:** value-bearing control, e.g. macro, scan, pitch, layer level, Fill intensity.
- **Momentary action:** invoke while held or on press, e.g. Panic, chord hold, step/event selection.
- **Toggle action:** explicit on/off state, e.g. Fill, Freeze, Mute, Play/Drone mode where applicable.
- **Increment/decrement or navigation action:** next/previous Pattern, viewport, macro bank, Performance Set item.
- **Selection action:** select layer, step, event, Pattern or mapping set.
- **Mode/action sequence:** enter/exit Learn, record arm/start/stop, Save, Draft restore, Undo, Redo.

For every target, the UI must define:
- Stable human-readable target name and scope (global, layer L1–L4, active Pattern, selected event, active Performance Set).
- Whether the mapping is momentary, toggle, absolute or relative.
- What happens on repeated input and on controller disconnect/reconnect.
- Visible state/feedback after the action; never infer success merely because a MIDI message arrived.
- Conflict behavior and soft-takeover applicability. Soft takeover applies to value controls; it must not be blindly applied to discrete actions.

This is a Phase 4 UX contract only. Mapping wire format, IDs, capacity and real-time implementation remain technical gates.

## 6. Freeze and recording consistency checks

- Freeze controls must distinguish Global from each individual layer; selected-layer focus must not silently change the target of a previously mapped action.
- UI states must include Off/live, capture pending, frozen snapshot active, already-static source and failure.
- Per DEC-042, existing grains/voices continue; new notes use the immutable snapshot; modulation, MIDI, sequencer, transport, FX and independent recording continue; thaw crossfades to the prior live/rolling source.
- Recording controls/status must distinguish independent per-layer capture from stereo master capture.
- One-shot/loop recording and buffer behavior must follow the approved R1 sample/recording requirements.
- Panic, Stop, Mute and Freeze are separate operations; none should be visually or semantically conflated.

## 7. Accessibility and usability risks

- **Focus ambiguity:** encoder focus, selected layer, selected event and MIDI target scope are different states. Show each explicitly.
- **Hidden depth:** a “More” page is acceptable only if every essential control has a deterministic route and can be selected/edited from a generic MIDI controller.
- **Touch-only setup dependency:** setup can use the touchscreen, but a performer must be able to use existing mappings, recall saved mapping sets and perform every essential operation without keyboard/mouse.
- **No feedback hardware:** R1 has no MIDI feedback requirement. Therefore, the instrument screen must remain the authoritative state display; do not assume controller LEDs or displays confirm an action.
- **Color:** critical states must not be communicated by color alone.
- **Error recovery:** Panic remains one deliberate gesture and available during error/recovery states. Failed/pending actions must be visible and must not be presented as successful.

## 8. Required closure items before Phase 4 UI sign-off

- [ ] Publish the authoritative R1 parameter inventory for the eight-knob engine section, per engine and layer.
- [ ] Remove L2 Sampler Loop Crossfade from R1 encoder defaults.
- [ ] Resolve each FX encoder assignment against each approved effect's actual parameter list.
- [ ] Add explicit UI and MIDI targets for Play/Drone per layer, Panic, chord hold/record, macros 1–10, preset/PC recall and mapping-set recall.
- [ ] Define controller-only layer targeting for per-layer Freeze, recording, mix and performance controls.
- [ ] Define the non-parameter MIDI action/event target model and controller-only navigation sequence.
- [ ] Complete step/event editing paths, including eight events per step and layer assignment.
- [ ] Define per-layer recording and master recording action/status workflows.
- [ ] Define macro bank/page navigation and active macro scope.
- [ ] Verify every operation in `LIVE_PERFORMANCE_SPECIFICATION.md` §1 against the final wireframe and MIDI target list.
- [ ] Add Phase 5 acceptance tests for every operation; do not claim coverage based solely on this document.

## 9. Owner decisions vs. technical gates

No new product behavior is approved by this audit. The following are **owner UX decisions**: persistent control hierarchy, visible grouping, encoder defaults, viewport-follow default, and whether selected-layer Freeze is supplemented by direct L1–L4 controls.

The following are **technical/Phase 5 gates**, not decisions to invent in this UI document: mapping wire format/capacity, action-event implementation, stable parameter IDs, soft-takeover algorithm, device hot-plug identity, real-time safety and measurable acceptance thresholds.

## 10. Conclusion

The current Phase 4 UX is a useful structural proposal, but it is not yet a complete live-performance control specification. The next document revision should close the visible/action/MIDI target map first; only then should a consolidated wireframe be treated as a reviewable candidate. No implementation, DSP work, framework choice, benchmark or merge is authorized by this audit.

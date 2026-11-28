# GRANULAR SYNTH — R1 Live-Performance Accessibility Audit

**Version:** 0.2  
**Status:** Phase 4 discovery audit — owner review required  
**Scope:** Controller-only performance reachability and visible UI access. Documentation only; no implementation or product-scope change.  
**Authority:** `LIVE_PERFORMANCE_SPECIFICATION.md` §1/§8, R1 product specifications, `PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md`, `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md`, `PHASE4_CONSOLIDATED_SCREEN_LAYOUTS.md`, and DEC-016/019/020/021/031/033/042.

## 1. Executive finding

**Result: UX coverage is now specified at proposal level; not ready for owner sign-off or implementation.**

The Phase 4 documents now provide:
- A named target catalogue with explicit global, layer, Patch, Pattern and event scopes.
- A proposed Granular and Sampler A–H control bank, grounded in R1 requirement IDs.
- Controller-only workflows for layer addressing, sequencer event editing, chord recording, audio recording, Pattern/Performance Set navigation and Program Change.
- Consolidated screen layouts for all six workspaces, persistent performance controls, the fixed 16-step viewport and relevant overlays.

This closes the earlier documentation gaps, but does **not** prove end-to-end operation. No controller hardware, UI implementation, MIDI mapping behavior or Phase 5 acceptance tests have been executed. The distinction is:
- **Specified:** a visible surface, named action/parameter target, scope and proposed workflow are documented.
- **Owner review:** the proposed interaction is awaiting product/UI approval.
- **Proven:** only after implementation and Phase 5 testing. No item is marked proven by this audit.

## 2. Essential performance coverage matrix

| Essential operation | UI surface | Controller target/workflow | Current finding |
|---|---|---|---|
| Preset/patch recall and Program Change | Preset Library overlay; Performance Bank editor | Explicit PC 0–127 assignment in active bank; mapped value recalls Preset; unmapped value ignored (DEC-033) | **Specified; owner review** |
| Layer mix and per-layer performance controls | Mix workspace, explicit L1–L4 rows | Layer Level/Pan/Mute/Solo and explicit layer-scoped targets | **Specified; owner review** |
| Play/Drone per layer | Layer performance controls | Explicit Play/Drone target for L1–L4; separate from shared transport | **Specified; owner review** |
| Panic / All Sound Off | Persistent global performance row, all workspaces | One deliberate global action; distinct from Stop, Mute and Freeze | **Specified** |
| Macros 1–10 | Modulation workspace | Explicit Macro 1–10 target within a selected layer Patch; base values shared per Patch (DEC-031) | **Specified; owner review** |
| Core eight-knob engine section | Engine A–H logical bank; four physical encoders page A–D/E–H | Eight named R1 targets per engine and explicit layer scope | **Specified as proposal; owner review** |
| Granular Scan/Position/Pitch and primary controls | Granular workspace | Scan, Position, Pitch, Size, Rate, Spray, Pan, Pan Spray; Density displayed as derived Rate × Size | **Specified; owner review** |
| Chord Hold and Chord Record | Chord workflow in layer Patch | Select slot 1–12; Arm; Capture; Stop & Commit or Cancel; Chord Hold separate | **Specified as proposal; owner review** |
| Sequencer transport | Persistent performance row | Shared Start/Stop/Continue semantics; distinct from audio recording | **Specified** |
| Pattern selection, recall and chain | Sequencer workspace / Performance Set panel | Browse/select separate from Recall; pending end-of-bar state | **Specified; owner review** |
| Step/event editing and layer assignment | Fixed 16-step viewport plus expanded Sequencer workspace | Select window/step/event; add/remove; assign L1–L4; edit eligible values/locks; visible pending/active state | **Specified; owner review** |
| Fill toggle and intensity | Persistent performance row | Separate latched toggle and continuous intensity targets | **Specified** |
| Layer, step and event Mute | Mix and Sequencer surfaces | Distinct mute targets; event unmute timing explicit | **Specified; owner review** |
| Pattern Save, Draft, Undo, Redo | Sequencer workspace | Separate discrete command targets with dirty/draft state | **Specified** |
| Per-layer and global Freeze | Persistent performance row and Mix workspace | Freeze L1–L4 and Freeze All; thaw separate; DEC-042 states visible | **Specified** |
| MIDI Learn entry/exit | MIDI Mapping overlay | Enter/Exit Learn, Confirm/Cancel, Unmap and reset-scope targets | **Specified; owner review** |
| Recall saved mapping state | MIDI Mapping overlay | Recall Global or active-Preset mapping state; no separate mapping-set library | **Specified; owner review** |
| Per-layer and Stereo Master recording | Mix workspace and persistent recording summary | Select target; One-shot/Loop; Arm; Start; Stop; Completed/Failed | **Specified as proposal; owner review** |

**Interpretation:** all essential operations now have a documented target/surface/workflow proposal. This is not evidence that every controller can operate them ergonomically or that implementation exists.

## 3. Sequencer completeness

The fixed 16-step strip is a viewport only. The expanded Sequencer workspace and controller catalogue cover:
- Previous/Next 16-step window and Go to Step.
- Step selection separate from step enable/mute.
- Event selection separate from step selection, including event index.
- Add, duplicate and remove event actions.
- Explicit assignment to Layer L1–L4.
- Velocity, Pitch and eligible parameter-lock editing.
- Up to eight simultaneous events per step.
- Pattern length and edit resolution, quantization, triplets, Swing, Humanize and Microtiming.
- Pending end-of-bar changes, explicit Save, temporary Draft, Undo and Redo.
- Event Mute and explicit unmute timing.

Remaining validation: the final event-lockable parameter inventory and the exact set of controls that are essential during a live set must be reconciled with the Phase 5 test plan. No parameter-lock target may be invented beyond R1.

## 4. Parameter-inventory and scope corrections

The current proposals explicitly address the known scope risks:

1. **Eight-knob engine bank:** A–H assignments are documented in `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md` §2.5. They use R1 requirement IDs; stable IDs, scaling and taper remain Phase 3/Q-T12/Phase 5 work.
2. **Granular Density:** Density is a derived display value (Rate × Size, GS-GRN-003), not an independent base parameter in the proposed A–H bank.
3. **Sampler:** Sample Start/Stop, Scan, Pitch/Rate, Spray, Pan Spray, Direction and Loop Mode are proposed. Loop Start/End remain in the waveform flag editor. Loop Crossfade (GS-SMP-006) is L2 and excluded.
4. **FX:** Encoder targets resolve against the selected effect's actual parameter inventory. No universal Wet/Dry or common parameter pair is assumed.
5. **Modulation:** Four LFOs per layer, matrix, XY pad and ten macros per layer Patch remain represented. Macro base values follow DEC-031; LFO Mono/Poly follows DEC-024.
6. **Mix:** No new routing selector or master-record gain stage is introduced by the layout.
7. **Layer scope:** A mapping explicitly bound to L1–L4 is never redirected by touchscreen focus.

## 5. MIDI action model

The controller catalogue defines the following UI-level classes:
- Continuous parameter.
- Toggle.
- Momentary action.
- Navigation.
- Selection.
- Command/transaction.
- Temporary mode sequence.

Each target has a human-readable name, explicit scope, action class and visible state/confirmation proposal. Soft takeover applies to continuous value controls, not blindly to discrete actions. Conflicts must be surfaced; mappings are not silently overwritten.

Technical items remain outside this UX audit: MIDI event wire format, stable parameter IDs, mapping capacity, controller normalization, device identity/hot-plug handling, and real-time thread ownership.

## 6. Freeze, Panic and recording consistency

- Freeze has explicit L1–L4 and Global targets; selected-layer focus is not a substitute for direct layer addressing.
- DEC-042 state presentation distinguishes live/off, pending, frozen snapshot, already static and failure.
- Freeze does not pause voices, sequencer, MIDI, modulation, transport, FX or independent recording.
- Thaw is distinct from Panic, Stop and Mute.
- Panic is one deliberate All Sound Off action and does not thaw Freeze.
- Recording targets are Layer L1–L4 or Stereo Master. The proposed Arm/Start/Stop flow does not assert concurrent multi-target capture.
- Transport Stop must not be shown as stopping audio recording; recording Stop is a separate action.

## 7. Remaining risks and owner review

- The A–H engine assignments are a requirement-backed proposal, not a benchmark-parity claim or approved final hardware mapping.
- Chord Record Arm/Capture/Commit/Cancel is an interaction proposal; note-count limits, duplicate handling and exact commit semantics require review.
- Recording target selection and Arm/Start/Stop are proposed; concurrency and resource policy remain engineering gates.
- Performance Bank and Performance Set are distinct. Active-bank selection/persistence and editor presentation require review; DEC-033's mapped-only Program Change behavior is fixed.
- The consolidated screen layouts are text wireframes. Touch-target dimensions, screen resolution and physical placement await display/hardware selection.
- A generic MIDI controller's physical ergonomics vary. R1 promises generic mapping and operation, not identical control layouts or MIDI feedback.

## 8. Closure progress and remaining gates

### Documentation-level closure completed
- [x] Named action targets and explicit scopes.
- [x] Controller-only layer targeting and sequencer event-edit sequence.
- [x] Proposed Granular/Sampler A–H engine banks.
- [x] Chord Record workflow proposal.
- [x] Layer/Master recording workflow proposal.
- [x] Performance Bank, Performance Set and Program Change distinction.
- [x] Consolidated layouts for six workspaces and Library/MIDI overlays.
- [x] README and PR #9 documentation index/summary updated.

### Owner UX review
- [ ] Approve or revise A–H assignments.
- [ ] Review Chord Record workflow.
- [ ] Review recording target and status workflow.
- [ ] Review Performance Bank editor and active-bank selection/persistence.
- [ ] Review global frame, persistent performance row and consolidated layouts.
- [ ] Confirm final performance-time subset for sequencer timing/edit controls.

### Phase 3 / Phase 5 technical and test gates
- [ ] Stable parameter IDs, exposure, scaling/taper and MIDI mapping protocol/capacity.
- [ ] Non-parameter action-event implementation and input normalization.
- [ ] Soft takeover, mapping conflicts, device hot-plug and recovery behavior.
- [ ] Acceptance tests for every essential operation, including controller-only operation.
- [ ] Validate audio-thread safety, responsiveness, and measurable performance/usability thresholds.

## 9. Authority and conclusion

Approved product behavior remains authoritative in the R1 specifications and DECISIONS_LOG.md. This audit records UI proposals only. It does not approve new product scope, implementation, hardware, DSP, technical protocol, acceptance thresholds or merge.

**Conclusion:** Phase 4 has moved from an incomplete action map to a coherent, cross-referenced UI/controller proposal. The next gate is owner review of the remaining UX proposals and a Phase 5 test-design pass; no item is claimed implemented or proven.

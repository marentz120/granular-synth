# GRANULAR SYNTH — Phase 4 Traceability Closure & Sign-off Readiness Report

**Version:** 0.1  
**Date:** 2026-11-28  
**Branch:** `phase4/ui-ux-specification`  
**PR:** #9 — Draft, open, not merged  
**Status:** Documentation audit complete; **Phase 4 owner sign-off is not granted**.  
**Scope:** Traceability and consistency review only. No product code, DSP, UI implementation, host prototype, benchmark or acceptance test execution.

## 1. Executive result

Phase 4 now has a coherent, cross-referenced UI/controller proposal covering the six primary workspaces, controller-only performance actions, the sequencer viewport, recording/Freeze states and the Performance Bank/Set distinction.

The audit confirms complete requirement-ID inventory coverage in the UI traceability table. This is a documentation result, not evidence of implementation, usability, controller reachability in a running product, or acceptance-test passage.

**Readiness decision:** suitable for owner review and Phase 5 test-design continuation; not ready for implementation sign-off or a claim of verified controller-only operation.

## 2. R1 requirement traceability

Source: `PHASE3_API_SYMBOL_TRACEABILITY.md` §2.  
Target: `PHASE4_R1_UI_TRACEABILITY_AUDIT.md` §2.

| Check | Result |
|---|---:|
| Unique R1 requirement IDs in Phase 3 inventory | 124 |
| Unique IDs in Phase 4 UI mapping table | 124 |
| Mapping rows | 124 |
| Missing source IDs | 0 |
| Extra IDs | 0 |
| Duplicate IDs in Phase 4 mapping table | 0 |

The Phase 3 source contains 129 requirement-bearing rows because some rows intentionally group multiple requirement IDs; those rows resolve to 124 unique R1 IDs. The Phase 4 UI table contains each of those 124 unique IDs exactly once.

## 3. Acceptance-criteria gap — remains open

The current Phase 3 baseline is unchanged:

- 49 requirements have a direct acceptance-criteria reference.
- 4 additional requirements have a cross-domain release-gate link.
- 53 of 124 therefore have some existing acceptance/release-gate linkage.
- 71 of 124 still lack either a direct acceptance reference or a cross-domain release-gate link.

The Phase 4 UI mapping does not close these 71 gaps. Phase 5 must add explicit, testable acceptance links or a documented cross-cutting rationale. No acceptance IDs, numeric thresholds or passing results are invented in this report.

## 4. Cross-document consistency findings

The following governing constraints are consistent across the current Phase 4 proposal set:

- **Layer focus vs mapping scope:** changing UI focus must not retarget an explicit L1–L4 controller mapping.
- **A–H engine bank:** eight logical slots are proposed for Granular and Sampler; the assignment matrix remains an owner-review item and must be validated against the authoritative R1 parameter inventories. Parameter IDs/scaling remain Phase 3 gates.
- **Sampler scope:** Loop Crossfade (GS-SMP-006) is L2 and excluded from R1 A–H assignments. Loop Start/End remain waveform-flag controls.
- **Granular Density:** derived from Rate × Size; not treated as an independent base encoder parameter.
- **FX:** no universal Wet/Dry or generic parameter pair is assumed; targets must follow each R1 effect's own parameter inventory.
- **Macros:** ten macro base values are scoped to a layer Patch under DEC-031.
- **Program Change:** only explicit assignments in the active Performance Bank recall a Preset; unmapped values are ignored under DEC-033. Performance Set remains a separate ordered Preset/Pattern list.
- **Freeze/Panic/recording:** Freeze, Thaw, Panic, Mute, transport Stop and recording Stop remain distinct. Freeze does not stop independent recording under DEC-042.
- **Sequencer:** the fixed 16-step strip is a viewport, not a Pattern-length limit; the detailed editor supports the proposed variable-length Pattern workflow.
- **Recording:** Layer L1–L4 and Stereo Master are distinct targets. Arm/Start/Stop is a proposal; simultaneous multi-target capture is not assumed.

## 5. Phase 4 deliverable status

| Deliverable | Documentation status | Remaining gate |
|---|---|---|
| 124-ID UI traceability table | Complete; inventory reconciled | Phase 5 acceptance links/rationale |
| Six workspace layouts | Proposed | Owner review; display/hardware dimensions later |
| Controller action/target catalogue | Proposed | Owner review; technical MIDI implementation later |
| Granular/Sampler A–H assignments | Proposed | Owner approval and R1 parameter-inventory validation |
| Sequencer viewport and editing flow | Proposed | Owner review; Phase 5 interaction tests |
| Chord Record workflow | Proposed | Owner review; GS-CHD-004 semantics |
| Layer/Master recording workflow | Proposed | Owner review; concurrency/resource policy |
| Performance Bank and Set separation | Proposed | Owner review of editor and active-bank persistence |
| Accessibility/performance audit | Reconciled at proposal level | Owner sign-off and actual controller-only tests |

## 6. Remaining owner decisions

1. Approve or revise Granular and Sampler A–H mappings.
2. Review Chord Record Arm/Capture/Commit/Cancel and the unresolved note-count/duplicate behavior.
3. Review recording target selection, status presentation and concurrency policy.
4. Review Performance Bank editor and active-bank selection/persistence.
5. Review the global frame, persistent performance row and consolidated workspace layouts.
6. Confirm the final set of controls that must be immediately reachable during live performance.

These are explicit review gates, not silently resolved design assumptions.

## 7. Phase 5 test-design priorities

A companion blueprint, `PHASE5_R1_ACCEPTANCE_TEST_DESIGN.md`, has now been added. It assigns all 71 unlinked requirements to shared test families, proposes observable oracles, fixture sequences, evidence artifacts and explicit dependency/measurement gates. The canonical backlog remains the per-requirement intent/evidence/dependency register. Neither document claims formal AC approval or executed tests.

1. Convert each essential live operation into a controller-only test case: setup, target scope, input event, expected state transition, visible confirmation and failure recovery.
2. Add acceptance coverage or cross-cutting rationale for the 71 requirements identified by Phase 3.
3. Test per-layer target isolation, including mappings whose target layer differs from current UI focus.
4. Test Program Change mapped/unmapped behavior and active Performance Bank selection.
5. Test Freeze/Panic/transport/recording independence, including DEC-042 persistence and failure states.
6. Test variable Pattern lengths, paging/follow behavior, up to eight events per step, event editing, pending bar-end changes, Save/Draft/Undo/Redo.
7. Test soft takeover, mapping conflicts, Learn cancellation, reset/unmap, device disconnect/reconnect and mapping recovery.
8. Validate the A–H parameter inventory, discrete vs continuous action behavior, value scaling and display units.
9. Add host/format-specific tests only after the relevant Phase 3 decisions and host prototypes are authorized.

No tests are claimed executed by this report.

## 8. Final disposition

- [x] R1 requirement IDs reconciled: 124/124.
- [x] Phase 4 proposal documents cross-checked for the listed governing constraints.
- [x] Stale discovery-gap language reconciled in the UI traceability audit.
- [ ] Owner UX sign-off.
- [ ] Phase 5 acceptance/test-design closure for the 71 remaining requirement gaps.
- [ ] Implementation and host validation.

**Conclusion:** Phase 4 documentation has reached an owner-reviewable state. It has not reached owner approval, implementation readiness or test-proven completion. PR #9 remains Draft and must not be merged as part of this work.

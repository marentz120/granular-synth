# Live Performance Specification

**Status:** Product behavior baseline approved under DEC-015…DEC-021 and the owner's explicit delegation to ChatGPT as product manager. UI layouts, mapping UX, DSP architecture, numeric acceptance thresholds and implementation remain later-phase work; this document does not authorize implementation.

**Version:** 0.3 (consistency fixes, 2026-11-27: §0 malformed table row repaired; §9 criterion 2 no longer contradicts §6's R1 feedback exclusion; §1 adds Freeze, GS-SMG-021)
**Inputs:** CLAUDE.md, DECISIONS_LOG.md (DEC-015, DEC-016), OPEN_QUESTIONS.md (Q-P05, Q-P06, Q-P12, Q-T01, Q-T12), GRANULAR_SYNTH_REQUIREMENTS.md (GS-MID-*, GS-UI-*), DSP_ARCHITECTURE.md §4, §9, §12a, GR_MEGA_FEATURE_MAP.md (performance workflows, §11).

---

## 0. What this document does and does not decide

| Decided here (by reference to DECISIONS_LOG.md) | Not decided here — remains open/proposed |
|---|---|
| DEC-015: studio and live performance are both primary | AU/VST3 component types and bus layout (Q-P06, DSP_ARCHITECTURE.md §5.6) |
| DEC-016: standalone essential live operations must be fully MIDI-controller-operable | Framework (Q-T01) |
| DEC-017: four independent layers in R1 | Parameter exposure scope/scheme (Q-T12) |
| DEC-018: Granular + Sampler in R1; Slice deferred | Numeric performance budgets and acceptance thresholds |
| DEC-019: full central sequencer in R1 | Detailed UI and controller-mapping UX |
| DEC-020: live input, Freeze and recording across layers | Legal, naming and distribution decisions |
| DEC-021: generic MIDI controller required, full sequencer editing included; no R1 MIDI feedback or named-controller template | — |

*(Consistency fix, 2026-11-27: the DEC-021 row previously broke the table's two-column structure, and the duplicated "Framework"/"parameter exposure"/"AU-VST3" lines below it repeated content already in the rows above. The line claiming "the specific target MIDI controller or a dedicated hardware template (Q-P12)" is still open was removed — Q-P12 is answered by DEC-021 itself (no named template/feedback in R1); its resolution is recorded in OPEN_QUESTIONS.md and DECISIONS_LOG.md, unaffected by this formatting fix.)*

The owner explicitly delegated resolution of remaining product questions to ChatGPT as product manager on 2026-11-27, provided decisions do not contradict prior owner decisions. DEC-015…DEC-021 record the resulting approved product baseline. Technical and implementation decisions remain gated.

---

## 1. Controller-only performance workflow

GS-MID-008 requires that every **essential live-performance operation** be reachable from a MIDI controller alone, without keyboard or mouse. §8 draws a setup-time/performance-time boundary to make "essential" testable — that boundary exists to keep the term precise, **not** to narrow DEC-016: any operation a performer genuinely needs *during* a set is essential and must be controller-reachable, full stop, regardless of whether it could also be prepared in advance.

R1 essential-operation set (product baseline; detailed acceptance criteria remain Phase 5 work):

| Operation | Source requirement | Notes |
|---|---|---|
| Preset/patch recall | GS-PRS-002, GS-MID-005 (program change) | Needs a stable PC-to-preset mapping; exact scheme is Phase 4/5, not decided here |
| Layer mix level, per-layer position/performance controls | GS-SEQ-009 | Already R1 |
| Play/drone mode per layer, panic (all notes/sound off) | GS-SEQ-008, GS-MID-006 | Already R1 |
| Macro control (up to 10 macros) | GS-MOD-008 | Already R1; macros are the primary "performance surface" by design |
| Core 8-knob engine section per layer | GS-UI-002 | Already R1; benchmark-parity knob layout is itself controller-friendly |
| Scan/position, pitch | GS-GRN-004, GS-GRN-009, GS-GRN-010 | Primary "performance" grain controls per the benchmark's own workflow (feature map §11) |
| Chord hold/record | GS-CHD-004 | Already R1 |
| Sequencer transport, pattern selection/chain, step/event editing, event/layer assignment | GS-SEQ-001…012 | All sequencer operations are essential and controller-accessible; physical ergonomics depend on the controller |
| Fill intensity/toggle and layer/event Mute | GS-SEQ-011 | Fill is latched and continuous; Mute is immediate |
| Pattern save/draft/undo/redo actions | GS-SEQ-012 | Must be reachable without keyboard/mouse during performance when needed |
| Freeze: per-layer and global | GS-SMG-021 | Independently switchable per-layer Freeze plus a global Freeze (DEC-020), controller-reachable under GS-MID-008. Freeze semantics are a Phase 3 architecture decision and are not specified here |
| MIDI Learn entry/exit and mapping recall | GS-MID-003, GS-UI-007 (new, this document) | The mechanism itself must be controller-reachable or at minimum not *require* mouse/keyboard mid-set once mappings exist |

This is the approved product-level operation set. Phase 4 defines the UI and mapping workflows; Phase 5 defines pass/fail acceptance tests for every operation. Any action genuinely needed mid-set remains controller-reachable under DEC-016, even if it can also be prepared in advance.

---

## 2. MIDI mapping architecture

R1 product requirements: generic class-compliant MIDI controller support; MIDI Learn plus manual CC, channel and Note assignment; all sequencer operations including editing are controller-accessible. Detailed mapping architecture below is a design direction for Phase 3/4, consistent with DSP_ARCHITECTURE.md §9 and §12a:

- Every mappable control is a registered parameter (stable string ID, per the existing registry — Q-T12 still decides *how many* parameters are exposed, not whether mapping exists).
- A **mapping** is a record `{controller-event-source (channel + CC/NRPN/note), target parameter ID, scaling/curve, soft-takeover flag}`. This mirrors the existing MIDI Learn requirement (GS-MID-003: "14-bit CC and NRPN input; soft-takeover (pickup) option") — no new MIDI data model is introduced, only a place to store many such bindings and group them.
- Mappings are **structural state** (§4.2 of DSP_ARCHITECTURE.md): built/edited on the Message/UI thread, applied to the audio thread via the existing command-queue mechanism. No new real-time path is required.
- Mapping sets are serialized as part of the existing state/preset model (GS-PRS-001) — see §4 below for scope (global vs. per-preset).

Open and explicitly **not** decided by this document: the exact wire format of a mapping record, how many simultaneous mappings are supported, and whether a specific third-party controller gets first-class auto-mapping (Q-P12).

---

## 3. MIDI Learn and editable mappings

- **Learn mode:** enter a learn state (console/UI gesture), move a target control (software or touch), then move the physical controller — binds the two. This already exists as a requirement (GS-MID-003); this document adds that the *entry into* and *confirmation of* learn mode should itself not strictly require mouse/keyboard where feasible (e.g., a dedicated learn gesture reachable from a controller), but does not mandate a specific implementation.
- **Editable:** every mapping must be viewable and deletable/reassignable after creation — a mapping list/editor is implied by GS-UI-007 (new) but its exact UI (list view, matrix, etc.) is Phase 4 work.
- **Soft takeover:** carried over unchanged from GS-MID-003 — a physical control must not cause a jump when a mapped parameter's current value and the controller's physical position disagree, until they cross.

---

## 4. Global and preset-specific mappings

Two mapping scopes are used for the four-layer R1 instrument (DEC-017). The mapping architecture addresses the active layer and layer index without hard-coding controller layouts; it must cover all four layers.

| Scope | Applies to | Typical use |
| --- | --- | --- |
| **Global** | Transport, panic, macro-bank select, preset/patch navigation | Stays constant regardless of which preset/patch is loaded — a performer's physical layout should not need remapping when changing sounds |
| **Preset-specific** | Macros' *destinations*, per-layer performance knobs, chord/hold behavior | Travels with the patch/preset (GS-PRS-001 data model); a mapping can be saved as part of a preset, or left to fall back to the global default |

Product-management design direction for Phase 4/5: preset-specific mapping overrides global mapping for the same physical control; otherwise global applies. Confirm conflict and recall workflows during UI design.

---

## 5. Mapping conflicts and recovery

- **Conflict definition:** two mappings bound to the same physical controller-event source (same channel + CC/note number) within the same active scope (global, or the same preset).
- **Detection:** on creating/importing a mapping that collides with an existing one in the same scope, the system must surface the conflict rather than silently overwrite — exact UX (replace / keep both / cancel) is Phase 4 work.
- **Cross-scope conflicts** (a global mapping and a preset mapping on the same physical control) are not errors — per §4's precedence rule, the preset mapping simply takes priority while that preset is active.
- **Recovery:** a "reset mappings to default" action (per scope) and an explicit "unmap" action must exist, so a performer is never permanently stuck with an unwanted or broken binding. A corrupted or missing mapping (e.g., referencing a parameter ID that no longer exists) must degrade to "unmapped," consistent with the existing crash-resilience requirement (GS-NFR-012) — never crash or silently reassign to an unrelated parameter.

---

## 6. Controller feedback — deferred beyond R1

- MIDI feedback to motorized faders, LED rings or controller displays is **not an R1 requirement**. It may be considered later; it must not be treated as a prerequisite for R1 operation.
- When supported, feedback keeps the physical controller's visual/haptic state in sync with the current parameter value (e.g., after a preset change moves a mapped value, a motorized fader should follow). This is Message/UI-thread work (DSP_ARCHITECTURE.md §12a) — never audio-thread-driven, never a source of audio-thread blocking.
- No named controller is targeted in R1 (Q-P12 answered by DEC-021). MIDI feedback is deferred; R1 requires controller-to-instrument input, not bidirectional feedback.

---

## 7. Dedicated versus flexible controls

R1 uses generic flexible controls only. A dedicated named-controller template is deferred beyond R1.

- **Flexible (R1, required by GS-MID-008/GS-UI-007):** generic MIDI Learn against *any* class-compliant MIDI controller. This is what actually satisfies DEC-016 for R1 — it requires no assumption about which hardware the performer owns.
- **Dedicated (proposed for later, not R1):** a named-controller template (e.g., a specific control surface's physical layout pre-mapped to the 8-knob engine section and macros) would improve out-of-box experience but is not necessary to satisfy DEC-016. **R1 decision:** dedicated templates are not required; the flexible MIDI Learn/manual mapping system is the supported baseline.

Q-P12 is answered by DEC-021: the generic MIDI Learn/manual mapping system is the R1 baseline; named-controller templates are deferred.

---

## 8. Setup-time versus performance-time operations

This boundary exists only to make GS-MID-008's "essential" testable — **it must never be used to reclassify an essential operation as setup-only.** Governing rule (proposed): an operation belongs in the setup-time column only if **both** (a) a performer can and normally would complete it before the set starts, **and** (b) nothing about it needs to be repeated, adjusted, or accessed again once the set is underway. If any realistic live-performance use case needs an operation *during* the set, it is performance-time/essential under GS-MID-008 — even though it could also, in principle, be done in advance. Being doable in advance is not sufficient grounds to classify something as setup-only.

| Setup-time (keyboard/mouse acceptable) | Performance-time (must be controller-reachable, GS-MID-008) |
|---|---|
| Sample import, recording-buffer configuration, slice editing | Recall of an already-prepared preset/patch |
| Detailed patch editing (modulation matrix rows, FX chain construction, envelope curve shaping) | Macro control, scan/pitch, layer mix, play/drone, panic, chord hold (§1 table) |
| Creating a **new** MIDI mapping (learn mode entry may be controller-reachable, but building/editing the mapping list is expected to use the UI) | **Using** an already-created mapping — including recalling a different saved mapping set, which GS-MID-008 requires to be controller-reachable if a performer needs it mid-set |
| Initial controller selection/configuration in the standalone app | — |

This boundary is a **recommendation**, meant to make "essential" testable; per the governing rule above, it clarifies GS-MID-008's scope but must not be read as narrowing it. Phase 4/5 should turn it into the authoritative scope list and attach acceptance tests, applying the same governing rule.

---

## 9. Performance acceptance criteria (product scope approved; tests not yet finalized)

Consistent with the gap already noted in GRANULAR_SYNTH_REQUIREMENTS.md §8 (acceptance criteria are a Phase 5 deliverable) and DEVELOPMENT_ROADMAP.md Phase 5, these are **proposed** criteria, to be finalized alongside other R1 acceptance tests, not approved here:

1. Every operation in the §1 essential-operation table is reachable from a generic class-compliant MIDI controller with zero required keyboard/mouse input, verified by a scripted/manual test pass per operation.
2. No audio dropout or added callback latency when a mapping is created or edited (ties into GS-NFR-002/GS-NFR-003 — no new NFR is introduced, this just states the existing ones apply here). Controller feedback is deferred beyond R1 (§6, DEC-021) and is explicitly out of scope for this criterion; if a later release adds feedback, the same no-dropout/no-added-latency bar applies to it then.
3. Soft-takeover behaves correctly (no value jump) across at least one deliberately-mismatched controller/parameter state per mapped control type.
4. A conflicting mapping is detected and surfaced, never silently overwritten, in 100% of tested conflict scenarios (§5).
5. Panic (all notes/sound off) is reachable and effective within one control gesture at all times, including mid-performance error states.

Numeric thresholds (if any are needed beyond pass/fail) are deferred to Phase 5, per existing process.

---

## 10. Sequencer decision — R1 (approved; supersedes the former L2 recommendation)

The former recommendation to defer the full sequencer to L2 is **superseded by DEC-019**. The owner chose a full central step sequencer for R1 after considering the standalone live-performance requirement.

The authoritative feature-level behavior is in docs/R1_PRODUCT_SPECIFICATION.md §4. Summary:

- One central sequencer controls the four layers.
- TB-3-inspired direct step-based workflow; original GRANULAR SYNTH interaction design.
- Variable pattern length and step/edit resolution; no arbitrary product-level maximum.
- Up to eight simultaneous events per step, freely distributed across the four layers.
- Events can trigger Grains and control layer/granular parameters independently.
- Patterns can be saved, recalled, switched and chained; switching during playback occurs at bar end.
- Internal BPM, MIDI Clock, Ableton Link and DAW/host transport synchronization.
- Quantization, triplets, Swing, Humanize and Microtiming.
- Automatic latched granular Fill across all active layers, with continuous intensity control; Fill changes granular character, not the rhythmic structure.
- Immediate Mute for layers and individual steps/events, with selectable unmute behavior for an individual event.
- Independent Pattern Library plus Pattern data stored with Presets; changing Preset does not replace the active Pattern.
- Ordered Performance Sets.
- Live editing with bar-end application, explicit Save, temporary Draft on Pattern switch, full Undo/Redo and no persistent version archive.
- All sequencer operations, including editing, are MIDI-controller accessible. Generic MIDI controllers are supported; each controller's physical capabilities determine the available ergonomics.

This is a product requirement, not an architecture choice. Sequencer data structures, scheduling, CPU limits, parameter ownership and thread boundaries are Phase 3 work. Acceptance tests and numerical thresholds are Phase 5 work.

## 11. Remaining questions and later gates

The product scope questions Q-P04, Q-P05, Q-P06, Q-P12 and Q-P20 are answered by DEC-017…DEC-021. Remaining open questions concern:
- AU/VST3 component and bus implementation for live input (Phase 3).
- Framework, parameter IDs/host exposure, sample and preset persistence, thread model and resource budgets (Phase 3).
- Detailed UI workflows, MIDI mapping editor and controller-only acceptance workflow (Phase 4/5).
- Numeric performance and acceptance thresholds (Phase 5).
- Legal/IP, naming, distribution and licensing questions (pre-release).


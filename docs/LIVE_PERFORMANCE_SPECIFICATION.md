# Live Performance Specification

**Status:** Draft. New document, created in direct response to two owner decisions recorded on PR #2 (2026-11-27): **DEC-015** (Q-P19 resolved — both studio sound design and live performance are primary use cases) and **DEC-016** (controller-only live operability is a core R1 requirement). Those two decisions, and the requirements they produce (GS-MID-008, GS-UI-007 in GRANULAR_SYNTH_REQUIREMENTS.md), are the only things this document treats as approved. Everything else below — workflow details, mapping architecture, acceptance-criteria numbers, and in particular the sequencer reassessment in §10 — is a **proposal/recommendation for ChatGPT and owner review**, not a decision. This document does not start Phase 4 (UI/UX) and does not authorize any implementation.

**Version:** 0.1
**Inputs:** CLAUDE.md, DECISIONS_LOG.md (DEC-015, DEC-016), OPEN_QUESTIONS.md (Q-P05, Q-P06, Q-P12, Q-T01, Q-T12), GRANULAR_SYNTH_REQUIREMENTS.md (GS-MID-*, GS-UI-*), DSP_ARCHITECTURE.md §4, §9, §12a, GR_MEGA_FEATURE_MAP.md (performance workflows, §11).

---

## 0. What this document does and does not decide

| Decided here (by reference to DECISIONS_LOG.md) | Not decided here — remains open/proposed |
|---|---|
| DEC-015: studio and live performance are both primary | Layer count (Q-P04) |
| DEC-016: standalone essential live operations must be fully MIDI-controller-operable | Engine scope / first-release engines (Q-P20) |
| | Step sequencer release tier (Q-P05) — §10 gives a recommendation, not an approval |
| | Live audio input in plugins (Q-P06) |
| | AU/VST3 component types (Q-P06, DSP_ARCHITECTURE.md §5.6) |
| | Framework (Q-T01) |
| | Parameter exposure scope/scheme (Q-T12) |
| | The specific target MIDI controller or a dedicated hardware template (Q-P12) |

Per CLAUDE.md governance: the human owner is the only source of product-decision approval; this document (Claude Chat/Code deliverable) prepares analysis and recommendations only.

---

## 1. Controller-only performance workflow

GS-MID-008 requires that every **essential live-performance operation** be reachable from a MIDI controller alone, without keyboard or mouse. §8 draws a setup-time/performance-time boundary to make "essential" testable — that boundary exists to keep the term precise, **not** to narrow DEC-016: any operation a performer genuinely needs *during* a set is essential and must be controller-reachable, full stop, regardless of whether it could also be prepared in advance.

Proposed essential-operation set for R1 (derived from GRANULAR_SYNTH_REQUIREMENTS.md §3, tiers unchanged by this document):

| Operation | Source requirement | Notes |
|---|---|---|
| Preset/patch recall | GS-PRS-002, GS-MID-005 (program change) | Needs a stable PC-to-preset mapping; exact scheme is Phase 4/5, not decided here |
| Layer mix level, per-layer position/performance controls | GS-SEQ-009 | Already R1 |
| Play/drone mode per layer, panic (all notes/sound off) | GS-SEQ-008, GS-MID-006 | Already R1 |
| Macro control (up to 10 macros) | GS-MOD-008 | Already R1; macros are the primary "performance surface" by design |
| Core 8-knob engine section per layer | GS-UI-002 | Already R1; benchmark-parity knob layout is itself controller-friendly |
| Scan/position, pitch | GS-GRN-004, GS-GRN-009, GS-GRN-010 | Primary "performance" grain controls per the benchmark's own workflow (feature map §11) |
| Chord hold/record | GS-CHD-004 | Already R1 |
| MIDI Learn entry/exit and mapping recall | GS-MID-003, GS-UI-007 (new, this document) | The mechanism itself must be controller-reachable or at minimum not *require* mouse/keyboard mid-set once mappings exist |

This table is a **working proposal**, not a final acceptance list — it is an input to Phase 4/5, where GRANULAR_SYNTH_REQUIREMENTS.md §8 traceability already schedules acceptance criteria. It does not add new tiers or reclassify any requirement; every row already cites an existing R1 requirement.

---

## 2. MIDI mapping architecture

Proposed, consistent with DSP_ARCHITECTURE.md §9 (parameter registry) and §12a (added in this revision):

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

Two mapping scopes are proposed, both needed because the instrument is multitimbral (some number of layers *N*; **N is not decided by this document** — layer count is Q-P04, still OPEN, and DEC-008's "4 layers" is itself still Proposed, not an architectural constraint). The mapping architecture below must hold for any approved *N*: it addresses "the currently active layer" / "layer index" generically, never a fixed count, so a future change to *N* does not require reworking the mapping model.

| Scope | Applies to | Typical use |
| --- | --- | --- |
| **Global** | Transport, panic, macro-bank select, preset/patch navigation | Stays constant regardless of which preset/patch is loaded — a performer's physical layout should not need remapping when changing sounds |
| **Preset-specific** | Macros' *destinations*, per-layer performance knobs, chord/hold behavior | Travels with the patch/preset (GS-PRS-001 data model); a mapping can be saved as part of a preset, or left to fall back to the global default |

Precedence proposal: preset-specific mapping overrides global mapping for the same physical control; otherwise global applies. This is a recommendation for Phase 4/5, not an approval.

---

## 5. Mapping conflicts and recovery

- **Conflict definition:** two mappings bound to the same physical controller-event source (same channel + CC/note number) within the same active scope (global, or the same preset).
- **Detection:** on creating/importing a mapping that collides with an existing one in the same scope, the system must surface the conflict rather than silently overwrite — exact UX (replace / keep both / cancel) is Phase 4 work.
- **Cross-scope conflicts** (a global mapping and a preset mapping on the same physical control) are not errors — per §4's precedence rule, the preset mapping simply takes priority while that preset is active.
- **Recovery:** a "reset mappings to default" action (per scope) and an explicit "unmap" action must exist, so a performer is never permanently stuck with an unwanted or broken binding. A corrupted or missing mapping (e.g., referencing a parameter ID that no longer exists) must degrade to "unmapped," consistent with the existing crash-resilience requirement (GS-NFR-012) — never crash or silently reassign to an unrelated parameter.

---

## 6. Controller feedback where supported

- Scope: **optional**, gated on controller capability — not all MIDI controllers accept feedback (motorized faders, LED rings/displays via MIDI out).
- When supported, feedback keeps the physical controller's visual/haptic state in sync with the current parameter value (e.g., after a preset change moves a mapped value, a motorized fader should follow). This is Message/UI-thread work (DSP_ARCHITECTURE.md §12a) — never audio-thread-driven, never a source of audio-thread blocking.
- No specific controller or feedback protocol is targeted by this document (Q-P12 remains open). The requirement is architectural readiness (bidirectional MIDI I/O in the standalone shell — DSP_ARCHITECTURE.md §12a), not a committed feature list.

---

## 7. Dedicated versus flexible controls

Two complementary approaches, not a forced choice between them:

- **Flexible (R1, required by GS-MID-008/GS-UI-007):** generic MIDI Learn against *any* class-compliant MIDI controller. This is what actually satisfies DEC-016 for R1 — it requires no assumption about which hardware the performer owns.
- **Dedicated (proposed for later, not R1):** a named-controller template (e.g., a specific control surface's physical layout pre-mapped to the 8-knob engine section and macros) would improve out-of-box experience but requires committing to a specific controller (Q-P12, OPEN) and is not necessary to satisfy DEC-016. **Recommendation:** treat dedicated templates as an L2+ enhancement layered on top of the flexible system, once a target controller is chosen; do not block R1 on it.

This recommendation does not decide Q-P12 — it only argues that Q-P12's resolution is not a prerequisite for satisfying DEC-016.

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

## 9. Performance acceptance criteria (proposed; not numerically finalized)

Consistent with the gap already noted in GRANULAR_SYNTH_REQUIREMENTS.md §8 (acceptance criteria are a Phase 5 deliverable) and DEVELOPMENT_ROADMAP.md Phase 5, these are **proposed** criteria, to be finalized alongside other R1 acceptance tests, not approved here:

1. Every operation in the §1 essential-operation table is reachable from a generic class-compliant MIDI controller with zero required keyboard/mouse input, verified by a scripted/manual test pass per operation.
2. No audio dropout or added callback latency when a mapping is created, edited, or triggers controller feedback (ties into GS-NFR-002/GS-NFR-003 — no new NFR is introduced, this just states the existing ones apply here).
3. Soft-takeover behaves correctly (no value jump) across at least one deliberately-mismatched controller/parameter state per mapped control type.
4. A conflicting mapping is detected and surfaced, never silently overwritten, in 100% of tested conflict scenarios (§5).
5. Panic (all notes/sound off) is reachable and effective within one control gesture at all times, including mid-performance error states.

Numeric thresholds (if any are needed beyond pass/fail) are deferred to Phase 5, per existing process.

---

## 10. Sequencer reassessment (recommendation only — Q-P05 is **not** approved here)

DEC-009 ("step sequencer in L2, not R1") was reasoned primarily from "DAW hosts sequence already" — an argument that assumes a studio/DAW context. DEC-015 confirms live performance is equally primary, and a solo live performer running the **standalone** app has no DAW host to fall back on for sequencing.

**Analysis (not a decision):**

- Arguments for keeping the sequencer in L2 (unchanged):
  - The benchmark's sequencer is deep (64 steps, 7 notes/step, 4 mod lanes, 16 p-lock lanes, ties — GR_MEGA_FEATURE_MAP.md) and would be a large, self-contained R1 cost (DEVELOPMENT_ROADMAP.md milestone sizing already treats it as L2-sized work).
  - R1 already ships per-layer drone/play mode, host-synced LFOs, and (with this document) full controller-only performance control of macros/scan/pitch/layer mix — a meaningful live performance is possible without a step sequencer.
  - Scope discipline: CLAUDE.md's phase gating explicitly warns against building everything in parallel; adding sequencer work to R1 now would expand scope that Phase 2 has not yet frozen.
- Arguments for reconsidering (new, from DEC-015):
  - A live performer without a DAW and without a step sequencer has no on-board pattern/groove tool at all in R1 — for live-performance-primary use this is a real capability gap, not just a "nice to have."
  — A **much smaller** performance-oriented tool (e.g., simple pattern/mute recall, not the full 64-step/p-lock system) could close much of this gap at a fraction of the full sequencer's cost, without pulling the whole L2 sequencer forward.

**Recommendation (for ChatGPT/owner review, not approved):** keep the full step sequencer in L2 as before (DEC-009 unchanged), but flag for Phase 2 scoping discussion whether a minimal performance pattern tool should be added to the R1 definition as a *separate, smaller* requirement — distinct from GS-SEQ-001 and the rest of the L2 sequencer block. This document does not create that requirement or change any tier; it only asks that Phase 2 consider it explicitly in light of DEC-015.

---

## 11. Summary of what remains open

Nothing in this document resolves Q-P04, Q-P05, Q-P06, Q-P12, Q-T01, or Q-T12 — see OPEN_QUESTIONS.md for their current (unchanged, except for the cross-references added in this revision) status. The only approvals this document relies on are DEC-015 and DEC-016, both already recorded in DECISIONS_LOG.md before this document was written.

*End of LIVE_PERFORMANCE_SPECIFICATION.md*

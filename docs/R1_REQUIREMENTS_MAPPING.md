# R1 Requirements Mapping — Initial Pass

**Status:** Initial / preliminary mapping document (as requested). Read-only analysis — **no approved requirement, decision status, or scope was changed to produce this document.** It is not itself an approved artifact; findings below are observations for ChatGPT/owner review, not decisions.
**Base commit:** `main` @ `908b06a` ("Phase 1: approved R1 product scope"), pulled fresh for this analysis.
**Method:** Every requirement in `docs/GRANULAR_SYNTH_REQUIREMENTS.md` and the relevant sections of `docs/DSP_ARCHITECTURE.md`, `docs/LIVE_PERFORMANCE_SPECIFICATION.md`, `docs/PRODUCT_VISION.md`, `docs/DEVELOPMENT_ROADMAP.md`, `docs/DECISIONS_LOG.md` and `docs/OPEN_QUESTIONS.md` was checked against `docs/R1_PRODUCT_SPECIFICATION.md` (the authoritative R1 product baseline, v1.0) and against DEC-015…DEC-021.

---

## 1. What changed since the last review (context)

Since the last documentation revision (commit `9660277`), `main` advanced 25 commits. The owner delegated remaining open product questions to **ChatGPT acting as product manager** (constraint: no delegated decision may contradict an already-approved owner decision), resulting in:

- **DEC-017** — exactly four independent layers in R1, each selectable as Granular or Sampler.
- **DEC-018** — R1 engines are Granular + Sampler; Granular Slice deferred; Tape (L2), Spectral (L3) unchanged. Supersedes DEC-008.
- **DEC-019** — a full central step sequencer (TB-3-inspired) is required in R1, not L2. Supersedes DEC-009.
- **DEC-020** — live input, Freeze and recording (per-layer + stereo master-mix) are R1 capabilities. Supersedes DEC-010.
- **DEC-021** — generic MIDI-controller operation, including full sequencer editing, is R1; **no MIDI feedback and no named-controller template in R1.**
- A new authoritative file, `docs/R1_PRODUCT_SPECIFICATION.md` (v1.0), consolidates all of this.
- `docs/OPEN_QUESTIONS.md` → v0.4: Q-P04, Q-P05, Q-P06, Q-P12, Q-P20 all moved to **ANSWERED**.
- `docs/GRANULAR_SYNTH_REQUIREMENTS.md` → v0.4, `docs/DSP_ARCHITECTURE.md`, `docs/LIVE_PERFORMANCE_SPECIFICATION.md` (→ v0.2), `docs/PRODUCT_VISION.md`, `docs/DEVELOPMENT_ROADMAP.md` were all largely reconciled to this new baseline — but not completely, as §4 below details.

This is a legitimate, substantial advance of the project since this session last touched it. The findings below are about **residual sync gaps** left by that reconciliation, not about the legitimacy of the decisions themselves.

---

## 2. Approved decisions vs. proposals vs. open questions

| Decision | Status | Note |
|---|---|---|
| DEC-001 GR-MEGA as benchmark | Approved | Unchanged |
| DEC-002 Repository as record | Approved | Unchanged |
| DEC-003 Research before implementation | Approved | Unchanged |
| DEC-004 Standalone + AU + VST3 | **Proposed** | Still open — format confirmation is Phase 3 (Q-P02) |
| DEC-005 JUCE/C++ | **Proposed** | Still open (Q-T01) |
| DEC-006 First-release engines/layer limits | Approved | Resolved via DEC-017/018 |
| DEC-007 `[RN]` supplemental tag | Proposed | Editorial convention, not a product decision |
| DEC-008 Granular+Slice+Sampler, 4 layers | Proposed (stale — see §4.1) | Content superseded by DEC-018 but status not updated |
| DEC-009 Sequencer in L2 | **Superseded** | By DEC-019 |
| DEC-010 Plugin live input | **Superseded** | By DEC-020 |
| DEC-011…014 | Proposed | Unchanged, Phase 3 technical items |
| DEC-015 Studio + live both primary | Approved | — |
| DEC-016 Controller-only operability | Approved | Refined by DEC-021 |
| DEC-017 Four layers, Granular/Sampler per layer | **Approved** | New |
| DEC-018 Granular+Sampler R1; Slice deferred | **Approved** | New |
| DEC-019 Full central sequencer in R1 | **Approved** | New |
| DEC-020 Live input/Freeze/recording | **Approved** | New |
| DEC-021 Generic MIDI controller, no feedback/template | **Approved** | New |

**Open questions (§B technical, §C evidence, §F legal) are unchanged** — framework (Q-T01), parameter exposure (Q-T12), sample-rate/CPU/memory budgets, AU/VST3 component types, and all legal/IP questions (Q-L01–L07) remain exactly as open as before. The product-manager delegation explicitly did not touch these. **This mapping does not propose answering any of them.**

---

## 3. Requirement coverage against R1_PRODUCT_SPECIFICATION.md

| R1_PRODUCT_SPECIFICATION.md section | Covered by | Gap? |
|---|---|---|
| §2.1 Four layers, per-layer Granular/Sampler selection | GS-ARC-001 (count only — see §4.2), GS-ARC-005 (engine selection) | Partial — §4.2 |
| §2.2 Engines: Granular + Sampler, Slice deferred | GS-ARC-005, §3.5 header | OK, but see §4.3 (DSP_ARCHITECTURE.md still says Slice=R1) |
| §3 Live input per layer | GS-SMG-012 | OK |
| §3 Freeze (per-layer + global) | **None** | **Missing — see §5.1** |
| §3 Recording (per-layer + master-mix) | GS-SMG-020, GS-SMG-008/009/010/011 | OK |
| §4.1 Central sequencer, 8 events/step, layer routing | GS-SEQ-001 | OK |
| §4.2 Timing/sync (BPM, MIDI Clock, Ableton Link, DAW) | GS-SEQ-005, GS-MID-007 | OK, but overlapping — see §6.1 |
| §4.3 Fill, Mute | GS-SEQ-011 | OK |
| §4.4 Patterns/Library/Performance Sets | GS-SEQ-004 | OK |
| §4.5 Editing/Draft/Undo/Save | GS-SEQ-012 | OK |
| §5 Controller-only operation, sequencer editing included | GS-MID-008, LIVE_PERFORMANCE_SPECIFICATION.md §1 | OK, but §1's table doesn't explicitly list sequencer editing or Freeze — see §4.4 |
| §5 No MIDI feedback/template in R1 | — | **Contradicted elsewhere — see §4.1** |
| §6 Delegated PM decisions (1–5) | Distributed across GS-SMG-020, GS-SEQ-005/011, LIVE_PERFORMANCE_SPECIFICATION.md §7 | OK |
| §7 Explicitly not decided | Matches OPEN_QUESTIONS.md §B/§C/§F | OK |

---

## 4. Contradictions found

### 4.1 GS-UI-007 still requires MIDI feedback, which DEC-021 excludes from R1 (highest priority)

- `docs/GRANULAR_SYNTH_REQUIREMENTS.md` §3.16, **GS-UI-007** (an **Approved** R1 requirement) reads: *"...global and per-preset mappings, conflict detection and recovery, **and controller feedback where the connected controller supports it** — sufficient to satisfy GS-MID-008."*
- `docs/DECISIONS_LOG.md` **DEC-021** (Approved) reads: *"...no R1 MIDI feedback or named-controller template."*
- `docs/LIVE_PERFORMANCE_SPECIFICATION.md` §6 is titled *"Controller feedback — deferred beyond R1"* and says feedback *"is not an R1 requirement."*
- `docs/DSP_ARCHITECTURE.md` §12a (line 414) says *"Controller feedback: not required in R1 (DEC-021)."*
- DECISIONS_LOG's own note on DEC-016 already flags this: *"Subsequent DEC-021 ... excludes named templates/MIDI feedback from R1"* — but **GS-UI-007's actual requirement text was never edited to remove the feedback clause.**

**Effect:** an Approved requirement's literal text still mandates something a later Approved decision explicitly excludes from R1. Anyone implementing strictly from GS-UI-007 would build R1 feedback support; anyone implementing from DEC-021/LIVE_PERFORMANCE_SPECIFICATION.md §6 would not.

### 4.2 LIVE_PERFORMANCE_SPECIFICATION.md §9 self-contradicts §6 on feedback

- §6 (as above): feedback deferred beyond R1.
- §9, acceptance criterion 2: *"No audio dropout or added callback latency when a mapping is created, edited, or **triggers controller feedback**..."* — phrased as if feedback is an in-scope, testable R1 behavior.

### 4.3 DSP_ARCHITECTURE.md §7 still labels Granular Slice "(R1)"

- `docs/DSP_ARCHITECTURE.md` §7 engine-comparison table, row **Granular Slice**, is still tagged **"(R1)"**.
- `docs/GRANULAR_SYNTH_REQUIREMENTS.md` §3.5 is titled *"Granular Slice engine (SLC) — deferred from R1"*, with every SLC-* requirement now tier **L2**, per **DEC-018 (Approved)**.
- The rest of DSP_ARCHITECTURE.md *was* updated for DEC-017…021 (e.g. §3 module tree, §11 transport, §12a) — this one table row appears to have been missed.

### 4.4 PRODUCT_VISION.md misstates DEC-017

- `docs/PRODUCT_VISION.md`, "Approved R1 product scope": *"Four independent **Granular** layers."*
- DEC-017 and R1_PRODUCT_SPECIFICATION.md §2.1 both say each of the four layers is **independently selectable as Granular or Sampler** — not that all four are fixed to the Granular engine.
- This is the project's top-level vision document; a reader relying on this line alone would get the R1 layer/engine model wrong.

### 4.5 GS-ARC-001 cites Q-P04 as if still open, inconsistent with the next row (GS-ARC-005)

- `docs/GRANULAR_SYNTH_REQUIREMENTS.md` §3.2, **GS-ARC-001**: *"4 independent layers... **decision Q-P04**"* — no mention of DEC-017, no mention of per-layer Granular/Sampler selection, and cites Q-P04 without noting it is now **ANSWERED**.
- The very next row, **GS-ARC-005**, correctly cites **DEC-018** and describes the Granular/Sampler selection.
- GS-ARC-001 and GS-ARC-005 describe two halves of the same DEC-017 decision (layer count; engine selectability) but only one of them was updated to reflect it.

### 4.6 `docs/LIVE_PERFORMANCE_SPECIFICATION.md` §0 table — malformed row

- The table row for DEC-021 (line 20) has only one cell (`| DEC-021: ... |`) instead of the table's two columns, breaking the Markdown table structure at that point. Minor, but worth a cleanup pass.

---

## 5. Missing requirements

### 5.1 Freeze has no requirement ID

`docs/R1_PRODUCT_SPECIFICATION.md` §3 (line 34) and **DEC-020** (Approved, `DECISIONS_LOG.md` line 27) both explicitly require: *"Freeze is available independently per layer and globally across the instrument."* "Freeze" is mentioned only in passing prose in `GRANULAR_SYNTH_REQUIREMENTS.md` lines 10 and 70 (the v0.4 changelog and the §2 R1-scope summary), in `PRODUCT_VISION.md` line 25 ("Approved R1 product scope" bullet list), and in `LIVE_PERFORMANCE_SPECIFICATION.md` §0's table (line 19, quoting DEC-020) — but **no `GS-*` requirement ID defines what Freeze actually does** (what it freezes — grain position, playback, modulation state; per-layer vs. global toggle semantics; MIDI-controller reachability). Confirmed by searching all of `GRANULAR_SYNTH_REQUIREMENTS.md` §3 (the requirement tables themselves, not the prose) for "Freeze": no match. It is also absent from `LIVE_PERFORMANCE_SPECIFICATION.md` §1's essential-operation table (lines 36–48), even though a live "Freeze" control is squarely a performance-time operation under GS-MID-008/DEC-016.

### 5.2 Sequencer editing and Freeze are not listed in the §1 essential-operations table

`LIVE_PERFORMANCE_SPECIFICATION.md` §1's table (lines 36–48) *does* include most sequencer operations — "Sequencer transport, pattern selection/chain, step/event editing..." (line 45) and "Fill intensity/toggle and layer/event Mute" (line 46) — but Freeze is entirely absent from that table, and it's worth double-checking in Phase 4 that every §4 sequencer sub-behavior in `R1_PRODUCT_SPECIFICATION.md` (e.g., the per-step unmute-timing choice, §4.3 line 64) has a corresponding line.

---

## 6. Duplicates / overlapping requirements

### 6.1 GS-MID-007 and GS-SEQ-005 both specify MIDI Clock input

- **GS-MID-007**: *"MIDI clock in (standalone)."* (R1, DEC-019)
- **GS-SEQ-005**: *"Synchronization to internal BPM, external MIDI Clock, Ableton Link and DAW/host transport."* (R1, DEC-019)

Both are now R1 and both cite DEC-019. GS-SEQ-005 is the broader, more complete statement (it also covers BPM/Link/DAW transport). GS-MID-007 is redundant with part of it. Not a contradiction (they agree), but a candidate for consolidation — e.g. make GS-MID-007 explicitly refine/cross-reference GS-SEQ-005 rather than restate it.

---

## 7. Stale summary table (`GRANULAR_SYNTH_REQUIREMENTS.md` §6)

The §6 "Requirement summary" counts table was **not recomputed** after the tier changes in §3. Spot-checked discrepancies:

| Area | Summary table says | Actual count in §3 |
|---|---|---|
| Slice (SLC) | R1: 4, L2: 2 | R1: **0**, L2: **6** (all of SLC-001…006 are now L2, per §3.5's own "deferred from R1" header) |
| MIDI (MID) | R1: 6, L2: 2 | R1: **7**, L2: **1** (GS-MID-007 moved L2→R1 per DEC-019, but the summary wasn't adjusted) |
| Sequencer (SEQ) | R1: 2, L2: 6, L3: 1, Other: 1 (total 10) | §3.12 now lists **12** SEQ requirements (SEQ-001…012); R1 ≈ 9, L2 = 1 (SEQ-007), L3 = 1 (SEQ-006), Pending = 1 (SEQ-010) |
| **Total (164)** | Likely understated given the above | Not recomputed here — flagging for a full recount, not guessing a new total |

This table should be fully recounted once Phase 2 formally closes the R1 requirement/tier matrix (R1_PRODUCT_SPECIFICATION.md §7 already schedules this as a Phase 2 deliverable).

---

## 8. Recommendations (non-binding proposals only — NOT approved decisions; require ChatGPT/owner review and sign-off before any doc is edited on their basis)

**None of these recommendations are approved. Each is a proposal for a future, separately-authorized correction pass; this document does not implement any of them.**

1. **Recommendation:** edit GS-UI-007's requirement text to remove the controller-feedback clause (or explicitly mark it "deferred beyond R1, tracked for a later release"), to match DEC-021.
2. **Recommendation:** edit LIVE_PERFORMANCE_SPECIFICATION.md §9 criterion 2 to remove the feedback-triggering scenario, or mark it conditional on a future, non-R1 feedback feature.
3. **Recommendation:** change DSP_ARCHITECTURE.md §7's Granular Slice row from "(R1)" to "(L2 — deferred, DEC-018)".
4. **Recommendation:** change PRODUCT_VISION.md's "Four independent Granular layers" to "Four independent layers, each selectable as Granular or Sampler."
5. **Recommendation:** update GS-ARC-001 to cite DEC-017 and note the per-layer engine-selection relationship to GS-ARC-005.
6. **Recommendation:** add a `GS-*` requirement for Freeze (functional definition + proposed tier R1, citing DEC-020), and add it to LIVE_PERFORMANCE_SPECIFICATION.md §1 — itself still a proposal pending review, not a tier approval.
7. **Recommendation:** recount GRANULAR_SYNTH_REQUIREMENTS.md §6 fully against the current §3 body.
8. **Recommendation:** consider consolidating GS-MID-007 into/with GS-SEQ-005.
9. **Recommendation:** fix the malformed table row in LIVE_PERFORMANCE_SPECIFICATION.md §0.

None of the above would change any approved or proposed decision's *substance* if acted on — recommendations 1–5 and 9 are proposed sync/correction fixes to make existing approved text internally consistent; 6–8 are proposed additions/bookkeeping that would not alter any approved scope. All nine remain proposals only until explicitly approved and separately implemented.

*End of R1_REQUIREMENTS_MAPPING.md (initial pass)*

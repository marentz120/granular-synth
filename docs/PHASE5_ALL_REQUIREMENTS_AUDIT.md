# GRANULAR SYNTH — All Requirements Audit

**Version:** 0.3  
**Date:** 2026-11-28  
**Branch:** `phase4/ui-ux-specification`  
**Status:** Documentation audit; owner and technical gates remain open.  
**Scope:** All 168 requirement rows, R1 traceability, acceptance coverage, test design, architecture decisions, domain specifications, UI/UX and implementation readiness.

## 1. Executive result

The requirement inventory is complete and its primary release tiers now reconcile. All 124 R1 requirements have architecture, candidate API-symbol and UI traceability. The acceptance/test layer is not complete: 53 R1 requirements have an existing acceptance or cross-domain release-gate link, while 71 still need an explicit link or a documented cross-cutting rationale. The Phase 5 test-design matrix contains exactly those 71 requirements.

**Overall verdict: not ready for production implementation.** The remaining work is not a missing product inventory; it is closing R1 behavior details, technical decisions, owner approvals, host evidence and pass/fail criteria.

No production code, DSP, UI, host prototype, benchmark or acceptance test was executed or authorized by this audit.

## 2. Requirement inventory and release tiers

The authoritative inventory in `GRANULAR_SYNTH_REQUIREMENTS.md` contains 168 unique requirement IDs, one row per ID.

| Tier | Count | Meaning |
|---|---:|---|
| R1 | 124 | Required for the first release |
| L2 | 27 | Later development wave |
| L3 | 9 | Later development wave |
| OPT | 8 | Optional |
| **Total** | **168** | All rows have one primary tier |

The tier placement is approved by DEC-041. The requirements table now uses a single primary tier per row. Where an R1 row mentions later subfeatures, those subfeatures are explicitly marked as deferred and do not become R1 scope:
- `GS-ARC-005`: Granular and Sampler are R1; Slice, Tape and Spectral are later.
- `GS-SMG-005`: waveform view is R1; spectrogram is L2.
- `GS-MOD-004`: the listed modulation sources are R1; CV and Draw X/Y are not R1.

`GS-MOD-005` is explicitly limited to eligible parameters belonging to R1 features; it does not require all 149 benchmark destinations regardless of release scope.

## 3. R1 traceability results

| Check | Result | Interpretation |
|---|---:|---|
| R1 requirements in canonical inventory | 124 | Baseline |
| Phase 3 architecture mapping | 124/124 | Every R1 ID has a proposed module/interface/validation-family mapping |
| Phase 3 candidate API-symbol mapping | 124/124 | Candidate symbols exist; no approved ABI or implementation |
| Phase 4 UI mapping | 124/124 | Every R1 ID is mapped to a proposed UI surface or internal/non-UI handling |
| Direct acceptance-criteria links | 49 | Product-level draft links |
| Additional cross-domain release-gate links | 4 | `GS-PLT-004`, `GS-NFR-002`, `GS-NFR-006`, `GS-NFR-007` |
| Existing acceptance/release-gate links | 53/124 | A link exists; this is not proof of a complete test |
| Requirements still needing an explicit acceptance link or documented rationale | 71/124 | Phase 5 gap |

The Phase 5 test-design matrix contains exactly 71 unique requirement rows and matches the set of 71 unlinked R1 requirements: no missing row and no extra row. The companion backlog also contains two supplemental MPE reset/lifecycle regression checks for requirements already linked to acceptance criteria; those are supplemental cases, not additional members of the 71-gap count.

**Important distinction:** 124/124 traceability does not mean 124 requirements are fully specified, implemented or tested. Likewise, an acceptance link does not necessarily mean its pass condition is sufficiently precise to execute.

## 4. Findings requiring attention

### 4.1 R1 behavior still needs specification closure

These are not all new product-scope requests. They are unresolved details needed to write unambiguous expected results for existing R1 requirements.

| Open item | Affected R1 requirement(s) | What must be clarified |
|---|---|---|
| Q-M03 | GS-GRN-011 | Ratio-mode behavior when Scan is zero; the proposed “hold last valid rate/size” rule is not approved |
| Q-M04 | GS-GRN-009 | Exact scan-mode/stop/wrap behavior; do not infer the missing mode from the benchmark text |
| Q-M09 | GS-GRN-018, GS-ARC-008 | Grain-count normalization law and the default four-layer headroom oracle |
| Q-M14 / Q-M16 | GS-GRN-014 | Off/On/Legato trigger semantics and the exact meaning of Voice Sync |
| Q-M17 | GS-GRN-015 | State transitions, reset points and recycle behavior for all four spawn modes |
| Q-M18 | GS-VOI-006 | Reconcile the stated 0.1 ms envelope minimum with the still-proposed benchmark ambiguity |
| Q-I07 | GS-VOI-008 | R1 filter-mode inventory and the relationship between LPF/HPF modes, routing, slopes and bypass |
| Q-I08 | GS-GRN-006 | Mathematical definition of the centre-weighted pan-spray distribution and its test tolerance |
| Q-I12 | GS-VOI-010 | Sub-oscillator waveform/default, or an explicit narrowing of the requirement |

The Phase 5 design also identifies broader specification gaps that must be closed before final pass conditions: `GS-GRN-006/008/012/013`, `GS-CHD-003/004`, `GS-VOI-004/006/009`, `GS-SMG-004` and `GS-NFR-004/005/009`. These include distribution curves, window reference behavior, arp ordering, chord-capture overlap, velocity/filter curves, import normalization, latency definition, RNG cross-platform policy and progressive-analysis responsiveness.

The expanded domain-spec gate register now also covers:
- R1 effect parameter ranges/defaults, Delay sync, EQ3 crossover behavior and effect-specific automation (`GS-FX-002/004/006)).
- The exact eligible R1 modulation destination inventory (MTX-05 / `GS-MOD-005)).
- Sampler interpolation modes/default and transition behavior (`GS-SMP-003)).
- Decoder/resampling, format-specific live input, recording-buffer limits and layer/master tap equivalence (`GS-SMG-001/008/009/012/020)).
- MPE zone configuration, host expression delivery and NRPN message handling (`GS-MID-002/003/007/008)).
- Preset serialization, atomic save/recovery, migration and embedded-audio admission (`GS-PRS-004/006`, `GS-NFR-012`).

### 4.2 Benchmark questions explicitly kept out of R1

The following questions have been reconciled against the current R1 product definition so they do not silently expand the first release:
- `Q-M15`: per-layer time-signature emulation is not an R1 requirement; R1 defines pattern length/edit resolution and shared timing grids.
- `Q-I16`: keyboard-position voice-stealing heuristics are not R1; `GS-VOI-001` defines the R1 priority as released, non-sustained, then oldest voices.
- `Q-I17`: DEC-032 and `GS-MOD-004` define the R1 source as a normalized two-axis XY pad; separate benchmark “Pos Y” semantics are not required.
- `Q-I18`: Elektron-style sequencer preset switching is not an R1 requirement; R1 separately defines end-of-bar Pattern switching and states that Preset recall does not replace the active Pattern.

Other open research items for L2/L3/OPT features remain in `OPEN_QUESTIONS.md`. They are not R1 blockers unless a later scope decision promotes them.

### 4.3 Technical decisions and evidence gates

The following are still open or proposed and must receive an explicit disposition before the relevant implementation or acceptance gate:
- **Platform/build:** Q-T01 framework, Q-T02 framework-independent core boundary, Q-T03 minimum macOS and Intel/Apple Silicon policy.
- **Audio/DSP:** Q-T04 sample-rate strategy, Q-T05 sample residency/long-sample policy, Q-T06 window implementation, Q-T07 global grain budget, Q-T08 multithreaded rendering, Q-T09 RT-safety tooling, Q-T13 modulation control rate and Q-T14 interpolation default.
- **Parameters/state:** Q-T12 stable parameter IDs and host exposure; Q-T15 serialization format and state-size policy; Q-T16 audio decoding.
- **Audio compatibility:** Q-T17 supported sample rates, buffer sizes and latency.
- **Hosts/reference environment:** Q-P06 AU/VST3 live-input component/bus implementation, Q-P09 exact host/version certification matrix and Q-P10 reference Mac/performance baseline.
- **Commercial/release:** Q-P03 distribution model, installer, licensing and copy-protection policy must be decided before Phase 6; Q-P01 final name/visual identity and Q-P14 factory content are Phase 8 decisions.
- **Legal/IP:** Q-L01…Q-L07 remain open or proposed as recorded in `OPEN_QUESTIONS.md`. GR-1/GR-MEGA patch import is optional and blocked pending Q-L01; framework and third-party library licensing must be resolved before distribution. These do not expand R1 functionality.
- **Benchmark research:** Q-P13 physical GR-MEGA access is deferred and is not an R1 blocker; Q-I15 firmware roadmap research is also a separate evidence task.
- **Spectral-only later scope:** Q-T10 and Q-T11 concern the L3 Spectral engine and do not block R1 if Spectral remains deferred.

A recommendation in an architecture document is not an approved technical decision. The Phase 3 roadmap requires each Q-T01…Q-T17 item to be approved, rejected or explicitly deferred with a target phase.

### 4.4 Cross-cutting architecture decisions

All six architecture questions remain open:
- **AR-01:** deterministic ordering/precedence when host automation, MIDI, sequencer locks and UI writes target the same parameter at the same sample offset.
- **AR-02:** deterministic voice/grain overload and reclamation policy, including held-voice fallback and grain rejection/fade behavior.
- **AR-03:** AU MPE same-pitch Note Off matching, shared member-channel behavior, sustain and zone reconfiguration.
- **AR-04:** VST3 Note Expression mapping and host delivery for Pressure and Timbre.
- **AR-05:** public AU/VST3 metadata and conversion for DEC-038's 48-semitone per-note range.
- **AR-06:** audible behavior when state restore commits while old notes/voices are active.

These affect R1 implementation and test oracles. Candidate directions and fixture plans exist, but no final policy is selected.

### 4.5 Freeze transaction detail

A cross-document audit found stale statements in the Sampler, Audio I/O, MIDI, live-performance, canonical requirement, historical requirements-mapping and R1 release-scope documents that still described Freeze semantics as undecided in Phase 3. These have been corrected or explicitly marked as historical and now point to DEC-042 and `FREEZE_BEHAVIOR_SPECIFICATION.md`. The product behavior is approved; only implementation, memory admission, crossfade/retirement and host validation remain open.

DEC-042 approves Source Snapshot Freeze behavior. One detail remains for owner confirmation: if a global Freeze request prepares successfully for some layers but fails for others, should the successful layers switch together while failed layers remain unchanged, or should the entire four-layer transaction abort? `AC-AUD-028` is explicitly proposed pending this decision. This does not reopen the approved Freeze semantics.

### 4.6 UI/UX approval

Phase 4 has six workspace layouts, a controller target catalogue, proposed Granular/Sampler A–H banks, sequencer viewport workflows, recording/Freeze controls and controller-only workflows. They remain proposals. Owner review is still required for:
- A–H encoder assignments.
- Chord Record workflow and overlap/duplicate behavior.
- Recording target selection, status and concurrency.
- Performance Bank editor and active-bank persistence.
- Global frame, persistent performance row and screen layouts.
- Final set of controls that must be immediately reachable during live performance.
- `GS-PRS-009` patch-comments behavior and UI; its R1 tier is approved, but the feature details remain a proposed product requirement pending owner review.

The accessibility audit says operations are specified at proposal level; no controller-only workflow has been implemented or proven.

### 4.7 Host and format evidence

Official API documentation and host manuals establish capabilities, not support for this exact product topology. No host prototype has been run and no host is certified. Still unverified:
- AU component strategy and full-band live input.
- VST3 instrument classification and full-band live-input bus type.
- Five stereo outputs, inactive-bus behavior and master fallback.
- Optional bus activation and routing after session save/reopen.
- Embedded recording/Freeze state payload round-trip and host size limits.
- Host-specific MPE and Note Expression delivery.

### 4.8 Acceptance criteria quality

The 53 existing links are Phase 2 product-level draft criteria or cross-domain release gates. They still need Phase 5 conversion into executable cases with:
- pinned fixtures and environment;
- exact stimulus and expected result;
- binary or numeric pass condition;
- named measurement method and approved tolerance where needed;
- evidence artifact and reproducibility data.

Numeric CPU, memory, latency, load-time and responsiveness thresholds are not approved. No tests have been run.

## 5. Repository and authorization state

- The audited branch is documentation-only; the repository tree contains project Markdown documents and no production source/test implementation.
- `CLAUDE.md` now reflects the current Phase 4/5 documentation work and explicitly preserves the no-code/no-benchmark/no-merge boundary.
- PR #9 remains open and Draft. It is not merged.
- This audit does not authorize production DSP, plugin/UI implementation, host prototypes, benchmarks, test execution, merge or release.

## 6. Recommended closure order

The detailed decision-by-decision owner, technical, host, UI/UX, legal and release register is maintained in `PHASE5_DECISION_CLOSURE_REGISTER.md`. It separates product decisions that may fall under the documented product-management delegation from technical, legal and owner-only gates.

1. Close the nine R1 behavior details in §4.1 and reconcile their source questions with the authoritative domain specifications.
2. Resolve AR-01/02 and the owner-only AR-03…AR-06 decisions; decide the mixed Global Freeze transaction behavior.
3. Close the R1 technical gates: framework/core boundary, platform target, parameter IDs, supported audio matrix, resource budget/reference machine, state serialization and host/component strategy.
4. Complete owner review of the Phase 4 UX proposals.
5. Turn all 124 R1 requirements into formal acceptance links or explicit cross-cutting rationales; finish the 71 currently unlinked requirements and tighten the existing 53 criteria.
6. Approve fixtures, test harness, CI plan and numeric thresholds. Only then request explicit authorization for a named implementation milestone.

## 7. Final disposition

| Area | Status |
|---|---|
| 168-row requirement inventory and primary tiers | Reconciled |
| R1 architecture/API/UI traceability | Complete at proposal level |
| R1 acceptance-link coverage | Incomplete: 71 gaps |
| Phase 5 test design | 71-row design prepared; owner/technical review open |
| R1 behavioral specification | Open details remain |
| Phase 3 architecture | Not formally closed |
| Phase 4 UX | Owner sign-off open |
| Host certification / benchmarks | Not run |
| Production implementation | Not started |

**Conclusion:** The project has a complete R1 requirement map, but it is not implementation-ready. The next work is specification and decision closure, not coding. No requirement should be marked implemented or tested until there is code and recorded evidence.

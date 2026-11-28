# Claude Code Implementation Handoff

**Version:** 0.2 — implementation preparation package  
**Status:** Ready for owner review; this document is not authorization to begin production implementation.  
**Audience:** Claude Code (implementation agent), project owner and architecture reviewer.  
**Authority:** Approved product decisions in `docs/DECISIONS_LOG.md`, Phase 2 product specification and requirements; Phase 3 documents are architecture proposals unless explicitly marked approved.

## 1. Purpose and operating model

This handoff converts the current Phase 3 architecture package into a controlled implementation workflow for Claude Code. Claude is the implementer; the project owner approves scope and product decisions; the architecture reviewer checks contracts, traceability and evidence.

**Do not infer implementation authorization from this handoff.** Before code begins, the owner must explicitly authorize the selected milestone and the unresolved decision gates listed in §3. Do not merge a PR without explicit owner approval.

## 2. Mandatory source-of-truth reading order

Claude Code must read these repository files before proposing code:

1. `docs/DECISIONS_LOG.md` — approved decisions and their exact boundaries.
2. `docs/R1_PRODUCT_SPECIFICATION.md` — R1 behavior and scope.
3. `docs/GRANULAR_SYNTH_REQUIREMENTS.md` — 168 requirements and tier assignments.
4. `docs/R1_ACCEPTANCE_CRITERIA.md` — current acceptance draft; see `docs/PHASE5_TEST_DESIGN_BACKLOG.md` for the corrected 71-row traceability backlog.
5. `docs/PHASE3_ARCHITECTURE_COMPLETION_AUDIT.md` — known gaps, corrections and review findings.
6. `docs/PHASE3_ARCHITECTURE_DECISION_REGISTER.md` — open technical decisions.
7. `docs/PHASE3_API_AND_SIGNAL_FLOW_SPECIFICATION.md` and `docs/PHASE3_INTERFACE_CONTRACTS.md` — candidate APIs and cross-domain contracts.
8. `docs/THREADING_AND_OWNERSHIP_SPECIFICATION.md` — realtime boundaries, ownership, queues and lifecycle.
9. `docs/SAMPLE_AND_STATE_LIFECYCLE_SPECIFICATION.md` and `docs/FREEZE_BEHAVIOR_SPECIFICATION.md` — sample, recording, Freeze and persistence behavior.
10. `docs/PARAMETER_ID_SPECIFICATION.md`, `docs/DSP_ARCHITECTURE.md`, `docs/PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md`, `docs/PHASE3_PLATFORM_AND_PLUGIN_RESEARCH.md` — parameter, DSP, host and platform proposals.
11. `docs/PHASE3_ARCHITECTURE_TRACEABILITY.md`, `docs/PHASE3_API_SYMBOL_TRACEABILITY.md`, `docs/PHASE5_TEST_DESIGN_BACKLOG.md` and `docs/DEVELOPMENT_ROADMAP.md` — requirement/API/test traceability and milestone sequence.

If documents conflict, stop and report the exact conflicting passages and decision authority. Do not silently reconcile product behavior in code.

## 3. Gates before production code

These are owner/engineering gates, not permission for Claude to decide autonomously:

| Gate | Required resolution | Why it blocks or constrains implementation |
|---|---|---|
| Q-T01/Q-T02 | Framework and framework-independent core boundary | Determines build, wrapper, dependency and ABI choices |
| Q-T03 | Minimum macOS and architecture target | Determines toolchain, deployment and CI matrix |
| Q-L04/Q-L05 | JUCE/dependency licence review | Legal/commercial dependency gate |
| Q-P06/Q-P09 | AU component strategy and host matrix | Determines host shell and bus contracts |
| Q-T12 | Parameter ID/host exposure manifest approval | Public automation and preset compatibility are difficult to change |
| AR-01 | Same-offset event tie-break and parameter writer precedence | Prevents accidental host/MIDI/sequencer/UI ordering semantics |
| AR-02 | Voice/grain overload selection and fallback policy | Prevents implementation-dependent stealing behavior |
| DEC-042 follow-up | Confirm global Freeze successful-subset versus all-or-nothing preparation failure | Existing documents flag this interpretation for owner review |
| Phase 5 | Explicit AC/test links or documented cross-cutting rationale for the 71 R1 requirements still lacking an acceptance/release-gate link | Prevents untestable completion claims |

Q-T07 numeric grain budget and numeric CPU/memory capacities remain measurement gates. Do not invent values. Any benchmark requires separate explicit owner authorization. Host prototypes and production DSP are distinct work items and require explicit scope authorization.

## 4. Implementation principles (non-negotiable)

- Implement only approved R1 requirements. L2/L3/OPT items must not enter R1 by implication.
- Preserve exactly four independent layers; each layer selects Granular or Sampler.
- Preserve DEC-020/DEC-040 live-input semantics and GS-ARC-007's parallel dry live-input path to master.
- Preserve DEC-025: unsaved recordings and Freeze snapshots are embedded in plugin state; library samples remain external references with relink/collect.
- Preserve DEC-031 macro base versus per-voice modulation semantics.
- Preserve DEC-039 transport clock behavior and DEC-042 Source Snapshot Freeze behavior.
- Audio callback: no allocation/deallocation, locks, blocking, file I/O, logging, UI calls, decoding, large copies or final destruction.
- Every cross-thread queue must document producer count, consumer count, capacity, ownership transfer, ordering, overflow and recovery. Never route multiple producers into an SPSC queue.
- No raw host SDK types in the framework-independent core.
- No raw mutable DSP pointers exposed to UI.
- Every structural change is prepared off-thread and published at a safe point with explicit lifetime/retirement handling.
- Failed replacement/restore preserves the prior valid runtime. No silent truncation, omission or fallback to different audio.
- Determinism must be testable with explicit state, event ordering, transport and RNG seeds.
- Do not claim a test passed unless it was actually run and its command/output is recorded.

## 5. Proposed implementation sequence (conditional)

The sequence below is a planning proposal. Start only after the owner authorizes the milestone and its gates.

### Milestone A — Build and test foundation
- Pin the selected compiler, C++ standard, framework/SDK versions, macOS deployment target and Universal 2 architecture.
- Create reproducible Debug/Release builds and CI for supported architectures.
- Create a headless test target independent of DAW hosts.
- Add formatting/static-analysis configuration and baseline test runner.
- Add RT-safety instrumentation hooks, initially as test infrastructure; do not claim complete RT proof.

**Exit evidence:** clean reproducible build, CI logs, unit-test runner, dependency/licence manifest and architecture record.

### Milestone B — Core types and deterministic runtime shell
- Define strongly typed frame count, sample rate, layer index, parameter ID, generation, sample offset and process mode.
- Define normalized process context and bus views; keep host SDK types in adapters.
- Implement deterministic event ordering only after AR-01 is approved.
- Implement parameter registry only after Q-T12 is approved.
- Add deterministic headless block renderer with seeded RNG and golden-test harness.

**Exit evidence:** type/range validation, event-order tests, block-boundary tests, deterministic repeated render and API review.

### Milestone C — Realtime infrastructure and lifecycle
- Implement bounded queues/mailboxes with proven producer/consumer topology.
- Implement structural command ownership transfer and generation checks.
- Implement non-RT retirement/reclamation and bounded backpressure.
- Implement immutable sample handles and replacement generations.
- Implement transactional memory admission accounting.

**Exit evidence:** queue saturation tests, stale-generation tests, retirement saturation tests, no-final-destruction-on-RT assertions, lifetime/race tests.

### Milestone D — Audio engine vertical slice
- Implement a minimal engine path with one layer, then four-layer routing only after contract tests.
- Add audio input/output views and master/layer output mapping.
- Preserve wet per-layer input path and required parallel dry-to-master path.
- Add deterministic voice allocation and grain admission only after AR-02 is approved.
- Add the simplest approved engine slice; do not add unapproved DSP features.

**Exit evidence:** offline golden renders, routing/impulse tests, four-layer isolation and RT-safety checks.

### Milestone E — Sample, recording, Freeze and state
- Implement off-thread decode/prepare and safe sample replacement.
- Implement preallocated recording with committed-frame and retention protection.
- Implement DEC-042 Freeze/Thaw and one global transaction according to owner-approved failure semantics.
- Implement DEC-025 embedded recording serialization, external references and relink/collect.
- Implement transactional restore that preserves prior runtime on failure.

**Exit evidence:** forced ring-wrap tests, recording soak, Freeze concurrency/failure tests, save interruption/corruption tests and restore round-trip tests.

### Milestone F — Host adapters and UI
- Build Standalone, AU and VST3 adapters against the normalized process contract.
- Validate bus activation, variable/zero-frame callbacks, transport, MIDI/MPE, automation, live input and state callbacks in the approved host matrix.
- UI observes snapshots and publishes commands; it never owns live DSP objects.
- Do not add AU component variants or optional formats without owner approval.

**Exit evidence:** host/version/OS matrix, plugin validation logs, session save/reopen tests and documented unsupported host capabilities.

## 6. Required work discipline for every Claude Code task

1. Start with `git status`, current branch, current commit and relevant open PR state.
2. Read the applicable requirements, decision records, architecture contract and acceptance criteria.
3. State the exact task scope and files expected to change before editing.
4. Implement the smallest vertical slice; avoid broad refactors and speculative abstractions.
5. Add/update tests in the same change. Tests must target requirement IDs and failure paths, not only happy paths.
6. Run only the authorized build/test commands. Do not run performance benchmarks unless separately authorized.
7. Inspect the full diff for scope creep, RT violations, undocumented numeric limits and product-decision drift.
8. Update traceability and technical documentation with actual behavior and evidence.
9. Commit on a dedicated branch and open a Draft PR. Never merge.
10. Report commit SHA, files changed, tests actually run, tests not run, known risks, open decisions and next smallest task.

## 7. Mandatory test families

- Core types and parameter conversion/range validation.
- Deterministic event ordering, same-offset precedence and block offsets 0 / final valid sample.
- Variable block sizes, zero-frame callbacks where permitted, sample-rate and bus-layout changes.
- Queue full, event overflow, panic/All Sound Off reserved delivery and producer concurrency.
- Sample replacement while voices/grains read old data; stale asynchronous completion; retirement saturation.
- Memory admission with active + pending + recording + Freeze + serialization staging coexistence.
- Recording ring wrap during Freeze, export and state capture.
- Freeze single-layer/global timing, mixed failures and all-sound-off semantics.
- Save interrupted at each chunk boundary, corrupt/missing chunks, missing external assets and failed restore publication.
- Four-layer signal routing, wet/dry live input, master/layer output taps and duplicate-summing prevention.
- Host automation, MIDI/MPE, transport arbitration and Program Change mapping.
- Standalone/AU/VST3 equivalence under the documented deterministic boundary.

Do not claim these tests are complete merely because the test names or harness exist.

## 8. Current readiness snapshot

- Phase 2 baseline is merged in `main`; Phase 3 remains an open Draft PR.
- Phase 3 architecture maps 124/124 R1 requirement IDs at module/interface level.
- Corrected acceptance audit: 49/124 direct AC-table references plus four cross-domain release-gate links (53/124); 71 requirements remain unlinked and are listed in `docs/PHASE5_TEST_DESIGN_BACKLOG.md`.
- Event precedence (AR-01) and voice/grain overload (AR-02) remain open.
- Sample replacement, memory admission, state save/restore and host adapter contracts have been refined in Phase 3 documentation.
- No production DSP, plugin shell, UI, host prototype, benchmark or implementation code is authorized by this handoff.

## 9. Copy/paste kickoff prompt for Claude Code

Use only after the project owner explicitly authorizes a specific milestone:

> You are the implementation engineer for GRANULAR SYNTH. Before changing code, read `docs/CLAUDE_CODE_IMPLEMENTATION_HANDOFF.md` and every source-of-truth document listed in §2. Inspect the current branch, git status, open PRs and repository structure. Do not assume that a proposal is an approved decision. Do not choose or change framework, deployment target, AU component strategy, parameter IDs, event precedence, voice/grain stealing, Freeze global-failure semantics, numeric capacities or product scope without explicit owner approval. Do not implement L2/L3/OPT features. Do not run benchmarks. Do not merge any PR. First return a concise implementation-readiness report identifying (a) approved decisions, (b) unresolved blocking gates, (c) the smallest safe first milestone, (d) exact files/tests it would touch, and (e) commands it proposes to run. Wait for owner authorization before editing files. Once authorized, implement only the named milestone, add requirement-linked tests, run only authorized checks, inspect the complete diff, commit on a dedicated branch and open a Draft PR. Report evidence honestly; never claim unrun tests passed.

## 10. Handoff completion checklist

- [ ] Owner approves framework/toolchain and legal gate.
- [ ] Owner approves host/component prototype scope.
- [ ] Owner resolves AR-01 and AR-02.
- [ ] Owner confirms global Freeze failure semantics.
- [ ] Q-T12 parameter manifest approved.
- [ ] Phase 5 acceptance plan addresses all 124 R1 requirements.
- [ ] Owner authorizes a named first coding milestone.
- [ ] Claude returns readiness report before editing.

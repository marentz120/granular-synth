# Project documentation index

Referenced from `CLAUDE.md` ("Maintain the project documents listed in docs/README.md").

| Document | Purpose | Status |
|---|---|---|
| [PRODUCT_VISION.md](PRODUCT_VISION.md) | Product intent, benchmark and approved R1 scope | Updated 2026-11-27 |
| [R1_PRODUCT_SPECIFICATION.md](R1_PRODUCT_SPECIFICATION.md) | Authoritative R1 product decisions and behavior | Approved product scope v1.0 |
| [R1_ACCEPTANCE_CRITERIA.md](R1_ACCEPTANCE_CRITERIA.md) | Phase 2 product-level acceptance criteria and cross-domain release gates; Phase 5 execution details remain open | Draft; 53 R1 acceptance/release-gate links |
| [GR_MEGA_FEATURE_MAP.md](GR_MEGA_FEATURE_MAP.md) | Traceable GR-MEGA 1.8.d feature inventory with evidence classes | Phase 1 draft (Issue #1) |
| [PHASE1_REFERENCE_SUMMARY.md](PHASE1_REFERENCE_SUMMARY.md) | Executive summary of the reference analysis | Phase 1 draft (Issue #1) |
| [GRANULAR_SYNTH_REQUIREMENTS.md](GRANULAR_SYNTH_REQUIREMENTS.md) | 168 requirement rows with approved primary release tiers and traceability | Phase 2 v1.0 — tier placement approved by DEC-041; technical values remain gated |
| [DSP_ARCHITECTURE.md](DSP_ARCHITECTURE.md) | Architecture options, RT model, framework comparison | Proposal v0.6 — technical decisions remain open |
| [UI_UX_SPECIFICATION.md](UI_UX_SPECIFICATION.md) | Screen layout, workspace behavior, touch/encoder interaction and performance UX | Phase 4 draft v0.2 — owner review required |
| [MIDI_MAPPING_UX_SPECIFICATION.md](MIDI_MAPPING_UX_SPECIFICATION.md) | MIDI Learn, manual mapping, scopes, conflicts and performance-time mapping UX | Phase 4 draft v0.2 — owner review required |
| [SCREEN_STATE_AND_NAVIGATION_SPECIFICATION.md](SCREEN_STATE_AND_NAVIGATION_SPECIFICATION.md) | Instrument/UI/transient state boundaries, navigation and recovery behavior | Phase 4 draft v0.2 — owner review required |
| [PHASE4_R1_UI_TRACEABILITY_AUDIT.md](PHASE4_R1_UI_TRACEABILITY_AUDIT.md) | Maps all 124 R1 requirement IDs exactly once to proposed UI surfaces or internal/non-UI handling; reconciles earlier gaps while retaining Phase 5 acceptance gaps | Phase 4 audit v0.2 — owner review required |
| [PHASE4_SIGNOFF_READINESS_REPORT.md](PHASE4_SIGNOFF_READINESS_REPORT.md) | Final traceability counts, cross-document consistency findings, owner review gates and Phase 5 test-design priorities; explicitly not an approval or implementation claim | Phase 4 readiness report v0.1 — owner review required |
| [PHASE5_R1_ACCEPTANCE_TEST_DESIGN.md](PHASE5_R1_ACCEPTANCE_TEST_DESIGN.md) | Test-design blueprint for all 71 unlinked R1 requirements: candidate oracles, shared fixtures, evidence artifacts, decision gates and execution order; no tests executed | Phase 5 design v0.3 — proposal, owner review required |
| [PHASE5_ALL_REQUIREMENTS_AUDIT.md](PHASE5_ALL_REQUIREMENTS_AUDIT.md) | Full audit of all 168 requirements, 124 R1 traceability rows, 71 acceptance gaps, unresolved R1 semantics, technical/owner gates and implementation readiness | Phase 5 audit v0.3 — documentation review; gates remain open |
| [PHASE5_DECISION_CLOSURE_REGISTER.md](PHASE5_DECISION_CLOSURE_REGISTER.md) | Consolidated product, technical, host, UI/UX, legal and release decisions still required before final acceptance and implementation | Phase 5 decision register v0.1 — planning only; no decisions approved |
| [MODULATION_SPECIFICATION.md](MODULATION_SPECIFICATION.md) | R1 LFO, matrix, sources, destination eligibility, macros and modulation feedback behavior | v0.2; MTX-05 candidate inventory linked, canonical registry still gated by Q-T12 |
| [R1_MODULATION_DESTINATION_ELIGIBILITY_MATRIX.md](R1_MODULATION_DESTINATION_ELIGIBILITY_MATRIX.md) | Candidate R1 modulation destination families, source-scope compatibility and explicit deferred-feature exclusions | v0.1; planning inventory only, MTX-05 remains open pending Q-T12 |
| [R1_ENGINE_PARAMETER_CANDIDATE_INVENTORY.md](R1_ENGINE_PARAMETER_CANDIDATE_INVENTORY.md) | Candidate Granular, Sampler and shared-voice parameter rows with source-backed ranges/defaults and explicit lifecycle unknowns | v0.1; planning inventory only, Q-T12 and MTX-05 remain open |
| [R1_ENGINE_PARAMETER_COVERAGE_AUDIT.md](R1_ENGINE_PARAMETER_COVERAGE_AUDIT.md) | Cross-checks engine/shared-voice parameter candidates against touch UI, encoder A–H, MIDI Learn, modulation and host automation | v0.1; gaps recorded, Q-T12 and MTX-05 remain open |
| [ADVANCED_GRANULAR_CONTROL_REACHABILITY.md](ADVANCED_GRANULAR_CONTROL_REACHABILITY.md) | Maps every R1 advanced Granular control to proposed touch, contextual encoder, MIDI Learn and modulation surfaces | v0.1; UI reachability proposal, owner review required |
| [SHARED_VOICE_CONTROL_REACHABILITY.md](SHARED_VOICE_CONTROL_REACHABILITY.md) | Maps voice, four ADSR envelopes, LPF/HPF and sub-oscillator controls to proposed UI and controller surfaces | v0.1; UI reachability proposal, owner review required |
| [SAMPLER_SPECIFICATION.md](SAMPLER_SPECIFICATION.md) | R1 sampler playback, loop/release, interpolation, voice and sample-state behavior | v0.1; interpolation mode/default remains an engineering gate |
| [EFFECTS_SPECIFICATION.md](EFFECTS_SPECIFICATION.md) | Seven approved R1 effects, per-layer slots, routing, state and unresolved parameter behavior | v0.2; FX implementation deferred to M14 by DEC-056 |
| [FX_MODULE_INTEGRATION_PLAN.md](FX_MODULE_INTEGRATION_PLAN.md) | Internal FX module responsibilities, preparation requirements and final M14 entry/exit gates | v0.1; planning only, no SDK/API/DSP implementation approved |
| [PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md](PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md) | Four-encoder workspaces, proposed R1-backed A–H engine banks, variable-length Pattern viewport and persistent performance controls | Phase 4 draft v0.2 — owner review required |
| [PHASE4_PERFORMANCE_ACCESSIBILITY_AUDIT.md](PHASE4_PERFORMANCE_ACCESSIBILITY_AUDIT.md) | Action-by-action audit with closure progress, remaining owner UX reviews and Phase 3/5 gates | Phase 4 audit v0.2 — owner review required |
| [PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md](PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md) | Named MIDI/controller targets, explicit scopes, chord-record and audio-record workflows, Performance Bank/Set and Program Change UX | Phase 4 draft v0.2 — owner review required |
| [PHASE4_CONSOLIDATED_SCREEN_LAYOUTS.md](PHASE4_CONSOLIDATED_SCREEN_LAYOUTS.md) | Consolidated wireframe-level layouts for six workspaces, global frame, performance row, sequencer strip, library and MIDI overlays | Phase 4 draft v0.2 — owner review required |
| [LIVE_PERFORMANCE_SPECIFICATION.md](LIVE_PERFORMANCE_SPECIFICATION.md) | Controller-only live-performance and R1 sequencer behavior; detailed UI/acceptance work remains later-phase | Product behavior approved v0.3; UI design pending |
| [DEVELOPMENT_ROADMAP.md](DEVELOPMENT_ROADMAP.md) | Phases, dependencies, exit criteria, milestone plan | Proposal v0.8 — FX delivery moved to final M14 milestone; no phase is automatically authorized |
| [DECISIONS_LOG.md](DECISIONS_LOG.md) | Decision records and approval status | Living |
| [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) | Owner decisions, technical decisions, evidence gaps and deferred benchmark-only questions | Living, v0.7 |
| [R1_REQUIREMENTS_MAPPING.md](R1_REQUIREMENTS_MAPPING.md) | Initial audit mapping requirements against R1_PRODUCT_SPECIFICATION.md; findings and non-binding recommendations | Initial pass, findings addressed 2026-11-27 |
| [TESTING_STRATEGY.md](TESTING_STRATEGY.md) | Test approach and release validation | Initial principles (Phase 5) |

- PHASE3_API_SYMBOL_TRACEABILITY.md — candidate class/API-operation mapping for all 124 R1 requirements; acceptance gaps and owner gates remain explicit.
- [PHASE5_TEST_DESIGN_BACKLOG.md](PHASE5_TEST_DESIGN_BACKLOG.md) — 71 R1 requirements still needing explicit acceptance/test links or cross-cutting rationale; includes the expanded R1 semantic-gate register, planning only.
- [PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md](PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md) — AU/VST3/Standalone maximum-block capacity, transient scratch accounting, physical-input versus per-layer source routing, and validation obligations; proposal only, no budgets or benchmarks.
- [PHASE3_HOST_CAPABILITY_EVIDENCE_MATRIX.md](PHASE3_HOST_CAPABILITY_EVIDENCE_MATRIX.md) — official Logic/Live/Bitwig/REAPER host evidence for AU/VST3 multi-output and live-input routing; evidence gaps and prototype exit criteria, not certification.
- [PHASE3_AU_COMPONENT_STRATEGY_AUDIT.md](PHASE3_AU_COMPONENT_STRATEGY_AUDIT.md) — evidence-based `aumu`/`aufm` trade-off, full-band live-input and MIDI/MPE host-test gates; recommendation only, no AU subtype selected.
- [PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md](PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md) — VST3 instrument category, processor/controller split, event/live-audio buses, five outputs, MPE Note Expression and host-validation gates; recommendation only, no category or bus type selected.
- [PHASE3_CROSS_FORMAT_EVENT_TRANSPORT_PARITY_AUDIT.md](PHASE3_CROSS_FORMAT_EVENT_TRANSPORT_PARITY_AUDIT.md) — AU/VST3/Standalone event timestamps, automation curves, MPE note identity, transport validity and loop-discontinuity parity; proposal and test design only.
- [PHASE3_VST3_HOST_CAPABILITY_MATRIX.md](PHASE3_VST3_HOST_CAPABILITY_MATRIX.md) — host-by-host VST3 evidence for instrument discovery, raw live input, MPE, five stereo outputs, bus activation and session persistence; evidence map only, no host certified.


- [PHASE3_MPE_NOTE_IDENTITY_AND_LIFECYCLE_AUDIT.md](PHASE3_MPE_NOTE_IDENTITY_AND_LIFECYCLE_AUDIT.md) — AU cable/member-channel versus VST3 noteId lifecycle, per-note expression mapping, bounded identity ownership and AR-03…AR-05 review gates; proposal only, no final matching or type mapping selected.
- [PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md](PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md) — official Logic/Live/Bitwig/REAPER MPE host evidence, distinguishing channelized MIDI 1.0 MPE from VST3 Note Expression; evidence map and Phase 5 fixtures only, no host certified.

- [PHASE3_MPE_EVENT_NORMALIZATION_AND_LIFECYCLE_FIXTURES.md](PHASE3_MPE_EVENT_NORMALIZATION_AND_LIFECYCLE_FIXTURES.md) — deterministic AU/VST3/Standalone MPE ingress, identity, expression, fan-out, release, channel-reuse and overload fixtures; Phase 5 test design only, with AR-03…AR-05 and host evidence gates open.
- [PHASE3_MPE_RESET_STATE_RESTORE_AND_HOST_LIFECYCLE_AUDIT.md](PHASE3_MPE_RESET_STATE_RESTORE_AND_HOST_LIFECYCLE_AUDIT.md) — distinguishes MIDI controller/note/sound reset, host render reset and persistent state restore; documents DEC-025/DEC-042 persistence, MPE generations and AR-06 owner gate.


- [PHASE3_CROSS_HOST_STATE_RESTORE_AND_RESET_EVIDENCE_MATRIX.md](PHASE3_CROSS_HOST_STATE_RESTORE_AND_RESET_EVIDENCE_MATRIX.md) — official Logic/Live/Bitwig/REAPER state-save, preset/reset and process-reload evidence; separates host documentation from AU/VST3 API contracts, DEC-025/DEC-042 payload tests, AR-06 alternatives and unverified bus/callback behavior. Evidence map only; no host certified.

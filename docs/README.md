# Project documentation index

Referenced from `CLAUDE.md` ("Maintain the project documents listed in docs/README.md").

| Document | Purpose | Status |
|---|---|---|
| [PRODUCT_VISION.md](PRODUCT_VISION.md) | Product intent, benchmark and approved R1 scope | Updated 2026-11-27 |
| [R1_PRODUCT_SPECIFICATION.md](R1_PRODUCT_SPECIFICATION.md) | Authoritative R1 product decisions and behavior | Approved product scope v1.0 |
| [GR_MEGA_FEATURE_MAP.md](GR_MEGA_FEATURE_MAP.md) | Traceable GR-MEGA 1.8.d feature inventory with evidence classes | Phase 1 draft (Issue #1) |
| [PHASE1_REFERENCE_SUMMARY.md](PHASE1_REFERENCE_SUMMARY.md) | Executive summary of the reference analysis | Phase 1 draft (Issue #1) |
| [GRANULAR_SYNTH_REQUIREMENTS.md](GRANULAR_SYNTH_REQUIREMENTS.md) | Requirements with proposed tiers and traceability | Draft v0.5 |
| [DSP_ARCHITECTURE.md](DSP_ARCHITECTURE.md) | Architecture options, RT model, framework comparison | Investigation v0.4 |
| [UI_UX_SPECIFICATION.md](UI_UX_SPECIFICATION.md) | UI/UX specification | Discovery not started (Phase 4) |
| [LIVE_PERFORMANCE_SPECIFICATION.md](LIVE_PERFORMANCE_SPECIFICATION.md) | Controller-only live-performance and R1 sequencer behavior; detailed UI/acceptance work remains later-phase | Product behavior approved v0.3; UI design pending |
| [DEVELOPMENT_ROADMAP.md](DEVELOPMENT_ROADMAP.md) | Phases, dependencies, exit criteria, milestone plan | Proposal v0.2 |
| [DECISIONS_LOG.md](DECISIONS_LOG.md) | Decision records and approval status | Living |
| [OPEN_QUESTIONS.md](OPEN_QUESTIONS.md) | Owner decisions, technical decisions, evidence gaps | Living, v0.4 |
| [R1_REQUIREMENTS_MAPPING.md](R1_REQUIREMENTS_MAPPING.md) | Initial audit mapping requirements against R1_PRODUCT_SPECIFICATION.md; findings and non-binding recommendations | Initial pass, findings addressed 2026-11-27 |
| [TESTING_STRATEGY.md](TESTING_STRATEGY.md) | Test approach and release validation | Initial principles (Phase 5) + A0 harness record |
| [BUILD_AND_TOOLCHAIN_RECORD.md](BUILD_AND_TOOLCHAIN_RECORD.md) | What the build actually is, observed toolchain, recorded deviations, and what is explicitly not validated | Milestone A0 v0.1 |
| [DEPENDENCY_AND_LICENCE_MANIFEST.md](DEPENDENCY_AND_LICENCE_MANIFEST.md) | Every dependency and its licence; currently zero third-party dependencies | Living, v0.1 (Q-L05 open) |

- PHASE3_API_SYMBOL_TRACEABILITY.md — candidate class/API-operation mapping for all 124 R1 requirements; acceptance gaps and owner gates remain explicit.
- [PHASE5_TEST_DESIGN_BACKLOG.md](PHASE5_TEST_DESIGN_BACKLOG.md) — 71 R1 requirements still needing explicit acceptance/test links or cross-cutting rationale; planning only, no thresholds or test IDs assigned.
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

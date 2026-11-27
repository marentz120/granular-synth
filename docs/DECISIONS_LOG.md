# Decisions Log

Statuses: **Proposed**, **Approved**, **Rejected**, **Superseded**.

| ID | Decision | Status | Notes |
|---|---|---|---|
| DEC-001 | Use Tasty Chips GR-MEGA as primary functional benchmark | Approved | Owner-directed; does not authorize copying proprietary implementation. |
| DEC-002 | Use `marentz120/granular-synth` as shared project record | Approved | Repository supplied by owner. |
| DEC-003 | Separate reference research/planning from production implementation | Approved | Requirements and architecture review before production coding. |
| DEC-004 | Target macOS standalone, AU and VST3 | Proposed | Initial intent; validate framework and formats. |
| DEC-005 | Select JUCE/C++ | Proposed | Candidate only; compare options before approval. |
| DEC-006 | Determine first-release engines and layer/voice limits | Pending | Decide after reference analysis and owner input. |
| DEC-007 | Add evidence tag [RN] (manufacturer release notes) alongside the four CLAUDE.md classes | Proposed | Issue #1. Reviewer may fold RN into Manual-confirmed or Unverified. |
| DEC-008 | R1 engines Granular + Granular Slice + Sampler; Tape L2; Spectral L3; 4 layers in R1 | Proposed | Issue #1 recommendation for DEC-006; see Q-P04, Q-P18, Q-P20. Owner decision required. |
| DEC-009 | Step sequencer in L2, not R1 | Proposed | Q-P05; depends on Q-P19 (studio vs live). Owner decision required. |
| DEC-010 | Plugins support live-input granulation (VST3 input bus; AU music-effect variant) | Proposed | Q-P06. Owner decision required. |
| DEC-011 | Framework-independent DSP core library, regardless of framework choice | Proposed | Q-T02; complements DEC-005. |
| DEC-012 | RAM-resident samples played at source rate with rate-compensated increments; streaming only for long Sampler files | Proposed | Q-T04, Q-T05. |
| DEC-013 | Stable string parameter IDs; expose all automatable parameters grouped per layer | Proposed | Q-T12 — irreversible after first public release. |
| DEC-014 | Roadmap phase numbering aligned with CLAUDE.md (8 phases) | Proposed | DEVELOPMENT_ROADMAP.md v0.2; v0.1 text preserved per phase. |

## Decision record template
- ID and date:
- Decision and status:
- Context:
- Options considered:
- Rationale and consequences:
- Owner approval:

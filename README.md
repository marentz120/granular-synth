# GRANULAR SYNTH

A professional sample-based granular instrument for macOS, designed for studio sound design and live performance.

## Current project status

**Product scope and R1 release tiers are approved. Architecture, UI/UX and acceptance testing are still in review/planning. Production implementation has not started.**

- 168 requirement rows: 124 R1, 27 L2, 9 L3 and 8 optional.
- R1 formats: standalone macOS application, AU and VST3.
- R1 engine scope: Granular and Sampler, across four independent layers.
- R1 also includes a central performance sequencer, live audio input, Freeze, recording and generic MIDI-controller operation.
- The primary functional benchmark is Tasty Chips GR-MEGA. It is a behavior reference only; do not copy proprietary firmware, source code, artwork or protected trade dress.

## Quality requirements

- Real-time-safe audio processing: no blocking I/O, unpredictable waits or routine heap allocation in the audio callback.
- Explicit bounded resource limits and deterministic overload behavior.
- Parameter smoothing where needed to avoid clicks and zipper noise.
- Safe off-thread sample preparation and ownership transfer.
- Consistent parameter/state behavior across standalone, AU and VST3.
- Reproducible tests and measured CPU, memory, latency and voice/grain capacity under approved conditions.

## Requirement and test status

- All 124 R1 requirements are mapped to proposed architecture modules/interfaces, candidate API symbols and proposed UI surfaces or internal handling.
- 49 R1 requirements have direct acceptance-criteria links; 4 more have cross-domain release-gate links.
- 71 R1 requirements still need an explicit acceptance link or documented cross-cutting rationale. A Phase 5 test-design blueprint covers those 71, but no tests have been executed.
- Technical values such as minimum macOS, supported sample rates/buffer sizes, parameter IDs, global grain budget and host certification remain gated by the relevant architecture decisions.

## Source of truth

- `CLAUDE.md` — project rules, authorization and workflow.
- `docs/README.md` — complete documentation index.
- `docs/GRANULAR_SYNTH_REQUIREMENTS.md` — canonical 168-row requirement inventory and approved primary tiers.
- `docs/DECISIONS_LOG.md` — approved and proposed decisions.
- `docs/OPEN_QUESTIONS.md` — unresolved product, technical, evidence and legal questions.
- `docs/PHASE5_ALL_REQUIREMENTS_AUDIT.md` — latest complete requirements and readiness audit.

**Current work is documentation and planning only.** Do not implement production DSP, plugin/UI code, host prototypes or benchmarks, and do not merge a pull request or create a release without explicit owner authorization.

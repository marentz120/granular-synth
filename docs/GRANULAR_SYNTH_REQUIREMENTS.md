# GRANULAR SYNTH Requirements

**Status:** Draft; subject to owner and architecture review.

## Platform
- REQ-PLAT-001: Target macOS.
- REQ-PLAT-002: Provide a standalone application.
- REQ-PLAT-003: AU and VST3 are proposed plugin formats, pending validation.

## Audio engine
- REQ-AUD-001: Audio processing must be real-time safe.
- REQ-AUD-002: The audio callback must not perform blocking I/O, unpredictable waits, or routine heap allocation.
- REQ-AUD-003: Resource limits and overload behavior must be explicit and testable.
- REQ-AUD-004: Parameter changes must avoid audible zipper noise and discontinuities where appropriate.
- REQ-AUD-005: Supported sample rates and buffer sizes must be specified during architecture/QA planning.

## Instrument capabilities
Detailed requirements for granular synthesis, samples, layers, voices, modulation, sequencing, effects, MIDI, presets, and alternate engines will be derived from the GR-MEGA manual and owner decisions. Do not assume parity or numeric limits before verification.

## Quality attributes
- QA-001: Prevent dropouts, clicks, and denormal-related performance problems.
- QA-002: Make DSP components independently testable.
- QA-003: Preserve consistent parameter meaning across standalone and plug-in builds.
- QA-004: Provide reproducible builds and automated tests.
- QA-005: Measure CPU, memory, latency and voice/grain capacity under defined conditions.

## Traceability
Each detailed requirement must have a unique ID, source/evidence classification, priority, rationale, dependencies and acceptance criteria.

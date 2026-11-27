# GRANULAR SYNTH Requirements

**Status:** Draft; subject to product-owner and architecture review.

## 1. Product targets
- REQ-PLAT-001: Target macOS.
- REQ-PLAT-002: Provide a standalone application.
- REQ-PLAT-003: AU and VST3 are proposed plug-in formats, pending technical validation.

## 2. Audio engine requirements
- REQ-AUD-001: Audio processing must be real-time safe.
- REQ-AUD-002: The audio callback must avoid blocking I/O, unpredictable waits, and routine heap allocation.
- REQ-AUD-003: Engine resource limits and overload behavior must be explicit and testable.
- REQ-AUD-004: Parameter changes must avoid audible zipper noise and discontinuities where applicable.
- REQ-AUD-005: Output must remain stable at supported sample rates and buffer sizes, to be specified during architecture and QA planning.

## 3. Instrument requirements — pending reference analysis
Detailed requirements for granular synthesis, sample management, layers, voices, modulation, sequencing, effects, MIDI, presets, and alternate engines will be derived from the GR-MEGA manual and owner decisions. Do not assume parity or numeric limits before verification.

## 4. Quality attributes
- QA-001: Prevent audio dropouts, clicks, and denormal-related performance problems.
- QA-002: Make DSP components independently testable.
- QA-003: Preserve consistent parameter meaning across standalone and plug-in builds.
- QA-004: Provide reproducible builds and automated tests.
- QA-005: Measure CPU, memory, latency, and voice/grain capacity under defined test conditions.

## 5. Requirement traceability
Each detailed requirement must have a unique ID, source/evidence classification, priority, rationale, dependencies, and acceptance criteria.

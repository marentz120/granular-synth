# Product Vision

GRANULAR SYNTH is a professional granular synthesizer for macOS, intended as a standalone instrument and audio plug-in.

## Functional benchmark
The primary benchmark is Tasty Chips GR-MEGA. Study its documented capabilities and workflows, then define an original GRANULAR SYNTH product specification. This is a functional reference, not permission to copy proprietary firmware or source code.

## Goals
- Expressive, high-quality granular sound design and musical performance.
- A coherent sample-based workflow.
- Stable, low-latency, drop-out-free real-time audio.
- Standalone and plug-in use with consistent sound and parameter behavior.
- MIDI control and automation where appropriate.
- A genuine live-performance tool, fully controller-operable for essential operations, not merely a plugin wrapper (DEC-016; see docs/LIVE_PERFORMANCE_SPECIFICATION.md).

## Proposed targets
macOS standalone, AU and VST3. These remain subject to technical validation and owner confirmation.

## Use case (decided)
Both studio sound design and live performance are primary — **DEC-015, Approved 2026-11-27** (owner decision on PR #2). This does not by itself decide layer count, engine scope, sequencer tier, or plugin live input (see "Not yet decided" below).

## Not yet decided
- Exact GR-MEGA parity versus inspired-by/differentiated product.
- First-release engines and capabilities.
- Layer, voice and grain limits.
- Framework, supported macOS versions and CPU architectures.

See docs/OPEN_QUESTIONS.md and docs/DECISIONS_LOG.md.

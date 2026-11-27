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

## Delivery targets
macOS Standalone, AU and VST3 are the intended R1 delivery targets. AU/VST3 format and component details remain subject to Phase 3 validation; this is not a framework or implementation approval.

## Primary use cases (decided)
Both studio sound design and live performance are primary — **DEC-015, Approved 2026-11-27**. Standalone is a complete performance instrument, not merely a plug-in wrapper.

## Approved R1 product scope
- Four independent layers, each selectable as Granular or Sampler (DEC-017).
- Granular and Sampler engines; Granular Slice deferred, Tape later (L2), Spectral later (L3).
- Live input per layer, Freeze per layer/globally, per-layer recording and stereo master-mix recording.
- Full central step sequencer, Patterns/Chains/Performance Sets, performance controls and generic MIDI-controller editing/operation.
- See docs/R1_PRODUCT_SPECIFICATION.md and DECISIONS_LOG.md DEC-017…DEC-021.

## Still open
- Framework, supported macOS versions, CPU architectures and measured resource budgets.
- Exact AU/VST3 component/bus implementation and host validation.
- Detailed UI/UX and controller-mapping workflows.
- Legal, naming, distribution, licensing and factory-content decisions.

See docs/OPEN_QUESTIONS.md and docs/DECISIONS_LOG.md.

# DSP and Application Architecture

**Status:** Investigation only; no framework or implementation approved.

## Goals
- Separate DSP engine from UI, host integration and persistence.
- Reuse a shared instrument engine for standalone and plug-in front ends where feasible.
- Make real-time behavior predictable, measurable and testable.
- Support incremental engine development.

## Candidate logical modules
1. Audio Core: processing graph, transport context, sample-rate/block-size handling.
2. Voice Manager: note allocation, release, stealing policy and polyphony limits.
3. Granular Engine: scheduler, grain voices, windows, playback heads, interpolation and spatialization.
4. Sample Store: immutable sample assets, metadata, preload/stream policy and background decoding.
5. Modulation System: source generation, routing, smoothing and destination scaling.
6. Effects Graph: defined processing placement based on requirements.
7. Parameter Model: stable IDs, ranges, units, serialization, automation and MIDI mappings.
8. Preset/Project Layer: versioned serialization and sample references; disk access off audio thread.
9. Host Adapters: standalone audio/MIDI devices and AU/VST3 interfaces.
10. UI: parameter editing, sample visualization and accessible interaction.

## Real-time constraints
- No synchronous file access in the audio callback.
- No mutex waits or blocking communication in the audio callback.
- Avoid allocation/deallocation in the audio callback.
- Use bounded communication between UI/background and audio threads.
- Prepare sample data off-thread and transfer ownership safely.
- Define deterministic behavior when CPU or voice/grain budgets are exceeded.
- Smooth parameters where abrupt changes could cause discontinuities.
- Test thread safety, denormals, sample-rate changes, buffer-size changes and reset behavior.

## Framework evaluation
JUCE/C++ is a candidate, not a decision. Compare licensing, macOS deployment, AU/VST3 support, UI flexibility, testing, maintainability and developer expertise before recommending a framework.

## Decisions required
Framework/language; threading model; target macOS and CPU architectures; initial release engines; sample formats and streaming; measured CPU/memory budgets; plugin formats and distribution/signing.

# DSP and Application Architecture

**Status:** Architecture investigation only. No framework or implementation approved.

## Goals
- Separate the DSP engine from UI, host integration, and persistence.
- Support standalone and plug-in front ends over a shared instrument engine where technically appropriate.
- Make real-time behavior predictable and measurable.
- Support incremental engine development and automated testing.

## Candidate logical modules
1. **Audio Core:** processing graph, transport context, sample-rate and block-size handling.
2. **Voice Manager:** note allocation, release, stealing policy, polyphony limits.
3. **Granular Engine:** grain scheduler, grain voices, windows, playback heads, interpolation and spatialization.
4. **Sample Store:** immutable sample assets, metadata, preload/stream policy, background decoding.
5. **Modulation System:** source generation, routing, smoothing and destination scaling.
6. **Effects Graph:** per-voice/per-layer/master placement only when justified by requirements.
7. **Parameter Model:** stable IDs, ranges, units, serialization, automation and MIDI mappings.
8. **Preset/Project Layer:** versioned serialization and asset references; all disk access off the audio thread.
9. **Host Adapters:** standalone audio/MIDI devices and AU/VST3 interfaces.
10. **UI:** parameter editing, sample visualization and accessible interaction.

## Real-time constraints
- No synchronous file access in the audio callback.
- No mutex waits or blocking inter-thread communication in the audio callback.
- Avoid dynamic allocation and deallocation in the audio callback.
- Use bounded queues or other bounded communication mechanisms between UI/background and audio threads.
- Prepare sample data and expensive resources off-thread, then transfer ownership safely.
- Define deterministic behavior when CPU or voice/grain budgets are exceeded.
- Smooth parameters whose abrupt changes could cause discontinuities.
- Test thread safety, denormals, sample-rate changes, buffer-size changes and reset behavior.

## Framework investigation
JUCE/C++ is a candidate because it can support shared DSP/application code and common audio plug-in formats. It is not yet selected. Compare against other viable approaches, including licensing, macOS deployment, AU/VST3 support, code signing/notarization, UI flexibility, testability, long-term maintenance and developer expertise.

## Decisions required
- Framework and language.
- Audio engine architecture and threading model.
- Target macOS versions and CPU architectures.
- Initial release engines and feature boundaries.
- Supported sample formats, sample rates and buffer ranges.
- Plug-in formats and distribution strategy.
- Layer/voice/grain budgets based on profiling targets rather than imitation of hardware specifications.

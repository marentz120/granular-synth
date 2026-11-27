# Testing Strategy

**Status:** Initial principles.

## DSP unit tests
- Grain window/envelope output and edge cases.
- Sample interpolation and playback boundaries.
- Scheduler rate, duration and bounded-capacity behavior.
- Pitch/time mapping and parameter scaling.
- Modulation routing, smoothing and clamping.
- Voice allocation, release and stealing.

## Real-time safety
- Detect audio-thread allocations and blocking where feasible.
- Stress dense polyphony and maximum configured grain count.
- Test CPU overload and deterministic resource reduction.
- Test sample-rate and buffer-size changes.
- Run long-duration playback and state-change tests.

## Integration
- Parameter persistence and stable parameter IDs.
- Preset save/load and missing-sample behavior.
- Standalone audio/MIDI device changes.
- AU/VST3 scanning, instantiation, automation and state restoration.
- Session save/reopen in representative hosts.

## Release validation
Define supported macOS versions, hardware, hosts and measurable thresholds before release. Record build configuration, commands, results, environment and known limitations for every milestone.

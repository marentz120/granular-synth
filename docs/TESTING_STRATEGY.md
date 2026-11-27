# Testing Strategy

**Status:** Initial testing principles.

## DSP unit tests
- Grain envelope/window output, normalization and edge conditions.
- Sample interpolation and playback boundary behavior.
- Grain scheduler rate, duration and bounded-capacity behavior.
- Pitch/time mapping and parameter scaling.
- Modulation routing, smoothing and value clamping.
- Voice allocation, release and stealing behavior.

## Real-time safety tests
- Detect allocations and blocking operations in the audio callback where feasible.
- Stress with dense polyphony and maximum configured grain count.
- Test CPU overload and confirm deterministic voice/grain reduction behavior.
- Test sample-rate and block-size changes.
- Run long-duration playback and state-change tests.

## Integration tests
- Parameter persistence and stable parameter IDs.
- Preset save/load and missing sample handling.
- Standalone audio/MIDI device changes.
- AU/VST3 scanning, instantiation, automation and state restoration.
- Session save/reopen in representative hosts.

## Release validation
Define supported macOS versions, hardware, hosts and test thresholds before release. Record exact build configuration, test command, result, environment and known limitations for each milestone.

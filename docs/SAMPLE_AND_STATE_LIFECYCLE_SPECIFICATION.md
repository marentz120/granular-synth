# Sample and State Lifecycle Specification

**Version:** 0.2 — Phase 3 proposal, DEC-042 aligned  
**Status:** Proposed; requires architecture and owner approval. No implementation is authorized.  
**Authority:** DEC-025/028/040, GS-SMG-001…021, GS-PRS-005/006, Q-T04/Q-T05/Q-T15/Q-T16.

## 1. Product policy (fixed by Phase 2)

- Unsaved recording buffers are embedded in plugin state (DEC-025).
- Existing library samples are external references; the product supports relink and collect.
- Granular sample duration target is at least six minutes stereo per layer at 48 kHz, subject to measured feasibility and safe memory use (DEC-028).
- Mono live input is centered and made available to both sides of stereo layer processing; explicit LL/RR duplication is not the default (DEC-040).
- R1 imports WAV, AIFF and FLAC, mono or stereo; R1 export is WAV.
- No file decoding, sample-rate conversion, file I/O or dynamic allocation occurs in the audio callback.

## 2. Asset model

Distinguish three concepts:
- **Library asset:** user-managed file in the searchable library; referenced by stable asset identity, content hash and source URI/path.
- **Prepared sample:** immutable decoded runtime representation for DSP, with native sample rate, channel count, frame count, audio payload and optional analysis products.
- **Recording buffer:** mutable capture resource owned by the active layer/master recorder; once stopped/committed it becomes a recorded asset and may be embedded in plugin state until exported/collected.

A patch refers to an asset identity, not a raw pointer or transient filesystem path. Runtime handles are rebuilt when state is restored.

## 3. Load and replacement lifecycle

1. Resolve the asset reference and verify existence/readability.
2. Decode and validate off-thread; reject unsupported/malformed input without changing the active sample.
3. Convert to the internal sample representation and prepare guard regions and any required metadata.
4. Build waveform overview progressively; optional analyses (onsets, centroid, zero crossings) are separate cancellable jobs.
5. Publish the completed immutable prepared sample to the audio thread using the bounded structural-command protocol.
6. Swap at a documented safe point, preserving old sample until no active voice/grain can read it.
7. Retire old data off-thread.

Missing library files do not crash the host. User can relink by content hash or explicit file selection. A failed replacement leaves the currently active sample intact.

## 4. Sample-rate policy

Q-T04 remains open. Candidate A (retain source-rate data and include sourceRate/hostRate in playback increments) is preferred for memory efficiency but requires measured interpolation/aliasing quality. Any SRC, if selected, runs off-thread.

Required validation matrix:
- Host rates: proposed 44.1, 48, 88.2, 96, 176.4 and 192 kHz.
- Source rates both matching and mismatching host rate.
- Forward/reverse, high/low pitch, loop boundary and fractional playback increments.
- Granular and Sampler paths independently.
- No clicks, unstable pitch, out-of-bounds reads or uncontrolled aliasing.

The final supported matrix must be explicit; proposed rates are not considered approved until verified.

## 5. Memory and resource admission

The six-minute stereo 48 kHz float32 target is 138.24 MB decimal per layer (raw audio payload), approximately 553 MB for four layers. This excludes analysis, guard frames, recording buffers, temporary copies, allocator overhead, voices, grains, effects and UI waveform data.

The runtime must maintain separate accounting for:
- prepared library samples;
- active and pending sample replacements;
- live recording buffers;
- committed but not exported recording buffers;
- analysis products;
- serialization staging buffers;
- engine/voice/grain/FX memory.

When a new sample or recording would exceed a safe measured budget:
- reject or stop the new operation before memory corruption or process instability;
- preserve the last valid active sample and valid captured audio;
- present a clear user-visible status;
- never silently truncate a saved recording or evict an in-use sample;
- do not attempt disk streaming in R1 unless separately approved and specified.

The actual budget, warning threshold and admission policy require reference-machine measurement in Phase 3/5. The arithmetic above is not a process-memory budget.

## 6. Recording lifecycle

States:
- `Idle`
- `Armed`
- `Recording`
- `Stopping`
- `CapturedUnsaved`
- `CommittedAsset`
- `Exporting`
- `Error/CapacityReached`

Per-layer recorders and the stereo master recorder are separate assets. Recording is written into preallocated buffers/rings; disk writing and WAV export happen off-thread. A buffer-full or resource-exhaustion condition stops capture safely, preserves valid frames and informs the user.

Recommended architecture defaults (pending product UX and host validation): per-layer capture is post-FX and pre-layer-fader, independent of layer fader/mute/solo; a muted or solo-isolated layer continues recording its own stem when armed. Master capture is the final stereo master mix after layer faders/mute/solo and before host output, including live input routed through the layer graph. No separate dry-to-master bypass is enabled by default. These tap positions are engineering recommendations, not new approved product requirements. Exact start modes, stop semantics and loop/scroll behavior remain to be specified and tested.

## 7. Plugin state and persistence — DEC-025 implementation

### 7.1 Required policy

- Unsaved recording buffers are embedded in plugin state.
- Existing library samples are referenced externally and support relink/collect.
- Do not convert this to “embed opt-in” or “reference-only” without a new owner decision.

### 7.2 Proposed state structure

Use a versioned manifest plus binary audio payload chunks:
- manifest: schema version, product version, patch/layer state, asset references, hashes, recording metadata, sample format/rate/channel/frame metadata;
- embedded chunks: unsaved recording PCM payload, with length and checksum;
- external references: stable asset ID, content hash, original path/URI and relink metadata;
- migration metadata: source schema version and explicit migration steps.

The precise container encoding (JSON manifest plus binary chunks is the candidate) must be chosen after host API and size constraints are verified. Avoid base64-encoding large audio payloads into JSON.

### 7.3 Save/restore behavior

- State save must operate on a stable snapshot; it must not block the audio thread.
- Host state callbacks may impose synchronous API constraints. The adapter must copy or snapshot bounded state without waiting for disk I/O or audio quiescence.
- If embedded recordings exceed a defined size/time budget, do not silently omit them. Return a documented failure or use a verified host-supported chunking strategy; notify the user and preserve the in-memory recording.
- Restore reconstructs the recording buffers and resolves external references off-thread before publishing runtime objects.
- If an external file is missing, load the rest of the project safely and expose a relink-required state.
- Checksums detect corruption; corrupt embedded audio is rejected without crashing or invalidating unrelated patch state.
- Save operations must be atomic or recoverable after interruption.

The maximum embedded state size, compression policy, chunking strategy and failure behavior are unresolved until host prototypes and reference tests are complete.

## 8. Collect and relink

- Collect copies referenced library assets into a user-selected project/package location and updates references only after successful copy and verification.
- Relink supports exact content-hash match first, then explicit user-selected replacement.
- Replacements with mismatched content hashes require explicit confirmation; do not silently substitute a different recording.
- Collect/relink must not mutate the original library source by default.
- Plugin state must remain portable when all referenced assets have been collected to a known package location.

## 9. Freeze integration (approved product behavior — DEC-042)

- Freeze creates an immutable snapshot of the layer's current valid source region, respecting active crop/loop bounds and excluding uncommitted frames.
- The source snapshot is prepared off-thread and published through the bounded structural-command path; the audio callback only switches to a ready snapshot.
- Existing grains/voices, new notes, sequencer, MIDI, modulation, envelopes, shared transport and layer FX continue normally.
- Independent recording continues at its defined tap. Freeze does not start/stop the recorder or mutate the frozen snapshot.
- Thaw crossfades back to the prior live/rolling source. Static library samples report already static or disable redundant capture.
- Global Freeze transitions all four layers at one logical boundary; partial failure must be visible.
- Freeze state and snapshot audio persist in plugin state; the snapshot is treated as an unsaved recording buffer and embedded under DEC-025.
- Memory exhaustion or source loss must fail safely, preserve the prior valid source and report status. All Sound Off does not thaw.

Numeric capture duration, memory budget, copy strategy, crossfade duration and snapshot buffer admission remain engineering decisions for Phase 3/5.

## 10. Open technical decisions

| Question | Status |
|---|---|
| Source-rate playback vs SRC | Q-T04 — proposal A preferred; measurement required |
| R1 resident memory/admission budget | Open — Phase 3/5 measurement |
| Decoder and malformed-file policy | Q-T16 — validate AudioToolbox coverage |
| State container format and migration | Q-T15 — JSON manifest + binary chunks proposed |
| Maximum embedded recording size | Open — host API prototype and test |
| Host state callback threading/size constraints | Open — AU/VST3 prototype |
| Exact per-layer/master record tap | Open — signal-flow decision |
| Recording buffer capacity and overflow | Open — measured and bounded |
| Freeze interaction with recording | **Answered — DEC-042**; recorder continues at its defined tap, frozen snapshot remains immutable. Exact tap definition remains open in signal-flow design |

**Status:** Proposed engineering specification aligned with approved product policies DEC-025 and DEC-042. It does not approve a memory ceiling or a serialization container.


## 6.1 Recommended recording tap contract

| Recorder | Tap | Includes | Excludes |
|---|---|---|---|
| Layer 1–4 | Post-layer FX, pre-layer fader | Engine output, live input routed through the layer, layer FX tails | Layer fader, mute and solo state |
| Stereo master | Post-layer mix, pre-host output | Four layer contributions after fader/mute/solo, including their routed live input | Host-side processing after plugin output |

The tap is fixed for a recording take and stored in recording metadata. The UI must label the source clearly (Layer Stem or Master Mix). A muted layer remains recordable as a stem; the master naturally reflects the mute/solo mix. Recording continues independently during Freeze per DEC-042.

Validation must inject distinct test signals into each layer, toggle fader/mute/solo during capture, and compare captured stems/master against the corresponding graph nodes. No tap may read an uncommitted recording region.

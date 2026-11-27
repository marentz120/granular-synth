# Sample and State Lifecycle Specification

**Version:** 0.5 — host render-capacity and scratch admission added  
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
2. Reserve the expected memory admission for decode output, guard regions, metadata and any required conversion before committing to the load. Reservation is accounting, not an allocation on the audio thread; if reservation fails, reject before disturbing the active asset.
3. Decode and validate off-thread; reject unsupported/malformed input without changing the active sample.
4. Convert to the internal sample representation and prepare guard regions and required metadata.
5. Build waveform overview progressively; optional analyses (onsets, centroid, zero crossings) are separate cancellable jobs and must not hold the audio publication slot.
6. Assign a monotonically increasing request generation per asset slot. A newer request may supersede an older prepared-but-not-published request; a stale completion must not replace a newer accepted request.
7. Publish the completed immutable prepared sample to the audio thread using the bounded structural-command protocol. Ownership transfers only after successful publication; if publication fails, the producer retains responsibility for the prepared object and its reservation.
8. Swap at a documented safe point. The active handle changes atomically; any existing voice, grain or sampler head that still reads the old asset must retain its own valid lifetime handle until it has stopped referencing it.
9. Retire the old runtime handle off-thread only after all audio readers have relinquished it. Retirement-queue saturation must retain ownership in a preallocated bounded fallback and must never drop, destroy or block on the audio thread; the required fallback capacity and saturation proof remain engineering gates.
10. Release unused reservation and prepared resources on the non-RT owner path after cancellation, stale-generation rejection or failed publication.

Missing library files do not crash the host. User can relink by content hash or explicit file selection. A failed replacement leaves the currently active sample intact. The system must report whether a request was rejected, superseded, prepared, published or activated; “load complete” must not be reported merely because decoding finished.

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

If all four six-minute sources are each copied into a distinct full-length Freeze snapshot, source plus snapshot audio alone can reach approximately 1.106 GB decimal (1,105,920,000 bytes, about 1.03 GiB). This is a capacity-planning scenario, not an approved memory budget; it excludes active recording headroom and all other runtime/state overhead. Immutable library samples should not incur a redundant copy.

The runtime must maintain separate accounting for:
- prepared library samples;
- active and pending sample replacements;
- live recording buffers;
- committed but not exported recording buffers;
- Freeze snapshots and the source-retention/rotation headroom needed while snapshots are prepared;
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

### 5.1 Admission and reservation transaction

Memory admission must account for peak coexistence, not only the steady-state size of the final asset. A replacement can temporarily require the active old asset, decoded/prepared new asset, guard/analysis data, voices or grains still reading the old asset, and retirement backlog at the same time.

Use a non-RT reservation lifecycle:
1. Estimate and reserve the operation's bounded worst-case payload categories before decode/copy begins; label estimates as estimates until exact sizes are known.
2. Reconcile the reservation against actual decoded frame/channel/format sizes before publication. If the actual requirement exceeds the reservation or safe budget, reject the prepared result and preserve the active asset.
3. Keep the reservation charged while the payload is pending publication, active, referenced by voices/grains, or awaiting retirement. Do not release accounting at pointer swap if readers still retain the old asset.
4. On cancellation, supersession, decode failure or publication rejection, release the reservation only after the corresponding non-RT owner has disposed of the prepared payload.
5. Never evict an asset that is active, referenced by a voice/grain, retained by Freeze, or required by an unsaved recording/state snapshot.
6. If retirement backlog prevents safe admission, reject/defer the new operation with visible status; do not reuse memory whose lifetime has not ended.

The reservation ledger must distinguish committed bytes from reserved-but-not-yet-materialized bytes and must avoid double-counting shared immutable library assets referenced by multiple layers. It must also distinguish logical ownership from physical copies: a shared handle is not a second sample payload, while a Freeze copy or serialization staging buffer is.

No numeric headroom, warning threshold, maximum concurrent replacement count or fallback-slot count is set here. These require measured workload evidence and a bounded concurrency model before implementation.

### 5.2 Render-time block capacity and transient scratch admission

Render-time scratch is a separate ledger category from resident samples and recordings. It is sized once per prepared host/device configuration from the negotiated maximum block, not from the actual callback length.

- AU capacity is `maximumFramesToRender`, established before render-resource allocation.
- VST3 capacity is `ProcessSetup::maxSamplesPerBlock`, established while inactive.
- Standalone capacity is the configured device callback maximum.
- Let `B` be prepared capacity and `N` the actual callback frame count. Require `N <= B`; no render callback may grow, shrink, allocate or free scratch.
- A call above `B` is a bounded adapter/configuration failure. Do not index beyond prepared storage or attempt an in-callback resize; exact host error/status mapping remains adapter-specific.
- VST3 zero-bus parameter flush is not an audio render and must not access render scratch or advance audio/transport.

For float32 scratch, the payload estimate is `B × 4 × C` bytes, where `C` is the number of channels actually materialized in engine-owned scratch. If all five stereo output buses (master + four layer stems) were copied to scratch, that component would be `40 × B` bytes. One additional stereo input staging copy would add `8 × B` bytes. These are conditional arithmetic examples, not a requirement to copy host buffers.

Host input/output buffers are borrowed for the callback and are not engine-owned scratch. Count only actual internal working planes, staging copies, alignment/guard space and algorithm-specific workspaces. A read-only live-input view may be shared/fanned out to multiple layer processors without duplicating its audio payload. Physical input bus count/channel layout is negotiated by the adapter; the one-stereo-input host test configuration is not a product-level cap (DEC-020).

Use checked wide byte arithmetic (candidate `uint64_t`) and validate that each allocation size fits `size_t` before allocation. `float32` sample width and a 32-bit `FrameCount` do not imply a 32-bit process. The current macOS architecture investigation concerns x86_64/arm64; Q-T03 remains open and no 32-bit macOS target is approved or implied. Long recording/frame totals and serialized byte lengths remain 64-bit.

All scratch is prepared/reserved off the render thread and belongs to a lifecycle generation keyed by sample rate, maximum frames, input/output bus arrangement and any format that changes buffer shape. Reconfiguration follows the AU/VST3/Standalone lifecycle in the Plugin Component and Bus Specification. Publish only a complete prepared generation; retain the old generation until no callback can reference it, then retire it off-thread.

See `PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md` for equations, conditional examples and planned validation cases. Numeric headroom, block-size range, maximum input channels and memory budgets remain open; no benchmark or runtime measurement was performed.

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

Recommended architecture defaults (proposal pending owner review and host validation): per-layer capture is post-FX and pre-layer-fader, independent of layer fader/mute/solo; a muted or solo-isolated layer continues recording its own stem when armed. Master capture is the final stereo master mix after layer faders/mute/solo and before host output, including both the wet layer-routed live input and the required parallel dry live-input path. The parallel dry live-input path to the master is required by GS-ARC-007; the master recording tap must include it. These tap positions are engineering recommendations, not new approved product requirements. Exact start modes, stop semantics and loop/scroll behavior remain to be specified and tested.

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

**Capture and save**
- State save operates on an immutable, internally consistent snapshot; it never reads mutable audio-writer memory directly and never blocks the audio thread.
- The capture descriptor records a state generation and the committed-frame/source-generation tuple for each unsaved recording and Freeze snapshot. A snapshot must not combine metadata from one recording generation with audio from another.
- Recording capture uses the same bounded retention/lease principle as Freeze: committed frames are not automatically immutable while a rolling ring continues to write. The capture coordinator reserves retention or copies into a pre-reserved immutable staging region off-thread; if it cannot guarantee a stable read, capture fails before returning a snapshot. The audio writer never waits for serialization.
- All unsaved recording payloads required by DEC-025 are part of one save manifest transaction. The serializer must not mark the state complete if any required recording or Freeze chunk is absent, truncated or checksum-invalid.
- Host state callbacks may impose synchronous API constraints. The adapter must follow the specific host callback contract; it may use a previously prepared immutable snapshot or a bounded capture/copy path, but must not wait for disk I/O, an audio-thread acknowledgement or global audio quiescence. If the required bounded snapshot is not ready within the host's permitted contract, return a documented save failure rather than omit audio.
- Serialize large audio payloads as binary chunks; do not base64-encode them into JSON. Chunk order, lengths, format, sample rate, channel count, frame range, source generation and checksum are recorded in the manifest.
- Write to a temporary/incomplete container and publish the final state only after all chunks and manifest have been written and verified. Use an atomic finalization mechanism where the host/container supports it; otherwise use a recoverable commit marker/version protocol. An interrupted save must leave either the prior valid state or a clearly detectable incomplete candidate, never a plausible-looking partial success.
- If embedded recordings exceed the defined size/time budget or the host cannot accept the payload, do not silently omit them. Return a documented failure or use a verified host-supported chunking strategy; notify the user and preserve the in-memory recording.

**Restore**
- Inspect container bounds, manifest schema/version, chunk lengths and checksums before allocating large payloads. Reject impossible lengths, overflow, unsupported formats and malformed manifests safely.
- Migrate metadata and prepare embedded recordings, Freeze snapshots and external sample references off-thread. Apply memory admission before allocating/decoding the full restore payload.
- Missing external library files do not invalidate unrelated patch/sequencer state; restore the rest with an explicit relink-required status and stable unresolved asset identity.
- Corrupt or unsupported embedded audio is never silently replaced by silence or omitted. Mark the affected recording/Freeze asset unavailable with explicit recovery status while preserving unrelated valid patch state where safe.
- Prepare the complete candidate runtime graph off-thread. Do not mutate the active project incrementally while decoding. Publish a validated restore transaction at one documented safe point; if preparation/publication fails, keep the previous active runtime intact.
- After successful publication, retire the previous runtime and release restore staging resources through the non-RT ownership path.

The maximum embedded state size, compression policy, chunking strategy, capture deadline, host-specific synchronous callback handling and recovery UI remain unresolved until host prototypes and reference tests are complete.

## 8. Collect and relink

- Collect copies referenced library assets into a user-selected project/package location and updates references only after successful copy and verification.
- Relink supports exact content-hash match first, then explicit user-selected replacement.
- Replacements with mismatched content hashes require explicit confirmation; do not silently substitute a different recording.
- Collect/relink must not mutate the original library source by default.
- Plugin state must remain portable when all referenced assets have been collected to a known package location.

## 9. Freeze integration (approved product behavior — DEC-042)

- Freeze creates an immutable snapshot of the layer's current valid source region, respecting active crop/loop bounds and excluding uncommitted frames.
- For a rolling source, committed frames may still be overwritten when the ring wraps. Before off-thread copying, the system must reserve a bounded retention lease (for example, pinned preallocated pages or segment rotation) that prevents the selected region from being overwritten. The exact mechanism and spare-capacity requirement remain engineering work.
- Recording remains independently armed and continues into a separate writable region while the lease is held. The audio thread must never wait, copy a large region or write into leased pages. If retention cannot be guaranteed, Freeze fails before switching sources; the previous source and recorder state remain unchanged and the failure is visible.
- The source snapshot is prepared off-thread and published through the bounded structural-command path; the audio callback only switches to a ready snapshot.
- Existing grains/voices, new notes, sequencer, MIDI, modulation, envelopes, shared transport and layer FX continue normally.
- Independent recording continues at its defined tap. Freeze does not start/stop the recorder or mutate the frozen snapshot.
- Thaw crossfades back to the prior live/rolling source. Static library samples report already static or disable redundant capture.
- Global Freeze is prepared and published as one bounded transaction with four per-layer outcomes, not four independent queue commands. All ready layers switch at the same logical boundary; failed layers remain on their previous source, and partial failure is visible. Failure to publish the transaction leaves all layers unchanged. **Owner-review item:** the proposed partial-preparation behavior lets ready layers switch together while failed layers remain unchanged; confirm this interpretation versus an all-or-nothing global operation before implementation.
- Freeze state and snapshot audio persist in plugin state; the snapshot is treated as an unsaved recording buffer and embedded under DEC-025.
- Memory exhaustion or source loss must fail safely, preserve the prior valid source and report status. All Sound Off does not thaw.

Numeric capture duration, memory budget, copy strategy, crossfade duration and snapshot buffer admission remain engineering decisions for Phase 3/5.

## 10. Open technical decisions

| Question | Status |
|---|---|
| Source-rate playback vs SRC | Q-T04 — proposal A preferred; measurement required |
| R1 resident memory/admission budget | Open — Phase 3/5 measurement; include simultaneous source, snapshot, recording and retention headroom |
| Rolling-source snapshot retention | Proposed bounded lease/segment rotation; exact mechanism and spare capacity require architecture review and Phase 5 stress validation |
| Decoder and malformed-file policy | Q-T16 — validate AudioToolbox coverage |
| State container format and migration | Q-T15 — JSON manifest + binary chunks proposed |
| Maximum embedded recording size | Open — host API prototype and test |
| Host state callback threading/size constraints | Open — AU/VST3 prototype |
| Exact per-layer/master record tap | Proposed: post-FX/pre-layer-fader for stems; final master pre-host-output for master capture — owner/host validation pending |
| Recording buffer capacity and overflow | Open — measured and bounded |
| Freeze interaction with recording | **Answered — DEC-042**; recorder continues independently at the proposed recording tap, while the frozen source snapshot remains immutable. Validate implementation tap equivalence in host/Standalone tests |

**Status:** Proposed engineering specification aligned with approved product policies DEC-025 and DEC-042. It does not approve a memory ceiling or a serialization container.


## 6.1 Recommended recording tap contract

| Recorder | Tap | Includes | Excludes |
|---|---|---|---|
| Layer 1–4 | Post-layer FX, pre-layer fader | Engine output, live input routed through the layer, layer FX tails | Layer fader, mute and solo state |
| Stereo master | Post-layer mix, pre-host output | Four layer contributions after fader/mute/solo, including their routed live input | Host-side processing after plugin output |

The tap is fixed for a recording take and stored in recording metadata. The UI must label the source clearly (Layer Stem or Master Mix). A muted layer remains recordable as a stem; the master naturally reflects the mute/solo mix. Recording continues independently during Freeze per DEC-042.

Validation must inject distinct test signals into each layer, toggle fader/mute/solo during capture, and compare captured stems/master against the corresponding graph nodes. No tap may read an uncommitted recording region.

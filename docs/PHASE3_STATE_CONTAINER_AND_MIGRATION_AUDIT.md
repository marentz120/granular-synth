# Phase 3 State Container and Migration Audit

**Version:** 0.1 — host-stream semantics, container framing and schema migration  
**Status:** Proposed technical contract. Q-T15 remains PROPOSED; no final container, codec, size limit or migration-retention policy is approved.  
**Baseline:** DEC-025/042, Q-T15, Q-P09, GS-NFR-001, SAMPLE_AND_STATE_LIFECYCLE_SPECIFICATION.md §7, THREADING_AND_OWNERSHIP_SPECIFICATION.md §§11–14.  
**Scope:** Documentation and API research only. No implementation, host prototype, benchmark or runtime measurement was performed.

## 1. Executive findings

The current architecture already proposes a versioned JSON manifest plus binary audio chunks. This audit makes that proposal implementable as a contract without prematurely choosing a byte-level ABI.

The most important distinction is between three layers:

1. **Canonical engine state:** an immutable, framework-independent snapshot of product parameters, layer/patch state, asset references and required embedded audio.
2. **Container encoding:** a versioned manifest plus bounded binary payload chunks, integrity metadata and a completion marker.
3. **Host transport:** the AU state dictionary or VST3 state stream used by a DAW to carry the container. The host transport is not itself the product's file format.

A standalone user-owned preset/project file is a fourth, separate persistence surface. It is the only surface where the application can own temporary-file creation and final file replacement. A plug-in cannot assume that a DAW-owned state stream supports file rename, truncation, rollback, durable flush or an atomic save transaction.

The recommendation remains JSON metadata plus binary audio chunks. Audio bytes must not be base64-expanded inside JSON. The exact embedded payload encoding (canonical PCM versus a self-contained audio container), compression, chunk size, byte order, checksum algorithm and maximum state size remain open.

## 2. Product policy and scope boundaries

The following are fixed and must not be weakened by the container design:

- DEC-025: DAW plug-in document state embeds unsaved recording buffers. Existing library samples remain external references with relink/collect support.
- DEC-042: Freeze snapshots persist with the project state. The immutable snapshot audio and its metadata must refer to the same source generation and frame range.
- Missing external library assets may produce a relink-required state; they do not justify dropping unrelated valid patch/sequencer state.
- Required embedded recording or Freeze audio may not be silently omitted, truncated or replaced by silence.
- Active MIDI note IDs, AU member-channel leases, VST3 note IDs, NoteInstanceIds, pending host events and Transport snapshots are transient and are not serialized.
- Host-negotiated stream formats, active bus state and render-resource capacities are not persistent product parameters; they are re-established through the host lifecycle and validated separately.

**Scope distinction still open:** DEC-025 explicitly requires embedding for DAW document state. It does not separately define whether a user-created AU/VST3 preset must include unsaved recording and Freeze payloads. Do not silently equate a user preset with a DAW document. Record the scope in the container metadata and resolve preset inclusion before implementing user-preset export/import.

## 3. API evidence and consequences

### 3.1 Audio Units

Apple documents AUAudioUnit.fullState as a persistable snapshot suitable for a user preset and fullStateForDocument as a persistable snapshot suitable for a user document. The fullState documentation says the unit should not persist transitory properties such as stream formats, while saving and restoring other properties. Both properties are dictionary-shaped ([Apple fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate), [AUAudioUnit](https://developer.apple.com/documentation/audiotoolbox/auaudiounit)).

Consequences:

- The AU adapter must keep user-preset and document-state scope distinct.
- Do not serialize negotiated bus/stream formats as if they were patch settings.
- A dictionary-shaped API is not evidence of an unlimited, seekable binary stream. Whether a property-list-compatible binary Data payload of the required size is accepted and preserved by each target host must be verified in an AU host prototype.
- If the AU host materializes the full payload in memory, that full materialization is part of peak state-memory admission. Do not describe AU state as streaming unless observed and documented for the target host.

### 3.2 VST3

Steinberg separates processor/component state from controller state. The documented restore sequence passes component state to the processor first, then to IEditController.setComponentState, then restores controller-specific state. The controller synchronizes its parameters from component state and must not emit duplicate host parameter-change callbacks during that synchronization ([VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html)).

IBStream is a binary read/write stream. Its read/write byte-count arguments are int32, while seek/tell positions are int64; read/write can report the number of bytes actually transferred ([IBStream](https://steinbergmedia.github.io/vst3_doc/base/classSteinberg_1_1IBStream.html)). Therefore:

- Large logical chunks must be transferred through multiple bounded read/write calls; never narrow a 64-bit payload length directly to int32.
- The reader must handle short reads, short writes, end-of-stream and zero-progress results explicitly.
- Do not require seek or back-patching for the core container, even though IBStream exposes seek/tell.
- Component state owns the engine/document payload. Controller state should contain only controller/UI-specific state that is not already in the component state.
- IStreamAttributes is optional host-provided metadata. It can distinguish contexts such as Project, Default and TrackPreset, but it must not be required to parse or validate the container ([IStreamAttributes](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IStreamAttributes.html), [StateType](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/group__stateType.html)).

These API facts do not prove a particular host's maximum state size, synchronous callback deadline or willingness to persist a large embedded audio payload.

## 4. Proposed container contract (not a frozen binary ABI)

### 4.1 Envelope

The container should be self-identifying and independently versioned from the product version. The proposed logical envelope is:

1. Fixed header: magic/signature, envelope version, flags, manifest length, chunk count and total logical length.
2. UTF-8 manifest: schema version, minimum reader version, writer/product version, state scope, required feature flags, canonical patch/layer state, external references and a complete chunk directory.
3. Binary payload chunks: one logical chunk per embedded recording, Freeze snapshot or other explicitly approved embedded asset.
4. Completion footer: a fixed marker and integrity summary that is present only after every required chunk has been emitted.

The header/manifest must contain enough lengths and identifiers to parse sequentially. The core reader must not require random access or a seekable stream. The exact field widths, byte order, alignment and footer encoding remain open until the host adapter prototypes and compatibility tests are complete.

### 4.2 Manifest fields

Candidate required fields:

- container/envelope version and product schema version;
- minimum reader version and writer version;
- state scope (document, user preset or standalone project);
- required feature identifiers, so an older reader can reject unsupported required behavior instead of silently losing it;
- stable parameter IDs and values, layer/engine configuration and patch metadata;
- stable external asset ID, content hash, original source URI/path and relink metadata;
- for every embedded chunk: stable chunk ID, semantic type, required/optional flag, stored length, decoded length, encoding/codec identifier, integrity algorithm and digest;
- audio metadata where applicable: sample format, sample rate, channel count, frame count, frame range and source-generation identity;
- transaction/completion metadata.

Do not store raw pointers, host handles, active MIDI identity, process-local paths as the sole asset identity, or transient render configuration.

### 4.3 Chunk semantics

- A chunk is a logical asset payload. Transport slices used to call IBStream.read/write are smaller bounded transfers and do not change the logical chunk identity.
- Each chunk has an explicit stored length and decoded length. All arithmetic uses checked 64-bit values before any conversion to size_t or host API argument types.
- Integrity metadata identifies its algorithm and the exact byte domain being hashed. Container-byte integrity and external asset content identity must not be conflated.
- A required chunk that is missing, duplicated, truncated, length-mismatched or integrity-invalid invalidates the document-state transaction. Optional chunks may be skipped only when the manifest explicitly marks them optional and no required feature depends on them.
- Unknown optional manifest fields may be ignored. Unknown required feature identifiers or required chunk types must fail safely with an unsupported-version/feature status.
- Audio payload encoding and compression are not selected here. Do not assume compression is beneficial, bounded or real-time safe. Any compression/decompression occurs off the render thread and has explicit workspace admission.

### 4.4 Bounded sequential stream behavior

The writer should be able to emit header, manifest, chunks and footer in order without seeking backward. If checksums are placed in the manifest, the immutable snapshot must be hashed before the host callback starts; otherwise a footer/index design must carry the final digest. This choice remains open.

For VST3, split transfers so each call fits the signed int32 byte-count argument and a separately configured bounded transfer slice. A short successful transfer is not equivalent to a complete write. The adapter must continue only while progress is made and must stop on error or zero progress. On restore, the reader must distinguish clean end-of-container from premature end-of-stream.

For AU, the adapter must validate the actual host-persisted representation and size behavior. The API's dictionary shape does not by itself prove that arbitrary Data values, large payloads or incremental streaming are supported by every target host.

## 5. Save transaction and durability semantics

### 5.1 Host-owned AU/VST3 state

The host owns the destination project/preset transaction. The plug-in cannot promise atomic rename, fsync, rollback or preservation of the host's previous project file from inside fullState/fullStateForDocument or getState.

The plug-in's enforceable guarantees are narrower:

- Capture one immutable, generation-consistent state snapshot.
- Validate all required recording/Freeze leases and chunk metadata before reporting a complete snapshot.
- Emit a self-validating container with explicit lengths and a completion footer/marker.
- Return a failure when a required payload cannot be captured or the host stream rejects/short-writes it.
- Reject a container with a missing completion marker, invalid length or invalid digest during restore.
- Never reinterpret a partial host stream as a valid reduced state.

The host's own project-save durability and previous-file recovery remain host responsibilities and must be tested per host. A plug-in must not claim that a failed host save preserves the previous project unless the host documents and the prototype verifies that behavior.

### 5.2 Plug-in-owned standalone preset/project file

For a file written by the standalone application, the application can own a temporary-file transaction: write a sibling temporary candidate, flush/close it, verify the complete container, then replace the target using the platform's supported same-volume atomic replacement mechanism. On interruption, retain the previous valid file and leave any incomplete candidate distinguishable. Exact durability flags, directory synchronization and replacement semantics are platform-specific implementation work and are not frozen here.

### 5.3 Restore transaction

Restore remains two-phase:

1. Read and validate the envelope and manifest without mutating the active runtime.
2. Validate lengths, chunk table, required features and integrity metadata.
3. Run deterministic schema migrations on metadata.
4. Reserve peak memory for decoded chunks, staging, candidate graph and retained old graph.
5. Decode/prepare required embedded audio and resolve external references off-thread.
6. Build a complete immutable candidate runtime.
7. Publish one restore transaction at the documented safe point.
8. Only after successful publication, retire the previous runtime through the non-RT owner path.

Any failure before commit leaves the previous active runtime authoritative. AR-06 still controls the audible policy for old voices at the commit boundary; this audit does not choose that policy.

## 6. Schema versioning and migration

The reader must distinguish container-envelope version from product-state schema version. The envelope governs framing; the schema governs the meaning of product metadata.

Proposed migration rules:

- Every schema has an explicit current version and minimum reader version.
- Migrations are deterministic, ordered and performed off the render thread.
- Each migration transforms a validated metadata representation; it does not mutate the source stream or the active runtime.
- Migration steps declare the source and destination schema versions and required feature changes.
- The original state remains available until the migrated candidate has passed validation and restore admission.
- Unknown optional fields may be preserved or ignored according to a documented forward-compatibility rule; unknown required features fail closed.
- A state written by a newer unsupported major schema is rejected without modifying the current project. Do not attempt a best-effort downgrade.
- Audio payload conversion is a separate off-thread operation with its own memory reservation; a metadata migration must not silently rewrite or discard audio.
- Migration failure, checksum failure or insufficient memory leaves the prior active state unchanged.

No migration retention window, downgrade policy, schema-numbering convention or compatibility promise has been approved.

## 7. Memory and resource admission

The save/restore peak is not just the final embedded audio size. Count physical payloads and copies once, including:

- immutable source recording/Freeze snapshots retained for the save;
- manifest and chunk-directory storage;
- any AU dictionary/Data materialization;
- bounded VST3 or file I/O transfer buffers;
- checksum and compression workspace, if selected;
- restore decode/conversion staging;
- candidate runtime graph and prepared audio assets;
- old active graph and assets retained until commit/readers retire;
- pending retirement backlog.

A planning equation is:

M_state_peak = M_old_active + M_snapshot_payloads + M_candidate_payloads + M_manifest_directory + M_transport_staging + M_codec_workspace + M_digest_workspace + M_retirement

Shared immutable external library assets are counted once. Borrowed host stream memory is not counted as engine-owned unless the adapter copies/materializes it. If AU persistence materializes the complete payload as Data, count the complete materialized payload; if VST3 is truly streamed, count only the bounded transfer buffer plus retained snapshot. Do not assume either behavior before host validation.

No maximum state size, maximum recording duration beyond approved product requirements, transfer-slice size, compression workspace, memory warning threshold or process-memory budget is selected. DEC-025 cannot be weakened to fit an unverified host assumption; an inability to persist required audio is a compatibility blocker.

## 8. Failure and recovery matrix

| Failure | Required outcome |
|---|---|
| Invalid magic/envelope version | Reject before large allocation; report unsupported/corrupt state |
| Manifest length/count overflow | Reject before allocation or pointer arithmetic |
| Duplicate chunk ID or overlapping/out-of-range extents | Reject container |
| Required chunk missing/truncated/short-read | Reject restore; do not substitute silence |
| Digest mismatch | Reject affected required state; preserve previous active runtime |
| Missing external sample | Restore unrelated state with explicit relink-required status |
| Unknown optional field/chunk | Skip only under explicit optional semantics |
| Unknown required feature/chunk | Reject safely as unsupported |
| Migration unavailable/fails | Preserve prior active runtime; report version/recovery status |
| Memory admission fails | Preserve prior active runtime and source file; do not partially publish |
| Host stream short-write/error | Return failure; never report save complete |
| Missing completion footer | Treat as incomplete, never as a valid reduced state |
| Standalone save interruption | Keep previous valid target; incomplete temp candidate is not promoted |

## 9. Required Phase 5 validation fixtures

The backlog should include deterministic fixtures for:

- envelope magic/version and truncated header;
- manifest bounds, integer overflow, duplicate keys/IDs and unsupported required features;
- chunk directory duplicate IDs, overlap, out-of-range extents and total-length mismatch;
- partial VST3 reads/writes, zero progress, stream error and transfer splitting at int32 boundaries;
- missing footer, interrupted write and corrupted digest;
- unknown optional field versus unknown required feature;
- each supported schema migration path and an unsupported future schema;
- migration failure preserving the active runtime;
- AU fullState versus fullStateForDocument scope and actual binary payload round-trip;
- VST3 component/controller save/restore ordering and no duplicate parameter writes;
- host stream failure versus standalone temporary-file atomic replacement;
- peak memory admission for old graph + snapshots + candidate graph + transfer/decode staging + retirement;
- required unsaved recording and Freeze payload integrity after save/reopen;
- external asset missing/relink without loss of unrelated patch state;
- state callback thread/context and elapsed-time evidence on each approved host.

These are planned tests only. No test IDs, thresholds, host pass results or benchmarks are claimed.

## 10. Open decisions and implementation gates

| Gate | Still open |
|---|---|
| Q-T15 | Final container framing, field widths, byte order, checksum algorithm, chunk boundaries, compression, schema numbering and migration retention |
| Q-T15 / Q-P09 | AU dictionary payload representation and size behavior; VST3 stream behavior and host size/deadline constraints |
| Q-T15 | Whether user presets embed unsaved recordings/Freeze, distinct from DAW document state |
| DEC-025 | Maximum embedded payload and behavior when host limits are reached; no silent omission |
| DEC-042 | Freeze snapshot chunk semantics and memory admission; approved snapshot behavior remains unchanged |
| AR-06 | Audible treatment of old voices at state-restore commit |
| Q-T07 | Numeric memory/CPU budget remains measurement-gated; no benchmark is authorized here |

The proposed JSON-manifest/binary-chunk architecture is a recommendation for owner/engineering review, not an approved irreversible format decision.

## 11. Primary API references

- Apple, AUAudioUnit.fullState: https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate
- Apple, AUAudioUnit: https://developer.apple.com/documentation/audiotoolbox/auaudiounit
- Steinberg, VST3 Persistence: https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html
- Steinberg, IBStream: https://steinbergmedia.github.io/vst3_doc/base/classSteinberg_1_1IBStream.html
- Steinberg, IStreamAttributes: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IStreamAttributes.html
- Steinberg, StateType: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/group__stateType.html

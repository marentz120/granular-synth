# Phase 3 State I/O Scheduling and Peak Memory Admission Audit

**Version:** 0.2 — cross-host callback context, coherent capture boundaries and failure signalling  
**Status:** Proposed engineering contract; Q-T15 and Q-P09 remain open. No numeric memory budget, host callback deadline, concurrency limit or container ABI is approved.  
**Baseline:** DEC-025, DEC-042, GS-NFR-001, GS-NFR-008, Q-T15, Q-P09, AR-06; `SAMPLE_AND_STATE_LIFECYCLE_SPECIFICATION.md` §§7/9; `THREADING_AND_OWNERSHIP_SPECIFICATION.md` §§4/5/14; `PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md` §9.  
**Scope:** Documentation and test design only. No implementation, host prototype, benchmark or runtime measurement was performed.

## 1. Executive findings

The existing state-container proposal defines what must be serialized and how the container is validated. The remaining scheduling risk is the interval between a live rolling recorder, a stable snapshot, and a synchronous AU/VST3 host-state callback.

This audit makes five boundaries explicit:

1. **Capture freshness:** a state snapshot is identified by a state generation and a per-recording committed-frame watermark. A serializer must not report success using a stale snapshot that omits frames already committed before the capture boundary.
2. **Retention:** committed frames in a rolling ring are not immutable merely because their frame count is committed. Every page/range read off-thread must be protected against overwrite for the entire read.
3. **Scheduling:** the render thread publishes bounded commit/capture metadata only. It never waits for a serializer, copies a long recording, hashes/compresses audio, performs stream or file I/O, or allocates staging.
4. **Host callbacks:** AU dictionary state and VST3 IBStream have different transport behavior. A host callback may be synchronous and its execution context is host-specific; the adapter cannot assume an asynchronous disk worker or an audio-thread acknowledgement is available.
5. **Admission:** save, Freeze, recording retention and restore use one physical-memory accounting model. Concurrent jobs must reserve their combined peak before they retain or materialize additional payloads.

The contract preserves DEC-025 (required unsaved recording payloads in DAW document state) and DEC-042 (Source Snapshot Freeze). It does not resolve user-preset payload scope, AR-06, or the owner-gated mixed-layer Global Freeze preparation-failure policy.

## 2. Ownership and scheduling boundaries

| Responsibility | Owner | Permitted work | Prohibited work |
|---|---|---|---|
| Recording writer | Audio/render domain | Write the current preallocated region; commit a completed block/page; publish a bounded commit record | Wait for a lease, copy a long range, serialize, hash, compress, perform I/O or allocate |
| Capture coordinator | Non-render state coordinator | Validate generations/watermarks; acquire retention leases; assemble an immutable descriptor; reserve staging | Mutate the live writer cursor or wait for render completion |
| Snapshot lease owner | State-capture transaction | Retain exact source-generation/page ranges until all readers finish | Release a page while a serializer or copy worker still reads it |
| Serializer | Non-render serialization worker or permitted host callback path | Emit manifest and required chunks; validate transfer progress; compute integrity data where admitted | Read mutable recording memory or claim completion after a partial transfer |
| Host adapter | AU/VST3 boundary | Translate the canonical container to/from the host-owned transport | Assume host streaming, host durability, unlimited payload size or a universal callback thread |
| Memory-admission ledger | Non-render resource coordinator | Reserve/release physical bytes and retention capacity transactionally | Double-count shared storage or release reservations while an owner still holds the payload |

The exact lock-free page-state primitive and queue topology remain implementation choices. Any chosen primitive must prevent the writer from reusing a leased page without making the writer wait.

## 3. Capture boundary and freshness contract

### 3.1 Committed-frame watermark

A capture descriptor records, for every required recording source:

- stable recording/source identifier;
- source generation;
- latest fully committed frame watermark included in the snapshot;
- exact frame range and channel/sample metadata;
- page or segment identities plus their lease generations;
- required/optional chunk classification;
- state generation and capture epoch.

The watermark is the latest complete recording block/page boundary published as committed before the snapshot descriptor is published. An in-flight, partially written block is not included. This provides a deterministic boundary without pausing or waiting for the audio callback.

Freeze chunks use the already immutable Source Snapshot generation and frame range. Their metadata and audio must identify the same source generation; a Freeze snapshot is not recaptured from a mutable live ring during save.

### 3.2 Stale-snapshot rejection

A prepared snapshot is eligible for a host save only if its per-source watermarks match the latest committed-watermark publication observed at the host callback's capture boundary. If the ready descriptor is behind that publication, the adapter must not silently return the older snapshot as a successful DEC-025 document save.

The adapter may use a bounded callback-time capture only where the actual host callback contract permits it and the required page leases are already obtainable without waiting for the render thread. Otherwise it must return a bounded save failure and preserve the in-memory recording. Host prototypes must establish whether each target can meet this rule.

No product-level promise about sub-block save timing is introduced. The chosen block/page commit boundary is an engineering proposal; the host-specific freshness and callback behavior remain Q-T15/Q-P09 evidence gates.

### 3.3 Required-payload transaction

All required unsaved recording and Freeze chunks for one DAW document state form one save transaction. If any required source generation, frame range, lease, chunk, or integrity result is unavailable, the container is not marked complete. The serializer must not produce a reduced-success state that silently omits one layer or one required audio asset.

This document-state rule does not decide whether a user-created preset includes those payloads. That separate Q-T15 scope question remains open.

## 4. Save transaction state machine

The proposed bounded state machine is:

`Requested → Preflighted → SnapshotReady → Serializing → Complete`

Failure and cancellation may terminate any non-terminal state:

`Preflighted | SnapshotReady | Serializing → Failed | CancelPending → Cancelled`

| State | Required action | Exit condition |
|---|---|---|
| Requested | Identify document/preset scope and capture generation | Scope is known; request enters bounded admission |
| Preflighted | Reserve snapshot retention capacity, staging, and known host-transport requirements | All required resources are reserved, or request fails without partial retention |
| SnapshotReady | Publish one immutable descriptor with all required chunk leases and watermarks | Descriptor passes generation/freshness validation |
| Serializing | Emit header/manifest/chunks/footer through the selected transport; check every transfer result | Every required byte and completion marker is accepted |
| Complete | Report success and release leases after the final reader has stopped | No serializer retains the snapshot |
| Failed | Report explicit failure; preserve active recording and prior valid state | All acquired resources are released by their non-render owners |
| CancelPending | Stop scheduling further chunks and wait for active non-render readers to stop | Leases are released only after reader quiescence |
| Cancelled | Record terminal cancellation status | No pending reader or retained lease remains |

The job queue and number of in-flight snapshots must be bounded. Queue saturation or admission failure returns a bounded busy/failure result; it must not create an unbounded backlog or cause the recorder to wait.

A cancelled save is not allowed to release its lease merely because a cancellation flag was set. The lease remains valid until the serializer/copy worker confirms that it no longer accesses the pages.

## 5. Host-specific scheduling

### 5.1 Standalone file save

- Run file serialization on a non-render I/O worker.
- The UI may display progress and await completion without blocking the audio callback.
- Capture at a published block/page boundary; the recorder continues into writable capacity separate from leased pages.
- Use the standalone-owned temporary-file/replace transaction described in the State Container Audit.
- Cancellation stops future reads and releases leases only after worker quiescence.

### 5.2 AU state

- Keep `fullState` and `fullStateForDocument` scope distinct.
- The dictionary-shaped API does not prove incremental streaming. If the host path materializes a complete binary Data value, the entire materialized payload and any additional source/copy storage must be admitted.
- Do not assume the callback can wait for a capture worker, audio-thread acknowledgement or disk I/O.
- Record actual callback context, payload representation, copy/materialization count, elapsed time and round-trip behavior in the AU host prototype.
- If a host cannot carry the DEC-025/DEC-042 document payload within its actual state contract, record a compatibility blocker; do not omit required audio.

### 5.3 VST3 state

- Keep the processor/component payload separate from controller/UI-specific state and preserve the documented component-first restore sequence.
- Write through bounded `IBStream` transfers. Each call respects the signed int32 byte-count parameter, checks the actual transferred count, and terminates on error or zero progress. A short transfer is not success.
- Do not require seeking or back-patching for the canonical container.
- Steinberg's state-interface thread annotations are not a universal host guarantee for every offline/render mode. Record the actual callback thread/context for each approved host.
- Prefer a prebuilt immutable snapshot. If a host invokes state handling on a processing thread, the path must not wait, allocate, perform unbounded capture, or block on a worker. If the required payload cannot be emitted within the permitted context, record a host compatibility blocker rather than weakening DEC-025.

### 5.4 Prepared-snapshot policy

A host adapter may consume a previously prepared immutable snapshot or use a bounded capture path only when the host contract permits it. It must not synchronously request that the render thread stop, wait for the next block, or acknowledge a capture command.

A prepared snapshot that is stale against the published committed-frame watermark is not a valid successful save. The adapter returns a bounded failure unless a host-permitted non-render refresh can complete without waiting on the render thread. This is a conservative engineering proposal, not a claim that current hosts have been validated.

## 6. Rolling-recording retention and backpressure

1. Preflight the complete set of required recording/Freeze ranges before publishing a snapshot.
2. Acquire generation-checked leases for all pages/ranges required by the transaction. If any required lease fails, release the partial acquisition off-render and publish no snapshot.
3. Leased pages cannot be overwritten or reused until the last serializer/copy reader releases them.
4. The recorder continues into separately reserved writable pages/segments. It never waits for lease release and never writes into leased pages.
5. If retention and writable headroom cannot both be guaranteed, fail the save capture before it is reported ready; leave the recording active and intact.
6. If a copy-to-staging strategy is used instead of page pinning/rotation, reserve and allocate the full copy off-render before copying. The writer remains independent of the copy.
7. Lease exhaustion, retirement saturation and queue-full conditions are observable bounded failures. No unsafe concurrent ring read is permitted.

The page pinning/segment-rotation algorithm, exact spare capacity, and whether a capture uses leases or pre-reserved copies remain engineering choices. This contract specifies their required behavior, not their implementation.

## 7. Unified peak-memory admission

### 7.1 Count physical allocations once

The ledger distinguishes **physical bytes** from **capacity reservations**:

- A leased page that aliases the live recording pool is counted once as physical memory. Its pinned-page count still consumes retention capacity.
- A copied snapshot/staging page is an additional physical allocation and is counted separately.
- Shared immutable library assets are counted once even if several layers reference them.
- AU Data materialization is counted as an additional allocation only when it creates an additional owned copy.
- VST3 transfer staging is counted at its actual allocated capacity; unknown host-owned stream buffers are not misrepresented as engine-owned memory.
- Reserved spare recording pages are counted in the recording pool's physical allocation once, while their writable/retention capacity is tracked separately.
- Old runtime memory is not counted again merely because it becomes retired. Count a retired generation separately only when it is a distinct additional physical allocation.

### 7.2 Save and restore working sets

A planning model for a save is:

`M_save_peak = M_live_runtime_and_recording_pool + M_snapshot_copy + M_manifest_directory + M_AU_materialization + M_VST3_or_file_transfer + M_codec_workspace + M_digest_workspace + M_other_retired_allocations`

A planning model for restore is:

`M_restore_peak = M_old_runtime + M_candidate_runtime + M_decode_or_conversion_staging + M_manifest_and_migration + M_transport_staging + M_codec_workspace + M_other_retired_allocations`

If save, restore, Freeze preparation, sample replacement or recording retention overlap, admission uses the union of their simultaneously live physical allocations and reservations. Independent per-feature budgets are not sufficient.

### 7.3 Admission transaction

- Reserve retention capacity and every known plug-in-owned staging/workspace requirement before acquiring long-lived leases or materializing payloads.
- Use checked 64-bit arithmetic for totals and offsets; validate conversion to `size_t` and host API widths before allocation/transfer.
- Do not treat a logical alias, lease, copy and host materialization as interchangeable memory costs.
- Keep reservations until the owning serializer, decoder, runtime reader or retirement collector has released the corresponding allocation.
- On failure, roll back the incomplete reservation/lease transaction off-render. The active runtime and valid recording remain authoritative.
- The host's unknown internal buffering is recorded as an evidence limitation, not assigned a guessed multiplier. Measure it only in an explicitly authorized host prototype; numeric performance measurement/benchmark remains separately gated.

No byte limit, concurrency count, page count, transfer-slice size, callback deadline, codec workspace or process-memory budget is selected here.

## 8. Failure and recovery matrix

| Condition | Required outcome |
|---|---|
| Snapshot descriptor behind latest committed watermark | Do not report a successful stale document save; return bounded failure or use a host-permitted refresh |
| Required recording/Freeze lease unavailable | No partial snapshot publication; recording and active state remain intact |
| Ring wrap while pages are leased | Writer uses reserved writable capacity; leased pages remain unchanged |
| Writable/retention headroom exhausted | Fail new capture before source/state switch; never wait in render |
| Host callback occurs in unsupported/RT context | No wait or unbounded work; return a bounded failure if no valid prepared path exists |
| AU materialization exceeds admitted capacity | Fail save; preserve in-memory audio; do not silently omit chunks |
| VST3 short write, zero progress or error | Stop safely; no completion marker or success result |
| Save cancelled during a read | Retain lease until the reader stops, then release off-render |
| Concurrent save/restore reservations exceed admission | Reject/defer a new bounded job; do not partially publish or grow an unbounded queue |
| Restore decode/admission/migration fails | Keep prior active runtime; release candidate/staging off-render |
| One required chunk is missing or invalid | Reject the document-state transaction; no reduced-success restore |

## 9. Required Phase 5 fixture families

These supplement, and do not replace, the existing Q-T15 container fixtures.

| Fixture family | Stimulus | Required assertion |
|---|---|---|
| Capture watermark | Save while one or more recording sources are active; commit blocks around the capture boundary | Descriptor records the exact latest fully committed frame range per source; no partial block is included |
| Stale prepared snapshot | Advance a recording commit watermark after the ready descriptor was built | Adapter does not return the stale descriptor as a successful DEC-025 document save |
| Host callback context | Trigger state save during normal and offline processing for each target host | Record callback thread/context; no render wait, unbounded copy, allocation or disk I/O |
| Ring-wrap retention | Force ring wrap while a save lease is held | Leased pages remain unchanged; recording continues in separate writable capacity |
| Lease acquisition rollback | Make one required page/range unavailable after earlier leases were acquired | No partial snapshot is published; all acquired leases are safely released |
| Retention exhaustion | Exhaust pinned-page/spare-page admission | Save fails visibly; recorder and previous state remain usable |
| Save cancellation | Cancel during a chunk read/write | No page is released while a reader still accesses it; no false completion marker |
| Bounded job queue | Submit more saves than the configured in-flight admission allows | Deterministic busy/failure result; no unbounded queue or RT wait |
| Save plus Freeze | Save while Source Snapshot Freeze and independent recording are active | Required recording/Freeze generations remain consistent and continue to be retained |
| Save plus restore | Overlap a save lease with restore candidate preparation | Aggregate peak reservation is enforced; no double-counting of aliases or omission of physical copies |
| AU materialization | Round-trip document state through each approved AU host at owner-approved payload sizes | Record Data representation, actual copies, payload size, callback context and integrity |
| VST3 transfer progress | Partial writes, zero progress, stream error and large logical chunk split across bounded calls | No truncation or false success; completion footer appears only after all required bytes |
| Admission rollback | Inject staging, digest/codec workspace or candidate allocation failure | All reservations roll back off-render; active state and recordings remain intact |
| Physical-memory ledger | Compare shared aliases, pinned pages, copied snapshots, AU materialization and transfer buffers | Each physical allocation counted once; retention capacity tracked separately |
| Callback freshness ladder | Repeat save at idle, during active recording, immediately after a commit and during offline processing | Freshness and failure outcomes are deterministic and host-specific |

These are planned fixtures only. No formal AC IDs, thresholds, host pass results, memory measurements or benchmark results are created.

## 10. Open gates and decision boundaries

| Gate | Still open |
|---|---|
| Q-T15 | Exact capture freshness implementation, host callback strategy, container framing/codec/checksum, maximum state size, concurrent job limit and migration retention |
| Q-P09 | Exact host versions, callback contexts, payload limits, AU materialization and VST3 stream behavior |
| Q-T07 / Q-T05 | Numeric CPU/memory/recording-retention budgets; measurement requires separate explicit authorization |
| DEC-025 | DAW document state must embed required unsaved recordings; no silent omission |
| DEC-042 | Source Snapshot behavior is approved. Successful-subset versus all-or-nothing policy for mixed-layer Global Freeze preparation failure remains owner-gated |
| Q-T15 | User-preset inclusion of unsaved recordings/Freeze remains distinct from DAW document-state requirements |
| AR-06 | Audible treatment of old voices when a restore transaction commits remains an owner decision |

No product decision or owner-gated behavior is changed by this audit. No new requirement IDs, AC IDs, public API/ABI commitments, numeric limits or host certifications are introduced.

## 11. Primary references

- Repository: [State Container and Migration Audit](PHASE3_STATE_CONTAINER_AND_MIGRATION_AUDIT.md)
- Repository: [Memory Admission and Render Capacity Audit](PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md)
- Repository: [Threading and Ownership Specification](THREADING_AND_OWNERSHIP_SPECIFICATION.md)
- Repository: [Sample and State Lifecycle Specification](SAMPLE_AND_STATE_LIFECYCLE_SPECIFICATION.md)
- Apple: [AUAudioUnit.fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate)
- Apple: [AUAudioUnit.fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument)
- Steinberg: [VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html)
- Steinberg: [IBStream](https://steinbergmedia.github.io/vst3_doc/base/classSteinberg_1_1IBStream.html)

**Completion boundary:** this document advances the architecture and test design only. It does not authorize production code, a host prototype, a benchmark, an AU/VST3 shell, a Phase 5 test execution or a PR merge.


## 12. Cross-host callback context and failure signalling — 2026-11-28

The new [Cross-Host State Callback & Snapshot Freshness Evidence Matrix](PHASE3_CROSS_HOST_STATE_CALLBACK_AND_SNAPSHOT_FRESHNESS_EVIDENCE_MATRIX.md) separates format API annotations from host-observed callback behavior.

Key refinements:

- VST3 component `getState/setState` are documented as UI-thread calls permitted while Processing; this does not stop or quiesce `process`. A Steinberg developer-forum clarification says offline get/set-state may run on the same thread as process; treat this as supplemental caution, not a universal host guarantee.
- AU `fullState/fullStateForDocument` do not publish a callback-thread guarantee or an explicit property-getter error result. A `nil` dictionary must not be assumed to abort the host's project save.
- Logic, Live, Bitwig and REAPER host guides establish persistence/hosting controls but do not establish the exact state-callback thread, deadline, stale-snapshot handling or failure propagation.
- A prepared descriptor must match one coherent cross-source capture-boundary vector: persistent state epoch, publication sequence, and every required source generation/committed-frame watermark. Independent unsynchronized per-source reads can produce a torn snapshot.
- If the descriptor is stale, a required chunk is absent, or the capture boundary cannot be read coherently, never report successful DEC-025 document state. A bounded refresh is allowed only where the actual callback context permits it and all leases are already available without waiting for render.
- AU's missing explicit getter error channel is a compatibility blocker until each target host's handling of nil/failed document state is observed. VST3 `tresult` is an explicit plug-in result, but host-visible save rollback/failure still requires prototype evidence.

No host callback was observed and no host was certified. DEC-025/DEC-042, AR-06, Q-T15 and Q-P09 remain unchanged. See the evidence matrix for host rows and planned fixtures.

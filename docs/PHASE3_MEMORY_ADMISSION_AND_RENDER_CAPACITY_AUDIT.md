# Phase 3 Memory Admission and Render Capacity Audit

**Version:** 0.2 — host state materialization and process-boundary accounting refinement  
**Status:** Proposed engineering contract; no numeric process-memory budget, host topology, framework, or implementation is approved.  
**Baseline:** DEC-017/020/025/026/028/040; GS-ARC-006/007; AU/VST3 adapter contract; Q-T03/Q-T05/Q-T07/Q-T17.  
**Scope:** Documentation-only audit. No code, host prototype, benchmark, or runtime measurement was performed.

## 1. Executive findings

The architecture already requires reservation-based admission for sample assets, Freeze, recording, and state restore. This audit closes a separate accounting gap: the render-time working set must be sized from the host's negotiated maximum block and bus arrangement, not from the current callback's frame count.

The normalized render capacity is:

- **AU:** host-provided `maximumFramesToRender`, established before render resources are allocated.
- **VST3:** `ProcessSetup::maxSamplesPerBlock`, established while the component is inactive.
- **Standalone:** the audio-device callback's configured maximum block size.

For each prepared configuration, define `B` as the maximum frame capacity and `N` as the actual frame count for a render call. The invariant is `0 <= N <= B`. The audio callback never grows or reallocates scratch storage. A host call exceeding the prepared capacity is a bounded configuration/protocol failure; the adapter must not write out of bounds or attempt an in-callback resize.

The current product/host documents also need a clear distinction between **physical host input buses** and **logical per-layer source views**. The R1 product specification does not impose a fixed product-level cap on live-input channels/sources. A single optional stereo input bus is therefore a minimum host-validation configuration, not a product-wide maximum. A host input view may be fanned out read-only to several layers without allocating four duplicate copies.

## 2. Product and host constraints preserved

- R1 has exactly four independent layers (DEC-017).
- The product requires one stereo master output and four independent stereo layer outputs (GS-ARC-006); layer outputs may be optional/deactivated where a host cannot expose them, but the internal layer contribution to master remains.
- Every layer can use live external audio as a source (DEC-020).
- Product policy does not set a fixed maximum number of live-input channels/sources; actual availability is constrained by device, host routing and measured resources.
- Mono input is centered for stereo layer processing (DEC-040).
- The six-minute stereo 48 kHz float32 source duration remains a target subject to measured feasibility and safe memory (DEC-028).
- Unsaved recordings and Freeze snapshots remain embedded in plugin state (DEC-025).
- No disk streaming, silent truncation, in-use asset eviction, or product-scope change is introduced.

## 3. Render-capacity model

### 3.1 Host maximum versus actual frames

| Runtime | Capacity source | Reconfiguration rule |
|---|---|---|
| AU | `maximumFramesToRender` | Set before `allocateRenderResources`; a changed maximum requires the AU resource lifecycle to be deallocated/reconfigured/reallocated. |
| VST3 | `ProcessSetup::maxSamplesPerBlock` | Change only while inactive: stop/deactivate, run `setupProcessing`, then reactivate/restart under a new lifecycle generation. |
| Standalone | Device callback's configured maximum | Prepare a new device/render configuration off the callback and publish only after the device lifecycle makes the old configuration quiescent. |

For every audio-render call:

1. Read the actual frame count `N` supplied by the host/device.
2. Validate `N <= B` before any scratch-buffer indexing.
3. Process only the actual `N` frames; unused capacity is not rendered.
4. Never resize, allocate, free, or change buffer capacity in the callback.
5. If `N > B`, enter a bounded adapter error path, record the fault outside the RT path, and produce the format-defined safe failure/output behavior without touching memory beyond `B`. Exact SDK status mapping is adapter-specific and must be validated; it is not guessed here.

A VST3 parameter-flush call with `numInputs == 0` and `numOutputs == 0` is not an audio render. It must not access audio scratch buffers, consume a fabricated frame count, or advance audio/transport. AU parameter scheduling may arrive from multiple thread contexts; it is an event-ingress concern, not permission to resize render resources.

### 3.2 Byte-accounting formula

For float32 audio, let:

- `S = sizeof(float) = 4 bytes`;
- `B` = prepared maximum frames per block;
- `C_in` = channels actually copied into engine-owned input staging;
- `C_out` = channels actually copied into engine-owned output/working staging.

Then:

`M_transient = B × S × (C_in + C_out)`

This is the payload size only. Add separately accounted alignment, guard regions, metadata and any other algorithm-specific workspaces. The formula must use checked multiplication before allocation.

For the product's five stereo output buses (master + four layer stems), if an implementation chooses to stage a full copy of every output bus, `C_out = 10`, giving `40 × B` bytes. If it also stages one stereo live-input bus, `C_in = 2`, giving `48 × B` bytes total.

| Illustrative prepared capacity B | Five stereo output copies | Plus one stereo input copy | Combined payload |
|---:|---:|---:|---:|
| 1,024 frames | 40 KiB | 8 KiB | 48 KiB |
| 4,096 frames | 160 KiB | 32 KiB | 192 KiB |

These are arithmetic examples, not an approved block-size range or a required allocation topology. Q-T17 remains open.

### 3.3 Borrowed host buses versus owned scratch

The five host output bus buffers and negotiated input buffers are normally borrowed views valid only for the callback. Their payload must **not** be counted as engine-owned scratch merely because the engine writes/reads them. The engine's transient memory ledger counts only buffers it actually owns or stages.

The implementation may render directly into valid host output buffers where signal flow and bus activation permit. It may also require internal per-layer working planes for FX, stem taps, master summing or crossfades. Those planes must be explicitly enumerated in the implementation's memory ledger; the architecture must not assume either zero scratch or five unconditional full-bus copies.

For input routing, the host adapter owns a negotiated collection of physical input buses/channels and normalizes it into mono/stereo channel-pair views while preserving source bus/channel identity; a wider physical bus may therefore produce multiple normalized views. It constructs logical per-layer source views for the four layers. A logical view may alias the same read-only host input samples used by another layer. Fan-out is not four extra copies. A copy is charged only if an explicit staging/format-conversion operation materializes one.

If the selected host exposes only one stereo input bus, all four layers may be routed from that source subject to the product's per-layer source controls. Additional independent physical inputs are available only where the host/device exposes and validates them. The one-stereo-input prototype configuration is not a fixed R1 product cap.

## 4. 32-bit sample data versus process architecture width

These are distinct concepts:

- **float32** describes the audio sample representation and is four bytes per sample in the calculations above.
- `FrameCount`/sample offsets may use a 32-bit unsigned integer without implying a 32-bit process.
- The existing macOS architecture investigation targets x86_64 and arm64, both 64-bit process architectures; Q-T03 (minimum macOS/architecture support) remains formally open. No 32-bit macOS build target is approved or implied by the use of `uint32_t`.

Memory accounting must use a wide unsigned byte-count type (candidate: `uint64_t`) with checked multiplication/addition. Before allocating, validate that the result fits the platform's `size_t` and the admission ledger's remaining capacity. Reject overflow or unrepresentable sizes before allocation. Do not cast a byte count to `size_t` first and then attempt to detect overflow.

Long-lived frame totals, recording lengths and serialized chunk lengths must remain 64-bit. A per-callback `FrameCount` is bounded by the negotiated host/device capacity. This distinction applies on both x86_64 and arm64.

## 5. Render-resource preparation and lifecycle

A prepared render-resource generation is keyed by at least:

- sample rate;
- maximum frames per block `B`;
- negotiated input channel/bus arrangement;
- negotiated output bus/channel arrangement;
- internal sample format and processing mode assumptions that affect buffer shape.

Preparation occurs off the render thread and reserves/allocates all fixed scratch and DSP workspaces needed by that generation. Publication is permitted only through the host/device lifecycle and safe-point rules already defined in the Threading and Plugin Bus specifications.

On reconfiguration:

1. Stop/deactivate or otherwise quiesce the old host/device configuration according to its API contract.
2. Build and admit the complete new resource generation off-thread.
3. If admission or preparation fails, preserve the prior valid configuration where the host lifecycle allows it; otherwise remain in a defined inactive/error state. Do not publish a partial resource set.
4. Advance the lifecycle generation and publish the complete new configuration.
5. Retire old resources only after no callback can still reference them; final destruction remains non-RT.

A smaller actual callback block does not justify shrinking buffers during processing. A changed maximum or bus arrangement is a lifecycle event, not a per-block allocation opportunity.

## 6. Admission ledger separation

Keep separate ledger categories for:

1. Persistent/resident decoded library sample payloads.
2. Active, pending, reader-held and retiring sample generations.
3. Recording rings and committed unsaved recordings.
4. Freeze snapshots and retention/rotation headroom.
5. State serialization/restore staging and embedded audio chunks.
6. Analysis/waveform data.
7. Fixed render scratch and per-layer working planes.
8. FX delay/state memory, voice/grain pools and modulation/sequencer runtime.
9. Adapter staging or format-conversion buffers, if any.

Do not double-count borrowed host buffers or shared immutable sample handles. Do count physical copies, retained generations and simultaneously live staging payloads. Admission for a state restore or Freeze must include the peak coexistence of old active state, new prepared state, readers, staging and retirement—not only the final steady-state payload.

The existing raw-audio estimate remains: one six-minute stereo 48 kHz float32 source is 138.24 MB decimal; four sources are 552.96 MB decimal. Four additional full-length Freeze copies would bring source-plus-snapshot audio payload to 1,105,920,000 bytes (about 1.03 GiB). This excludes all render scratch, active recording headroom and other runtime overhead. It is not a process-memory budget.

## 7. Required validation evidence (not yet run)

- AU: verify actual render frame counts never exceed the prepared `maximumFramesToRender`; verify changed maximum follows the AU resource lifecycle.
- VST3: verify actual `numSamples` stays within `maxSamplesPerBlock`; verify deactivate/setup/reactivate ordering; verify zero-bus parameter flush touches no audio scratch and advances no transport.
- Standalone: verify device maximum and sample-rate/channel reconfiguration generations.
- Boundary cases: `N = 0` where permitted, `N = 1`, `N = B`, and a deliberately invalid `N > B` adapter fixture.
- Bus permutations: master only; master plus any subset of four layer outputs; optional input absent; mono input; stereo input; multiple negotiated input channels/buses where the host supports them.
- Verify one stereo input may be fanned out to four layer source views without four implicit copies; verify any actual staging copy is accounted.
- Checked arithmetic: multiplication/addition overflow, `uint64_t` to `size_t` conversion, and admission-ledger exhaustion.
- Reconfiguration stress: repeated sample-rate, maximum-block and bus-layout changes with allocation guards proving no render-thread allocation/free.
- Confirm inactive layer outputs do not remove layer contribution from master and do not cause duplicate summing.
- Confirm AU failed input pull is treated as unavailable input for that call; no stale buffer is read.

These are planned validation obligations, not test results. Numeric memory headroom, warning thresholds, maximum input channels, maximum block size, maximum simultaneous recordings and global grain budget remain open pending approved host matrix and authorized measurement.

## 8. Open gates

- Q-T03: minimum macOS and supported architecture baseline.
- Q-T05: resident sample policy and long-sample strategy.
- Q-T07: measured global grain/CPU budget.
- Q-T17: supported sample-rate, block-size and latency matrix.
- Q-P06/Q-P09: final AU component and host-specific bus exposure/activation.
- AR-01/AR-02: event precedence and deterministic overload/reclamation policy.
- DEC-042 owner review: successful-subset versus all-or-nothing behavior for mixed Global Freeze preparation outcomes.

No owner decision is inferred by this audit. No framework, AU component type, plugin bus maximum, numeric memory budget or implementation is selected.



## 9. State serialization and migration working-set admission

State save/restore has a separate peak working set from steady-state audio rendering. Admission must account for the maximum simultaneously live physical payloads, not only the final state-file size.

Candidate accounting:

M_state_peak = M_old_active + M_snapshot_payloads + M_candidate_payloads + M_manifest_directory + M_transport_staging + M_codec_workspace + M_digest_workspace + M_retirement

Interpretation:

- M_old_active includes the active graph and any sample/recording/Freeze assets retained until the restore commit and reader retirement.
- M_snapshot_payloads includes immutable recording and Freeze regions retained while save is in progress. A shared immutable asset is counted once; a physical copy is counted separately.
- M_candidate_payloads includes decoded/prepared embedded recordings, Freeze snapshots and any sample conversion needed by the restored candidate.
- M_manifest_directory includes the manifest, chunk table, validation metadata and migration representation.
- M_transport_staging includes only buffers actually owned/materialized by the adapter. Borrowed host stream memory is not charged as engine-owned.
- M_codec_workspace and M_digest_workspace are included only if compression or digest calculation uses additional workspace. Both remain non-RT and require admission.
- M_retirement includes old generations awaiting safe non-RT destruction.

Host-specific distinction:

- AU fullState/fullStateForDocument are dictionary-shaped. If a target host/API path materializes the embedded payload as one or more complete Data values, count the complete materialized bytes plus any immutable source snapshot. Do not assume an incremental streaming path.
- VST3 IBStream permits incremental read/write calls, but the adapter still needs a bounded transfer buffer and the host may internally buffer its own stream. Do not count unknown host-owned memory as engine memory, and do not claim that the host streams to disk without evidence.
- Standalone file serialization can stream bounded slices from immutable snapshots; the application-owned I/O worker and any OS cache are distinct from render scratch.

Required invariants:

1. Use checked 64-bit arithmetic for total/container/chunk lengths and verify conversion to size_t and host API argument widths before allocation or transfer.
2. Reserve memory for old active state + candidate state + snapshots + transfer/decode staging + pending retirement before restore publication.
3. Do not double-count a shared immutable library asset; do count Freeze copies, AU Data materialization, decode output and staging copies.
4. Release reservations only after the corresponding non-RT owner has disposed of the payload or the old runtime is no longer readable.
5. If admission fails, preserve the previous active state and all valid in-memory recordings; never silently omit DEC-025/DEC-042 payloads.
6. No compression, digest, allocation, file I/O or host-stream transfer is performed by the audio callback.

No maximum embedded-state size, AU materialization limit, VST3 transfer-slice size, codec workspace, compression ratio, warning threshold or process-memory budget is set. Q-T15/Q-T07 and host prototypes remain gates. This section contains accounting requirements only; no benchmark or memory measurement was performed.

## 10. State I/O scheduling and unified peak admission — 2026-11-28

State-save admission must distinguish physical bytes from retention-capacity reservations:

- A pinned page that aliases the live recording pool is counted once as physical memory; its pinned-page occupancy still consumes retention capacity.
- A copied snapshot/staging page is an additional allocation and is counted separately.
- Shared immutable library assets are counted once. AU Data materialization is an additional allocation only when it creates an additional owned copy.
- VST3 transfer staging is counted at its actual allocated capacity. Unknown host-owned stream buffering remains an evidence limitation, not an invented multiplier.
- Spare writable recording pages are included in the recording pool's physical allocation once; their availability is tracked separately from pinned-page occupancy.
- A retired generation is not counted twice if it is already included in the old-runtime allocation set.

Planning models:

`M_save_peak = M_live_runtime_and_recording_pool + M_snapshot_copy + M_manifest_directory + M_AU_materialization + M_VST3_or_file_transfer + M_codec_workspace + M_digest_workspace + M_other_retired_allocations`

`M_restore_peak = M_old_runtime + M_candidate_runtime + M_decode_or_conversion_staging + M_manifest_and_migration + M_transport_staging + M_codec_workspace + M_other_retired_allocations`

If save, restore, Freeze, recording retention or sample replacement overlap, admission uses their union of simultaneously live physical allocations and capacity reservations. Independent per-feature budgets are not sufficient.

The capture watermark is the latest fully committed recording block/page included in the immutable descriptor. Stale descriptors cannot be reported as successful DEC-025 document saves. Retention leases protect the exact source-generation/frame range until the last reader completes; the writer continues only into separately reserved writable capacity and never waits.

Reservation rollback and lease release occur off-render after readers quiesce. All totals/offsets use checked wide arithmetic before conversion to size/host API widths. No numeric budget, concurrency count, page capacity, AU payload limit, VST3 transfer slice or callback deadline is selected. See `PHASE3_STATE_IO_SCHEDULING_AND_PEAK_MEMORY_ADMISSION_AUDIT.md`.


## 11. State callback materialization and host-process accounting — 2026-11-28

The callback evidence matrix adds two memory-accounting constraints:

- AU state is dictionary-shaped. Count the plug-in-owned manifest, Data objects, materialized payloads and any confirmed bridging/copy allocations separately. The number of Foundation/host copies is unknown; do not apply an assumed multiplier.
- VST3 plug-in-owned transfer buffers are counted at their actual allocated capacity. Host-owned IBStream buffering is unknown and is not a plug-in allocation unless a prototype demonstrates an owned copy.
- Bitwig's documented hosting modes can place plug-ins in the audio-engine process or in separate shared/individual processes. Any future authorized memory prototype must record the selected mode, per-process memory and aggregate process memory. Process isolation changes accounting boundaries; it does not remove the payload's physical-memory cost.
- A state callback that overlaps render may retain immutable snapshot pages while the recorder continues. Count those pages once as physical memory and separately reserve the required retention/writable capacity.
- State save/restore admission continues to include old runtime, snapshot retention/copy, AU materialization or VST3 staging, decode/codec workspace and pending retirement. Unknown host-owned buffers remain an evidence gap, not an invented byte estimate.

No memory measurement, process-memory budget, copy count, payload limit or benchmark was produced. See the callback/freshness evidence matrix and Q-T15/Q-P09 gates.

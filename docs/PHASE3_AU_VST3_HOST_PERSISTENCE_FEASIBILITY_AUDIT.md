# Phase 3 AU/VST3 Host Persistence Feasibility Audit

**Version:** 0.2 — callback context and failure-signalling evidence  
**Status:** Evidence map and test design only. No host is certified; no payload limit, callback deadline, container ABI or host compatibility promise is approved.  
**Checked:** 2026-11-28  
**Baseline:** DEC-025, DEC-042, Q-T15, Q-P06, Q-P09, GS-NFR-001, PHASE3_STATE_CONTAINER_AND_MIGRATION_AUDIT.md and PHASE3_CROSS_HOST_STATE_RESTORE_AND_RESET_EVIDENCE_MATRIX.md.  
**Scope:** AU and VST3 persistence surfaces in Logic Pro, Ableton Live, Bitwig Studio and REAPER. Documentation research only; no host prototype, implementation, benchmark or runtime measurement was performed.

## 1. Executive findings

The reviewed evidence supports three conclusions:

1. **The plug-in formats expose different persistence transports.** AU exposes dictionary-shaped `fullState` and `fullStateForDocument` properties. VST3 exposes component/controller state methods that read and write an `IBStream`.
2. **Ableton provides the clearest host-level statement about opaque plug-in state.** Its support article says a VST/AU plug-in's state is stored in a reserved binary data block in the Live Set and that the plug-in itself reads and writes that block. This establishes the existence of an opaque state path, not its maximum size, large-audio suitability, callback deadline or failure semantics.
3. **No reviewed host/API source publishes a maximum payload size or a deterministic save/restore deadline for the required GRANULAR SYNTH recording and Freeze payload.** DEC-025 must not be weakened to fit an undocumented host limit. If a target host cannot round-trip the required payload, that is a compatibility blocker to resolve explicitly.

Evidence labels used below:

- **D — documented:** the vendor explicitly documents the stated general behavior.
- **P — partial:** a related capability is documented, but the required product payload or lifecycle is not.
- **U — unverified:** no sufficient public evidence was found for the exact behavior.
- **API only:** the plug-in format defines an interface; host behavior remains unproven.

A documented project save, a plug-in preset, a plug-in binary being installed, and a complete DEC-025 document-state round-trip are four different claims.

## 2. AU persistence boundary

### 2.1 Documented API semantics

Apple documents:

- `AUAudioUnit.fullState` as a persistable `[String: Any]` snapshot suitable for a user preset. Hosts may use it for a user preset or a document. The unit should not persist transient properties such as stream formats.
- `AUAudioUnit.fullStateForDocument` as a persistable `[String: Any]` snapshot suitable for a user document. If a subclass does not implement this property, it interfaces with `fullState` instead.
- The base `fullState` implementation saves parameter-tree values.

Sources: [AUAudioUnit.fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate), [AUAudioUnit.fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument), [AUAudioUnit](https://developer.apple.com/documentation/audiotoolbox/auaudiounit).

### 2.2 Consequences and limits of the evidence

- Keep user-preset intent and DAW-document intent distinct in the product container, even though hosts may use `fullState` for either context and an AU subclass may fall back from `fullStateForDocument` to `fullState`.
- The API is dictionary-shaped; it is not documented as a seekable streaming interface. Do not claim that a large recording/Freeze payload is streamed, or that arbitrary binary payload sizes are accepted.
- The reviewed Apple references do not specify a maximum dictionary/payload size, a host callback deadline, interruption semantics, or a universal callback-thread guarantee for reading/writing these properties.
- Apple documents AUv3 instantiation in-process or in a separate extension process in some configurations. This is not evidence of a particular Logic Pro state-transfer implementation or of a known IPC copy count.
- A binary `Data` value remains a candidate representation only. Its accepted type, size, copying behavior and round-trip must be proven on each target host before Q-T15 can be closed.

**Classification:** AU API capability is documented; large custom payload behavior and callback timing are **U**.

## 3. VST3 persistence boundary

### 3.1 Documented API semantics

Steinberg documents two separate state surfaces:

- Component state: `IComponent::getState` / `setState`.
- Controller state: `IEditController::getState` / `setState`, with `setComponentState` receiving the processor's component state.

The documented save/load order is:

1. Save: component `getState`, then controller `getState`.
2. Load: component `setState`, controller `setComponentState`, then controller `setState`.

The controller must mirror processor state without generating duplicate host parameter writes.

Sources: [Steinberg VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html), [IComponent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html), [IEditController](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IEditController.html).

### 3.2 Stream contract

`IBStream` exposes:

- `read` and `write` with an `int32` requested byte count and an optional `int32` actual-transfer count;
- `seek` and `tell` with `int64` positions.

Source: [Steinberg IBStream](https://steinbergmedia.github.io/vst3_doc/base/classSteinberg_1_1IBStream.html).

This means one call's requested transfer length is bounded by its signed 32-bit argument. It does **not** define a 32-bit total-state limit: a logical payload may be processed through multiple bounded calls, provided the implementation handles partial progress, end-of-stream, errors and checked 64-bit offsets. Conversely, the existence of a stream interface does not prove that a particular host backs it with incremental disk streaming rather than memory buffering.

### 3.3 Thread and lifecycle semantics

Steinberg annotates `IComponent::getState/setState` as UI-thread calls and permits them in lifecycle states that include Processing. The `IEditController` state methods are annotated UI-thread and Connected.

Interpretation:

- “Processing” is a permitted component lifecycle state; it is not a statement that the method executes on the audio render thread.
- The API annotation does not promise that processing has stopped while state is read or applied.
- Do not mutate render-owned graphs, voice maps or audio resources directly from state methods. Capture/prepare immutable state outside render, then publish using the existing safe-point and retirement contract.
- The API documentation does not set a maximum state size or a callback-time budget.

**Classification:** VST3 component/controller and stream APIs are documented; host-specific buffering, total payload limits and callback timing are **U**.

## 4. Host × format evidence matrix

| Host / format | Positive evidence | What it does not prove | Payload feasibility |
|---|---|---|---|
| **Logic Pro / AU** | Apple documents plug-in settings stored with the project and automatically recalled when the project is reopened. Logic also documents separate load/save plug-in-setting actions. | Large embedded recording/Freeze payloads, AU dictionary materialization cost, exact reset callback order, bus activation persistence and failure atomicity. | Project settings **D**; DEC-025/042 payload size and round-trip **U**. |
| **Ableton Live / AU + VST3** | Ableton explicitly documents a reserved binary data block in a Live Set for plug-in state; the plug-in reads/writes the block. It also documents VST/AU preset/configuration workflows. | Maximum binary-block size, whether large audio payloads survive every Set/preset operation, callback timing, partial-write behavior and recovery from truncated state. | Opaque per-instance state path **D**; large DEC-025/042 payload feasibility **U**. |
| **Bitwig Studio / VST3** | Project folders can contain a `plugin-states` subfolder. Bitwig documents multiple plug-in hosting modes and a Reload Plug-in action after a crash. | Whether this product payload is stored as one opaque chunk or external files, payload-size limits, exact state callback ordering, or preservation of active transient voices. | Project-associated plug-in-state storage **D**; custom payload size/round-trip **U**. |
| **REAPER / AU** | REAPER documents project files holding track/media/settings and supports AU FX. | The reviewed guide does not define AU `fullState` payload representation, maximum size, or DEC-025/042 binary round-trip. | General project/FX persistence **P**; required AU payload **U**. |
| **REAPER / VST3** | REAPER documents FX-chain parameter settings, VST/VST3 compatibility options including “Save state as VST bank” and “Hard reset on playback start”, plus opening FX offline or bringing them online after load. | These controls do not define the generic VST3 opaque state limit, the host's `IBStream` buffering strategy, or custom recording/Freeze payload acceptance. | FX/project/preset workflows **D/P**; required custom payload **U**. |

### 4.1 Host-specific interpretation

**Logic Pro / AU**

Apple's Logic guide says plug-in settings are stored with the project and automatically recalled on reopen. The separate plug-in setting UI supports load/save operations. Keep this evidence separate from the AU `reset()` API and from a proof of large embedded binary payload acceptance. No size or callback deadline is published in the reviewed sources.

**Ableton Live / AU and VST3**

Ableton's support article is unusually explicit: plug-in state is in a reserved binary block in the Live Set, readable/writable by the plug-in itself. A different Ableton article clarifies that “Save as Default Configuration” stores the visible parameter configuration, not the current plug-in state; a specific Rack/preset workflow is needed for a saved configuration. Collect All and Save does not copy third-party plug-in binaries into the project folder. The latter is an installation/portability issue, not evidence that plug-in state is absent from the Set.

**Bitwig Studio / VST3**

The project folder's `plugin-states` directory is evidence of project-associated state storage. Bitwig's hosting mode can run plug-ins in-process, together in another process, by manufacturer, by plug-in, or individually. The selected hosting mode must be recorded in any memory and persistence test; changing it can require project reopen or audio-engine reload. Reload Plug-in is a process-recovery control, not a guarantee that transient voices or a large custom state payload round-trip correctly.

**REAPER / AU and VST3**

The current REAPER User Guide v7.86 (November 2026) documents project files, FX-chain save/load, offline/online load options and VST/VST3 compatibility settings. “Save state as VST bank” and “Hard reset on playback start” are optional VST compatibility settings; they must not be treated as universal project-state or reset semantics. The reviewed guide does not state a maximum opaque state size.

## 5. Threading and memory implications

### 5.1 AU

Treat AU state as potentially materialized in memory because the public surface is a dictionary rather than a documented streaming object. Include the complete AU state object and any host/serialization copies in the peak-memory test model. The exact number of copies is unknown and must be measured only in an authorized host prototype.

### 5.2 VST3

The adapter may issue bounded `IBStream` operations and must account for partial transfers and checked offsets. Do not allocate one buffer equal to the total state size merely because the stream is seekable. Do not assume the host itself streams incrementally; record observed behavior and peak resident memory in an authorized prototype.

### 5.3 Host process mode

Bitwig hosting mode can change the process boundary and resource profile. Record the mode and whether the plug-in is in-process or isolated. Separate plug-in-owned peak memory from host/process/IPC copies in the evidence report; do not invent a fixed copy multiplier.

### 5.4 Required invariant

The Audio Thread must never wait for serialization, decompression, checksums, disk I/O, host state streams or process IPC. Capture a generation-consistent immutable snapshot, prepare and validate the restore candidate off the render path, reserve peak coexistence memory, and publish only through the existing safe-point transaction. A failed or incomplete restore preserves the previous valid runtime and required DEC-025/DEC-042 payloads.

The current planning model remains:

`M_peak = M_old + M_snapshot + M_candidate + M_host_or_stream_staging + M_decode_or_codec + M_retirement`

The host/stream staging term is deliberately unknown. No numeric ceiling is proposed.

## 6. Phase 5 host-prototype test plan (not executed)

Use the same deterministic logical fixture in every applicable host:

- four layers with distinct parameter sentinels;
- one unsaved recording payload and one DEC-042 Freeze snapshot per selected layer;
- stable chunk IDs, declared lengths and digests;
- one external library sample reference to exercise relink without dropping unrelated state;
- no active MIDI identities or transport snapshots in the serialized payload.

| Test family | Host-specific stimulus | Required observation |
|---|---|---|
| AU preset/document scope | Logic AU: save/load user setting, save/reopen project; exercise `fullState` and `fullStateForDocument` behavior including fallback | Record which property path is used, state byte count, copies/materialization, callback thread/context and exact payload hashes. |
| Ableton opaque block | Live AU and VST3: save/reopen Set with deterministic embedded audio; separately save a Rack/preset and use Collect All and Save | Verify binary-block round-trip and exact hashes; distinguish plug-in state from plug-in binary installation and from parameter-configuration defaults. |
| Bitwig project state | Bitwig VST3: save/reopen in each selected hosting mode; controlled Reload Plug-in path in an authorized prototype | Record project `plugin-states` artifacts, host mode, process boundary, payload hashes, memory and state restore result. |
| REAPER project and VST bank | REAPER AU/VST3: save/reopen project, save/load VST bank where applicable, compare default versus enabled Hard reset, and offline/online FX load | Record opaque state round-trip separately from FX-chain/preset behavior and lifecycle reset calls. |
| VST3 stream progress | Inject partial read/write, zero progress, end-of-stream, seek failure and logical chunks crossing the int32 per-call boundary | No truncation or false success; preserve checked int64 positions and the previous active runtime. |
| Peak memory and callback context | Run only after explicit host-prototype authorization and approved test-size ladder | Record total payload, peak plug-in and process memory, state method thread, elapsed time and host mode; do not call this a benchmark unless separately authorized. |
| Failure and recovery | Interrupt or corrupt state, force memory-admission rejection, reopen the project | Incomplete state is rejected; required audio is never silently omitted; previous valid state remains authoritative. |

No exact payload sizes, callback deadlines, memory thresholds or pass results are assigned here. The test-size ladder and host prototype require explicit authorization; benchmark execution remains separately prohibited without explicit approval.

## 7. Product consistency and open gates

Unchanged:

- DEC-025: unsaved recording buffers required by the DAW document are embedded; library samples remain external references with relink/collect support.
- DEC-042: Source Snapshot Freeze audio and metadata persist; reset/reload does not silently thaw or delete it.
- DEC-020, DEC-026, DEC-034 and DEC-038 remain unchanged.
- The R1 inventory remains 124 requirements; the existing acceptance/release-gate link baseline remains 53/124.
- No AC IDs, host certifications, parameter IDs, numeric limits or compatibility promises were added.

Still open:

- Q-T15: final container framing, codec, checksum, chunk boundaries, maximum state size and migration retention.
- Q-P09: exact host versions/builds and host-specific persistence behavior.
- AU: accepted binary value representation, materialization/copy behavior and maximum practical document payload.
- VST3: host stream buffering, maximum total payload and callback timing.
- User-preset inclusion of unsaved recordings/Freeze remains distinct from the DEC-025 DAW-document requirement.
- AR-06: audible treatment of voices already active at restore commit.

This audit closes none of these gates. It narrows the evidence: Ableton documents an opaque per-instance binary state block, while the remaining hosts expose general persistence mechanisms without a public size guarantee for the required embedded audio.

## 8. Official source register

Checked 2026-11-28.

### Apple / Logic Pro / AU
- [AUAudioUnit.fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate)
- [AUAudioUnit.fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument)
- [AUAudioUnit class](https://developer.apple.com/documentation/audiotoolbox/auaudiounit)
- [Logic Pro — Work with plug-in settings](https://support.apple.com/en-il/guide/logicpro/lgcp4dcb0092/mac)

### Ableton Live
- [VST/AU plug-ins reset to default preset — binary state block](https://help.ableton.com/hc/en-us/articles/115001443850-VST-AU-plug-ins-reset-to-default-preset)
- [Live Manual 12 — Working with Instruments and Effects](https://www.ableton.com/en/manual/working-with-instruments-and-effects/)
- [Collect All and Save](https://help.ableton.com/hc/en-us/articles/209775645-Collect-All-and-Save)
- [Saving plug-in parameter configurations](https://help.ableton.com/hc/en-us/articles/209073089-Saving-plug-in-parameter-configurations)

### Bitwig Studio
- [Working with Projects and Exporting](https://www.bitwig.com/userguide/latest/working_with_projects_and_exporting/)
- [Plug-in Handling and Options](https://www.bitwig.com/userguide/latest/vst_plug-in_handling_and_options/)

### REAPER
- [REAPER User Guide v7.86 — November 2026](https://dlx.reaper.fm/userguide/html/)

### Steinberg VST3
- [VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html)
- [IBStream](https://steinbergmedia.github.io/vst3_doc/base/classSteinberg_1_1IBStream.html)
- [IComponent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html)
- [IEditController](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IEditController.html)


## 9. Callback context and failure-signalling refinement — 2026-11-28

The [Cross-Host State Callback & Snapshot Freshness Evidence Matrix](PHASE3_CROSS_HOST_STATE_CALLBACK_AND_SNAPSHOT_FRESHNESS_EVIDENCE_MATRIX.md) adds the following distinctions:

- AU's dictionary properties have no reviewed public callback-thread, deadline or explicit getter-failure contract. Optional \`nil\` is not proven to abort a host project save. This is especially important because DEC-025 forbids silently omitting required unsaved recording payloads.
- VST3 component state calls are annotated UI-thread and permitted while Processing. Processing may continue concurrently; the component must serialize an immutable generation rather than mutable render-owned state.
- Steinberg's developer-forum clarification warns that offline get/set-state may execute on the process thread. This is supplemental evidence, not a guarantee for Logic, Live, Bitwig or REAPER.
- The reviewed host guides do not publish exact AU/VST3 state-callback threads or failure propagation. Project persistence and an opaque binary state block do not prove callback freshness or save rollback.
- A cross-source capture token must be coherent across every required recording/Freeze source. The serializer rejects stale descriptors and never substitutes an older snapshot.

The AU nil-result path and VST3 host reaction to non-success \`tresult\` remain Q-T15/Q-P09 prototype gates. No callback timing, maximum payload, copy count or host certification is approved.

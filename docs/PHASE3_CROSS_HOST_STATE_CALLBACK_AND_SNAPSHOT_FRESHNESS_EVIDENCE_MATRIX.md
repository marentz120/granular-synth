# Phase 3 Cross-Host State Callback & Snapshot Freshness Evidence Matrix

**Version:** 0.1 — callback context, failure signalling and coherent capture boundaries  
**Checked:** 2026-11-28  
**Status:** Official-documentation evidence map and Phase 5 test design only. No host callback was observed, no host is certified, and no callback deadline, payload limit, container ABI or memory budget is approved.  
**Baseline:** DEC-025, DEC-042, Q-T15, Q-P09, GS-NFR-001/008, AR-06; `PHASE3_STATE_IO_SCHEDULING_AND_PEAK_MEMORY_ADMISSION_AUDIT.md`; `PHASE3_AU_VST3_HOST_PERSISTENCE_FEASIBILITY_AUDIT.md`; `THREADING_AND_OWNERSHIP_SPECIFICATION.md`.  
**Scope:** Logic Pro/AU, Ableton Live/AU+VST3, Bitwig/VST3 and REAPER/AU+VST3. Research and contract refinement only; no implementation, host prototype, benchmark or runtime measurement.

## 1. Executive findings

The evidence separates into three layers that must not be conflated:

1. **Format API semantics.** AU exposes dictionary-shaped state properties without a documented callback-thread or explicit property-getter failure contract. VST3 exposes component/controller state methods and an explicit stream/result interface.
2. **Host persistence evidence.** Logic, Live, Bitwig and REAPER document project, preset, plug-in-state or process-recovery behavior. The reviewed host documentation does not establish the exact thread, deadline, failure propagation or snapshot-freshness behavior of their AU/VST3 state callbacks.
3. **Observed behavior.** No GRANULAR SYNTH host callback has been observed. All host-specific callback-context and freshness results remain unverified until an authorized prototype is run.

The most consequential refinement is the VST3 concurrency boundary: Steinberg's API documents state calls as UI-thread operations and permits component state calls while the component is in Processing. This does not mean audio processing is stopped. A Steinberg developer-forum clarification additionally warns that offline get/set-state calls may occur on the same thread as process. The latter is supplemental vendor clarification, not a universal host guarantee.

For AU, Apple's public `fullState` and `fullStateForDocument` descriptions do not specify a callback thread, callback deadline or explicit failure result for the optional dictionary property. Returning `nil` must not be assumed to make the DAW's project save fail.

A successful DEC-025 document save therefore needs a **fresh, coherent, immutable capture descriptor**. A stale prepared descriptor must never be silently returned as successful state. Where the format API does not expose a proven failure channel, the target host's handling is a compatibility gate—not permission to omit required audio.

## 2. Evidence vocabulary

| Label | Meaning |
|---|---|
| **D — documented** | Official host/API documentation explicitly describes the stated capability. It does not certify GRANULAR SYNTH. |
| **API — format contract** | The plug-in format specifies an interface or lifecycle annotation; this does not prove a host's concrete callback schedule. |
| **S — supplemental** | Vendor developer-forum clarification or other non-normative evidence. Useful for risk analysis, not a host certification. |
| **U — unverified** | The reviewed official sources do not establish the exact callback, failure or freshness behavior. |
| **N/A** | The format is not a target for that host. |

A project-save statement, an opaque plug-in-state block, an API thread annotation and an observed callback trace are four different kinds of evidence. None substitutes for the others.

## 3. Format API callback contract

### 3.1 AU / Audio Unit

Apple documents:

- `AUAudioUnit.fullState` as an optional persistable `[String: Any]` snapshot suitable for a user preset; a host may use it for a preset or a document.
- `AUAudioUnit.fullStateForDocument` as the document-oriented state property. A subclass that does not implement it interfaces with `fullState`.
- The base `fullState` implementation saves parameter-tree values.

The reviewed API references do **not** specify:

- a universal thread on which a host reads or writes either property;
- whether the getter can be called while render is active or from a realtime context;
- a callback deadline or interruption contract;
- a maximum dictionary/Data payload size or copy count;
- whether a getter returning `nil` aborts the host's project save, omits only the plug-in state, or invokes a fallback;
- a transactional error result for the property setter during restore.

The property type is optional, but optionality is not a documented save-failure protocol. Do not infer that `nil` is a reliable error signal. `presetState(for:)` is a separate throwing API and does not establish an error channel for the `fullState` property getter.

**Project consequence:** prepare an immutable document-state dictionary off-render and make its recording/Freeze chunks generation-consistent. The AU adapter must not return a stale dictionary as a successful DEC-025 save. Until a host prototype establishes how a failed/nil document-state result is handled, the AU failure path remains unproven.

Sources: [AUAudioUnit.fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate), [AUAudioUnit.fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument), [AUAudioUnit](https://developer.apple.com/documentation/audiotoolbox/auaudiounit).

### 3.2 VST3

Steinberg documents:

- `IComponent::getState/setState` as UI-thread calls, permitted in lifecycle states that include Processing.
- `IEditController::getState/setState/setComponentState` as UI-thread calls in the Connected state.
- Save order: component `getState`, then controller `getState`.
- Restore order: component `setState`, controller `setComponentState`, then controller `setState`.
- The general VST3 threading model places exported plug-in calls on the UI thread except `IAudioProcessor::process` and `IAudioProcessor::setProcessing`, which may be called on an audio thread.

Interpretation:

- “Permitted while Processing” does not mean processing is suspended. The audio process may continue while a UI-thread state callback runs.
- The component state callback must not mutate render-owned graphs, voices, note maps, recording writers or audio buffers directly.
- A candidate state must be parsed/validated separately and published through the existing safe-point/generation contract.
- `IBStream` and `tresult` provide a transfer/result surface, but the reviewed host documentation does not establish how each target host reacts to a failed `getState`, short transfer or invalid `setState`.

**Offline caveat (supplemental):** a Steinberg developer-forum response states that in an offline context, `getState/setState` may be called on the same thread as `process`. Treat this as a conservative scheduling warning, not as a guarantee for every SDK version or host. The implementation must not assume that every state callback is safely off the processing thread merely because the realtime API annotation says UI thread.

Sources: [VST3 API threading model](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html), [IComponent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html), [IEditController](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IEditController.html), [VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html), [Steinberg offline-thread clarification](https://forums.steinberg.net/t/is-icomponent-getstate-setstate-thread-safe/201734/2).

## 4. Host × format callback evidence matrix

| Host / format | Positive persistence evidence | Callback thread/context | Stale-state failure signal | Current classification |
|---|---|---|---|---|
| **Logic Pro / AU** | Plug-in settings are stored with the project and recalled on reopen. | No callback-thread or render-concurrency contract found in the reviewed Logic/Apple sources. | AU property returns an optional dictionary; Logic's behavior for `nil`/stale state is unverified. | Project setting **D**; AU API **API**; callback/failure/freshness **U**. |
| **Ableton Live / AU** | Live documents a reserved binary plug-in-state block in the Set, read/written by the plug-in. | No AU state callback thread/deadline documented. | AU property has no documented explicit error result; Live's handling of nil/stale document state is unverified. | Opaque state path **D**; callback/failure/freshness **U**. |
| **Ableton Live / VST3** | Same documented reserved binary state block; Live supports VST3. | VST3 API annotations apply, but Live's actual callback thread and offline behavior are not documented by the reviewed Live pages. | `tresult` exists; Live's project-save response to failure/short stream is unverified. | Host state path **D**; VST3 API **API**; callback/failure/freshness **U**. |
| **Bitwig Studio / VST3** | Project-associated `plugin-states` storage and multiple plug-in hosting modes are documented. | No official Bitwig state-callback thread contract found. Hosting mode changes process isolation, not the VST3 callback contract. | VST3 `tresult` exists; Bitwig's save failure and stale-snapshot handling are unverified. | Project storage/hosting modes **D**; callback/failure/freshness **U**. |
| **REAPER / AU** | AU FX and project/FX persistence are documented. | No AU state callback thread contract found. | AU nil/property-set failure behavior in REAPER is unverified. | General persistence **D/P**; callback/failure/freshness **U**. |
| **REAPER / VST3** | FX-chain/preset controls and optional “Hard reset on playback start” compatibility setting are documented. | No REAPER VST3 state callback thread contract found. | VST3 `tresult` exists; REAPER's project-save handling of failure/short stream is unverified. | FX/preset controls **D/P**; callback/failure/freshness **U**. |
| **Standalone** | The application owns its project/preset file path. | A dedicated non-render file worker can be explicitly owned by the application; this is not DAW-host evidence. | File transaction can report an application-level error, subject to the standalone file-owner contract. | Application-controlled design; separate from host certification. |

### 4.1 Host-specific cautions

- **Logic Pro:** project recall proves a persistence path, not the thread or failure semantics of the AU state property.
- **Ableton Live:** the reserved binary block proves an opaque per-instance state path. It does not prove a maximum size, callback context or failure propagation.
- **Bitwig:** its documented hosting modes range from in-process to isolated plug-in processes and trade resource use against isolation. Record the selected mode and account for process-level memory in any future authorized prototype. The guide does not document the state callback thread.
- **REAPER:** “Hard reset on playback start” is an optional compatibility setting. It must not be treated as proof of the thread or ordering of generic project-state callbacks.
- No host-specific callback thread is marked **D** in this matrix.

## 5. Snapshot freshness and capture linearization

The State I/O audit already requires per-source committed-frame watermarks. This matrix adds an explicit cross-source consistency requirement: the watermarks must be observed as one coherent capture-boundary vector, not as unrelated reads that can produce a torn multi-source snapshot.

### 5.1 Candidate capture-boundary token

A conceptual immutable `CaptureBoundaryToken` contains:

- the persistent `RuntimeStateEpoch`;
- a monotonically increasing capture/publication sequence;
- for every required recording source, its source generation and latest fully committed frame/page watermark;
- the source-generation identity for every required Freeze payload.

This is a documentation-level model, not a public API or ABI commitment. The implementation mechanism (immutable publication, versioned table or another bounded technique) remains open.

### 5.2 Linearization rule

1. The state adapter obtains one coherent published capture-boundary token at the start of the state-capture decision.
2. A prepared descriptor is eligible only if its persistent-state epoch and every required source-generation/watermark entry match that token.
3. A commit published after the token's linearization point belongs to a later capture boundary; it does not retroactively invalidate the token already captured. A commit published before the token must be represented.
4. If the token cannot be read coherently, a required source generation changed, or the prepared descriptor is stale, the adapter must not report a successful DEC-025 document save using that descriptor.
5. A bounded refresh is permitted only if the actual host callback context allows it and all required leases/resources are already obtainable without waiting for render. Otherwise the operation must fail safely; it must not stop or wait for the audio callback.
6. All required recording and Freeze chunks are one document-state transaction. A partial source set is not a successful save.

Exact sequence representation, retry count, lock-free primitive and callback-time work budget remain engineering choices. Do not add a spin/retry loop to a potentially realtime callback.

### 5.3 Why a coherent vector matters

With four independent recording sources, reading source A's watermark, then source B's, while commits continue can combine values from different publication moments. The resulting descriptor may be internally inconsistent even if each individual watermark was valid. A cross-source publication sequence or equivalent coherent snapshot mechanism must make this detectable. The mechanism is not selected here.

## 6. Failure-signalling contract and compatibility blocker

| Condition | Required product/architecture behavior | Format/host evidence |
|---|---|---|
| Fresh prepared descriptor | Serialize/return the complete required document state. | API path exists; host round-trip still requires prototype. |
| Stale descriptor | Never return it as a successful DEC-025 save. | AU nil semantics and VST3 host reaction to failed `tresult` are U. |
| Missing required recording/Freeze chunk | Reject the complete state transaction; never silently omit the chunk. | Host failure propagation U. |
| VST3 short/zero-progress stream transfer | Treat as incomplete; do not emit a valid completion footer. | Plug-in can observe transfer result; host's project-save response U. |
| AU getter cannot provide fresh state | Do not substitute an older snapshot. | No documented property-getter error result; nil behavior must be observed per host. |
| Invalid restored state | Preserve the prior active runtime; do not publish a partial candidate. | AU setter failure reporting and VST3 host handling require validation. |

**AU-specific compatibility gate:** the AU property surface does not document a result code for a failed getter. If a target host treats `nil` as “no plug-in state” and still completes the DAW project save, that behavior cannot satisfy DEC-025 for an active unsaved recording. The host must be shown to reject the save or another supported failure path must be established. Do not return stale state or weaken the product requirement to work around this gap.

**VST3-specific compatibility gate:** the adapter can return a non-success `tresult`, but the host's visible save failure, rollback and retry behavior must be observed. A return code alone is not proof that the DAW project remains unchanged.

These are technical compatibility gates, not new product decisions.

## 7. Threading and memory consequences

- State serialization reads an immutable descriptor; it never traverses the live recording writer or mutable render graph.
- A VST3 UI-thread state call may overlap audio processing. No lock, condition-variable wait or synchronous audio-thread acknowledgement is permitted.
- In offline VST3 mode, state calls may share the process thread according to the supplemental Steinberg clarification. The adapter must not assume a background worker can be synchronously joined from that context.
- AU callback context is unknown. Use the same conservative ownership boundary until each target host is measured: no direct mutation of render-owned data and no unbounded work in a callback that might be realtime.
- AU dictionary/Data materialization, bridging and host copies are unknown. Count observed plug-in-owned copies; do not invent a fixed copy multiplier for host-owned memory.
- VST3 plug-in-owned transfer buffers are counted at their allocated capacity. Host-owned IBStream buffering is unknown and must not be charged as plug-in-owned memory without evidence.
- Bitwig hosting mode changes process isolation and may change where RSS appears. Record per-process and aggregate memory in a future authorized prototype; do not treat sandboxing as free memory or as a callback-thread guarantee.
- No numeric callback deadline, payload size, copy count, process-memory budget or concurrency limit is selected.

## 8. Phase 5 fixture families (not executed)

| Fixture family | Stimulus | Required assertion | Gate |
|---|---|---|---|
| VST3 realtime state concurrency | Call component getState/setState while process continues | Render never waits or reads a partially published candidate; state reads one immutable generation | Q-T15; VST3 host prototype |
| VST3 offline same-thread path | Exercise getState/setState in offline processing context | No assumption of a separate UI/worker thread; bounded safe behavior | Q-P09/Q-T15; host prototype |
| AU callback-context discovery | Save/restore with render active in Logic, Live and REAPER | Record actual callback thread, process overlap and caller context | Q-P09; AU host prototypes |
| AU nil/failure semantics | Force absence of a fresh descriptor and observe property getter/host save result | Host must not complete a successful project save with required DEC-025 audio silently omitted | DEC-025/Q-P09; AU host prototype |
| VST3 result propagation | Inject getState failure, short/zero write and invalid setState | Record host-visible save/restore failure and prior-project/runtime preservation | Q-T15; VST3 host prototype |
| Coherent multi-source watermark | Commit four sources at staggered boundaries while capture token is read | Descriptor is one coherent vector; no torn combination | Q-T15; capture prototype |
| Stale descriptor race | Advance one source watermark after prepared descriptor publication but before callback entry | Stale descriptor is rejected; no stale success | Q-T15/Q-P09 |
| Post-boundary commit | Publish a new recording commit after capture token linearizes | Current descriptor matches the captured token; next capture includes the later commit | Q-T15 |
| Bitwig hosting-mode accounting | Repeat persistence fixture in selected hosting modes | Record mode, process count, per-process/aggregate memory and round-trip hashes | Q-P09/Q-T15; authorized prototype |
| REAPER reset distinction | Compare Hard reset on playback start enabled/disabled with project save/reopen | Do not infer generic state callback ordering from the compatibility option | Q-P09; REAPER prototype |
| Ableton opaque-state round-trip | Save/reopen AU and VST3 with deterministic required audio payload | Exact payload hash and host-visible failure behavior | Q-T15/Q-P09 |
| Logic project round-trip | Save/reopen AU project with deterministic recording/Freeze payload | Exact payload hash and nil/failure behavior | Q-T15/Q-P09 |

No test IDs, AC IDs, pass results, numeric thresholds or host certifications are created. Host prototypes and benchmarks remain separately gated.

## 9. Open gates and product consistency

| Gate | Status after this audit |
|---|---|
| DEC-025 | Unchanged: every required unsaved recording buffer must be embedded in DAW document state; no silent omission or stale success. |
| DEC-042 | Unchanged: Source Snapshot Freeze persists; mixed-layer preparation-failure policy remains separately owner-gated. |
| Q-T15 | Open: container, callback scheduling, freshness mechanism, AU failure path, VST3 failure propagation, size/codec/checksum and migration. |
| Q-P09 | Open: exact host versions/builds, callback contexts, payload round-trip, process modes and failure behavior. |
| AR-06 | Open owner decision: audible treatment of old voices during restore. This audit does not decide it. |
| User-preset payload scope | Open and distinct from DEC-025 document-state requirements. |

The 124-row R1 requirement inventory and the 53/124 acceptance/release-gate link baseline are unchanged. No product behavior, AC ID, public API/ABI, numeric limit or owner decision is added.

## 10. Primary source register

### Apple / AU / Logic
- [AUAudioUnit.fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate)
- [AUAudioUnit.fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument)
- [AUAudioUnit](https://developer.apple.com/documentation/audiotoolbox/auaudiounit)
- [Logic Pro — Work with plug-in settings](https://support.apple.com/en-il/guide/logicpro/lgcp4dcb0092/mac)

### Ableton Live
- [VST/AU plug-ins reset to default preset — reserved binary state block](https://help.ableton.com/hc/en-us/articles/115001443850-VST-AU-plug-ins-reset-to-default-preset)
- [Live Manual 12 — Working with Instruments and Effects](https://www.ableton.com/en/manual/working-with-instruments-and-effects/)

### Bitwig Studio
- [Plug-in Handling and Options — hosting modes and reload](https://www.bitwig.com/userguide/latest/vst_plug-in_handling_and_options/)
- [Working with Projects and Exporting](https://www.bitwig.com/userguide/latest/working_with_projects_and_exporting/)

### REAPER
- [REAPER User Guide v7.86 — November 2026](https://dlx.reaper.fm/userguide/html/)

### Steinberg VST3
- [VST3 API documentation and threading model](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html)
- [IComponent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html)
- [IEditController](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IEditController.html)
- [VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html)
- [Steinberg developer clarification — offline get/setState thread](https://forums.steinberg.net/t/is-icomponent-getstate-setstate-thread-safe/201734/2)


## 11. AU failure-signalling refinement — 2026-11-28

The earlier shorthand “AU has no explicit getter error result” is corrected by separating AUv3 and AUv2 surfaces:

- AUv3 **fullState** and **fullStateForDocument** are nullable dictionary properties bridged to AUv2 ClassInfo properties. Their public signatures do not expose a separate OSStatus, throws or NSError result. Apple does not document the exact nil-to-AUv2-status mapping or host save behavior.
- AUv2 **AudioUnitGetProperty** returns an OSStatus. This is an explicit API-level result, but it does not establish whether Logic, Live or REAPER aborts a document save on failure.
- Apple's **ClassInfoFromDocument** documentation specifies a **restore** fallback: if the document property errors or is not implemented, the host should use the same preset with **ClassInfo**. This must not be generalized into a save-time fallback or rollback rule.
- The fallback can be unsafe for DEC-025 if the preset-scoped state lacks the required unsaved-recording/Freeze chunks. A parameter-only fallback is not a complete document restore.
- AUv2 **AudioUnitGetProperty** returns an owned CF object reference that the caller must release. This does not prove a deep copy of nested audio Data; memory accounting follows actual backing allocations and observed materialization.

Save failure, restore fallback, AUv3 nil bridging and host-visible rollback are separate observations. No host behavior is certified. See [AU State Failure Signalling & DEC-025 Compatibility Audit](PHASE3_AU_STATE_FAILURE_SIGNALLING_AND_DEC025_COMPATIBILITY_AUDIT.md).

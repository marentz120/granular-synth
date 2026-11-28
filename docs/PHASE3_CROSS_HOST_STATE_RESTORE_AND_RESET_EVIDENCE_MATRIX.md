# Phase 3 Cross-Host State Restore & Reset Evidence Matrix

**Date:** 2026-11-28 · **Version:** 0.1  
**Status:** Official-documentation evidence map and Phase 5 test design only. No host is certified; no host prototype, implementation, benchmark, state-size limit or audible restore policy is approved.  
**Authority:** DEC-025 (unsaved recordings embedded; library samples referenced externally with relink/collect), DEC-042 (Source Snapshot Freeze persistence and reset isolation), DEC-034/038, GS-NFR-001, Q-P06/Q-P09/Q-T15 and AR-06.  
**Research boundary:** Vendor documentation checked 2026-11-28. Exact supported host versions/builds remain open. API capability, host documentation and observed product behavior are separate evidence classes.

## 1. Evidence vocabulary

| Label | Meaning |
|---|---|
| **D — documented** | Official documentation explicitly describes the general host or API behavior. It does not certify GRANULAR SYNTH. |
| **P — partial** | A related host capability is documented, but the complete plug-in state, payload or lifecycle contract is not. |
| **U — unverified** | No sufficient official evidence was found for the exact behavior; an owner-authorized host test is required. |
| **API only** | The plug-in format exposes an API contract, but the host's implementation and timing are not established by that API page. |
| **N/A** | The format is not a target for that host (for example, Bitwig/AU). |

A host-level “save project” statement is not proof that a particular host serializes every opaque plug-in payload, honors a large embedded recording, restores optional bus activation, or invokes callbacks in a specific order. No **D** entry means that GRANULAR SYNTH has passed a test.

## 2. Non-negotiable product and architecture invariants

- **DEC-025:** embed every unsaved recording buffer required by the product in plug-in state; keep existing library samples as external references with relink/collect support. Do not silently omit, truncate or replace embedded audio.
- **DEC-042:** Freeze is an immutable Source Snapshot; the snapshot and its audio persist under DEC-025. MIDI reset, Panic, host processing stop and project restore must not silently thaw or delete the snapshot.
- Persistent patch/layer/project state, recording payloads and Freeze payloads are distinct from transient runtime state.
- Never persist SourceNoteIdentity, NoteInstanceId, AU cable/member-channel leases, VST3 event-bus/noteId lookup entries, active voice handles, callback pointers, transport snapshots, stream formats or render scratch.
- Restore must validate and prepare a complete candidate off the render thread, reserve old-plus-new peak memory, publish atomically at a safe render boundary, and preserve the prior active state on failure.
- A restored VST3 controller must mirror processor state without generating duplicate host parameter writes.
- **AR-06 remains open:** if a restore commits while voices are active, either terminate old voices at commit or let them finish as non-addressable tails under the old immutable runtime generation. This is audible product behavior; this matrix deliberately specifies both test branches and selects neither.
- No host version, component subtype, callback deadline, maximum state size, memory ceiling or reset policy is approved here.

## 3. Format API contract — separate from host evidence

### 3.1 AU / Audio Unit

Apple documents `AUAudioUnit.fullState` as a persistable snapshot suitable for a user preset; hosts may use it for a preset or document. The unit should not persist transient properties such as stream formats, but should save and restore other persistent properties. Apple also exposes the document-oriented `fullStateForDocument` property.

Implications for this project:

- Map preset state and document/session state deliberately; do not assume the host treats both properties identically.
- DEC-025's unsaved recording and DEC-042's Freeze payload still need to be represented in the chosen document state. The API description alone does not prove that Logic or another AU host accepts the required payload size or timing.
- Exact callback thread, deadline, interruption and large-payload behavior remain host-prototype questions.
- AU `reset()` and render-resource deallocation are lifecycle operations, not permission to delete persistent recording/Freeze state.

Sources: [Apple AUAudioUnit.fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate) · [Apple AUAudioUnit.fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument) · [Apple AUAudioUnit](https://developer.apple.com/documentation/audiotoolbox/auaudiounit).

### 3.2 VST3

Steinberg documents two persistent states: processor/component state and controller state. The documented save sequence is component `getState`, then controller `getState`. The documented restore sequence is:

1. `IComponent::setState(componentState)`
2. `IEditController::setComponentState(componentState)`
3. `IEditController::setState(controllerState)`

The controller receives the processor state so it can synchronize its parameter representation. It must not emit duplicate host parameter writes during that synchronization.

The `IComponent` reference marks `setState` / `getState` as UI-thread calls and lists permitted lifecycle states including Processing; `activateBus` and `setActive` have their own setup-state constraints. This does **not** establish a universal host callback sequence, a bounded state-stream size, or permission to mutate render-owned objects directly.

Sources: [Steinberg VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html) · [Steinberg IComponent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html) · [Steinberg VST3 API documentation](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html).

## 4. Host × format evidence matrix

| Host / format | Project/session state evidence | Preset/reset evidence | Host process/reset evidence | DEC-025 embedded audio / DEC-042 Freeze payload | Current classification |
|---|---|---|---|---|---|
| **Logic Pro / AU** | Logic's guide says plug-in settings are stored with the project and recalled when the project is next opened. | Logic documents loading/saving plug-in settings and a **Reset Setting** command. This is a user preset/setting action, not proof of the AU `reset()` callback sequence. | AU lifecycle APIs exist; Logic's exact callback order, caller context and behavior during project restore are not established by the reviewed host guide. | Host-level acceptance of large custom embedded recording/Freeze payloads and failure reporting is not documented by the reviewed pages. | Project plug-in settings **D**; UI setting reset **D**; AU API **API only**; custom payload and callback lifecycle **U**. |
| **Ableton Live / AU + VST3** | Live's manual says saving a Live Set saves device/control settings. Live documents per-instance VST preset banks; the exact opaque third-party state round-trip sequence is not specified by that statement. | VST preset/bank save is documented. No generic host command proving an AU/VST3 processor reset callback or reset semantics was found. | Plug-in scanning/rescan is documented, but that is discovery/reload, not proof of processing-stop/reset callback order. | Ableton says third-party plug-ins themselves are not collected into the project folder by Collect All and Save. This is a portability limitation for the plug-in binary, not evidence that the plug-in's serialized state is absent from the Live Set. DEC-025 audio payload round-trip remains unverified. | Set/device settings **D**; per-instance VST preset behavior **D**; opaque state and embedded audio **P/U**; reset callback **U**. |
| **Bitwig Studio / VST3** | Bitwig documents a project folder with generated subfolders including `plugin-states`, recordings and master recordings; the Project Panel reports missing plug-ins and version conflicts. | Plug-in presets are documented; no generic reset-to-default behavior for arbitrary third-party plug-ins was found. | Bitwig documents sandbox/hosting modes and a Reload Plug-in action after a plug-in crash. Changing hosting mode for an already loaded project requires reopening the project or reloading the audio engine. These are process lifecycle controls, not a guarantee of state restore order. | A project-associated plug-in-state storage location is documented, but the reviewed guide does not specify opaque chunk limits, DEC-025 custom recording payload acceptance, or exact processor/controller restore ordering. | Project-associated state container **D**; reload/hosting controls **D**; exact VST3 state and embedded payload round-trip **P/U**; reset semantics **U**. |
| **REAPER / AU + VST3** | REAPER's guide documents saving FX chains with parameter settings, project templates containing tracks/FX/routing, and project save/backup behavior. The reviewed user-guide passages do not specify a complete opaque AU/VST3 state-chunk contract for this product. | REAPER documents FX-chain/preset save. For VST/VST3 it also exposes a compatibility option named **Hard reset on playback start**; it is optional and is not a universal project-restore rule. | Project loading can be configured to bring FX online after load or open them offline for recovery. Exact AU/VST3 callback ordering and whether the optional hard-reset setting calls the same lifecycle path as normal stop/start remain unverified. | Exact DEC-025 recording/Freeze payload acceptance, size limits, corruption handling and round-trip are unverified. | FX chain/preset behavior **D**; project opaque state **P**; optional hard-reset/offline-load controls **P**; custom payload and callback lifecycle **U**. |
| **Bitwig / AU** | — | — | — | — | **N/A** — Bitwig's documented plug-in path for this project is VST3; AU is not part of this host cell. |

### Interpretation

1. Logic provides the clearest direct host statement for plug-in setting persistence on project reopen. It still does not prove the required large DEC-025 payload or bus-activation persistence.
2. Ableton documents Live Set device/control persistence and per-instance VST preset behavior. Do not confuse the Live Set's saved plug-in state with collecting the third-party plug-in binary: Ableton explicitly says third-party plug-ins are not copied by Collect All and Save.
3. Bitwig's `plugin-states` project folder is strong evidence that project-associated plug-in state is stored as project content. Its sandbox reload controls prove a process-reload path, not that an active voice, note identity or custom audio chunk survives exactly as required.
4. REAPER documents FX-chain parameter settings and an optional VST/VST3 hard-reset-on-playback-start compatibility setting. The reviewed documentation is not sufficient to claim the exact opaque state-stream and restore contract for this product.
5. A host's **Reset Setting**, **Hard reset on playback start**, plug-in crash reload, transport stop, component deactivation and project-state restore are different operations. Never map all of them to one destructive `clearAll()` path.
6. Existing bus activation/routing save-and-reopen status remains **U** for the exact GRANULAR SYNTH topology in all host cells. This matrix does not upgrade the multi-output evidence matrix.

## 5. Host-specific test plan (not executed)

Every test must record exact host product/version/build, macOS version, architecture, plug-in format and identifier, component version, host plug-in hosting mode (where applicable), state schema version, payload byte counts, active bus arrangement, logs and a reproducible project/preset. Do not record a host as passing without an actual run and artifacts.

| Host / format | Required test cases | Specific observations |
|---|---|---|
| Logic Pro / AU | Save project; close/reopen; load a plug-in setting; use Reset Setting; save/reopen with optional output buses active/inactive; restore a project containing unsaved recording and Freeze payloads. | Confirm the plug-in setting is recalled; distinguish preset reset from AU reset/deallocation; verify embedded payload hashes and bus activation/routing independently. |
| Ableton Live / AU | Save/reopen a Set with the AU instrument; save/load AU preset; restore unsaved recording and Freeze payload; test missing external library sample; test Collect All and Save separately. | Confirm per-instance device state; prove custom payload is inside plug-in state and not mistakenly expected to be collected as an external third-party plug-in; verify relink/collect behavior and buses. |
| Ableton Live / VST3 | Save/reopen a Set; save/load the instance's VST preset/bank; restore embedded payload; verify configured plug-in parameters and routing; test missing plug-in/binary on another machine only as a portability test. | Verify VST component state and controller synchronization; distinguish Live Set state, VST preset/bank, and external plug-in binary availability. |
| Bitwig Studio / VST3 | Save/reopen project; inspect project-associated plug-in-state presence; test each host process/hosting mode selected for the pinned version; exercise Reload Plug-in after a controlled failure only in an authorized prototype; restore recording/Freeze payload. | Record whether the plug-in is hosted Within Bitwig, Together, By manufacturer, By plug-in or Individually; verify project state after reload/reopen without assuming sandbox mode preserves transient voices. |
| REAPER / AU | Save/reopen project; load/save FX chain; test project open with FX online/offline options; restore embedded recording/Freeze payload. | Distinguish chain preset from per-instance project state; verify AU restore and reset callback behavior in a host trace. |
| REAPER / VST3 | Save/reopen project; save/load FX chain; compare default behavior with the optional Hard reset on playback start compatibility setting; test offline/online FX load. | Record actual reset callback sequence and whether processor/controller state is restored once; verify payload hashes and optional buses. |

## 6. Shared deterministic state fixture

Use one logical fixture across all applicable hosts; keep payload bytes deterministic and identifiable.

### 6.1 State payload

- Four layers with deliberately distinct parameter sentinels and patch identities.
- One external library sample reference with a known content hash; a second copy of the fixture deliberately omits that external file to exercise relink-required behavior.
- At least one unsaved recording per applicable source path, split into multiple manifest chunks with known lengths and checksums.
- One DEC-042 Freeze snapshot per tested layer, with source-generation metadata and known audio hash.
- Project and schema version markers sufficient to exercise current-schema and one supported migration path once Q-T15 is defined.
- No active SourceNoteIdentity, NoteInstanceId, host note ID, active voice handle, stream format, transport snapshot or render scratch in the serialized manifest.

Do not invent a maximum payload size, compression policy, chunk count, migration window or deadline. Those remain Q-T15 / host-prototype gates.

### 6.2 Round-trip assertions

After host save, close/reopen and restore:

1. All four layer sentinels and persistent patch values match exactly within their defined serialization representation.
2. Every required unsaved recording and Freeze chunk is present, length-correct and checksum-valid.
3. External sample references preserve stable identity; a missing file produces explicit relink-required state and does not silently substitute another asset.
4. Runtime note identity tables start empty/invalid. No old host note ID, member-channel lease or active note is resurrected.
5. The VST3 controller mirrors the restored processor state without duplicate host parameter writes.
6. Optional bus activation, channel mapping and routing are recorded as a separate result; generic plug-in-state success does not imply bus persistence.
7. Corrupt, truncated, unsupported or memory-rejected restore leaves the prior active project state intact; partial payload success is not reported as complete.
8. Freeze remains frozen with the persisted snapshot; Panic, CC 120/121/123, host stop and process reload do not implicitly thaw it.
9. The audible treatment of voices active at restore commit is recorded against both AR-06 branches until the owner selects one.

## 7. Reset and lifecycle test taxonomy

| Stimulus | Must be tested separately from | Required invariant |
|---|---|---|
| MIDI CC 121 / 123 / 120 | Internal Panic and host reset | Existing reset distinctions in the MPE lifecycle audit remain intact; Freeze payload is not deleted or thawed. |
| AU `reset()` | `fullState` / `fullStateForDocument` restore | Clear only transient render/MPE state at a safe boundary; preserve persistent recordings and Freeze. |
| AU render-resource deallocation/reallocation | Project close/reopen | No render resource is destroyed while a callback/reader can still reference it; persistent state is retained. |
| VST3 `setProcessing(false)` | `IComponent::setState` | Invalidate transient processing state only; do not clear component/controller persistent state. |
| VST3 `setActive(false)` / setup reconfiguration | Component state restore | End one render configuration and reject stale prepared configurations; do not mutate active graph from the render callback. |
| Host plug-in reload/crash recovery | MIDI Panic and project restore | Recreate transient state empty; recover persistent state only through the host's documented state path. |
| Project/preset restore | Transport stop / reset-to-default | Use the full validated restore transaction; AR-06 controls old-voice audible behavior. |

## 8. Threading, memory and failure requirements

- Treat host state callbacks as host-owned call sites. Never infer that an API callback is always non-RT merely because a common host normally calls it from the UI thread; VST3 documentation explicitly permits component state calls in Processing state.
- State capture reads an immutable generation-consistent snapshot. It must not read mutable writer memory directly or wait for render-thread acknowledgement.
- Restore parsing, migration, checksum validation, sample resolution, decoding and graph construction occur off the render thread.
- Admission accounts for the old graph, candidate graph, embedded recording/Freeze chunks, staging and retirement backlog simultaneously. No numeric budget is set here.
- Safe-point publication is atomic. Old graph retirement and final destruction happen off RT after all readers/voices/grains release ownership.
- No render-thread allocation/deallocation, lock, blocking wait, file I/O, logging, host callback, large copy or final destruction.
- If the host requires synchronous state serialization and a valid immutable snapshot is not available within its permitted callback contract, fail explicitly; never silently omit DEC-025 audio.
- Test interruption at each save/restore phase: manifest creation, chunk write, checksum, memory admission, candidate construction, publication and retirement. A failed transaction preserves the previous valid state.

## 9. Owner and evidence gates

| Gate | What remains open |
|---|---|
| **AR-06** | Terminate active old voices at restore commit, or let them finish as non-addressable tails under the old immutable runtime generation. Owner decision required before audible pass criteria can be frozen. |
| **Q-P09** | Exact host versions/builds and certification scope. |
| **Q-P06** | AU component strategy and final plug-in topology; this matrix does not select `aumu` / `aufm` or VST3 bus types. |
| **Q-T15 / DEC-025** | Container format, schema/migration policy, embedded-audio limits, chunking and host callback constraints. |
| **Q-T07 / memory admission** | Numeric RAM/CPU/capture budgets require engineering evidence; any benchmark needs separate explicit owner authorization. |
| **Host prototype** | Exact save/reopen, callback order, bus persistence, custom audio payload and reset behavior remain unverified until a separately authorized host prototype. |

No owner question is being asked or answered by this evidence matrix. The two AR-06 branches remain test alternatives only.

## 10. Product consistency and exit condition

This work does not change DEC-020, DEC-025, DEC-026, DEC-034, DEC-038 or DEC-042; it does not change the 124-row R1 requirement inventory or the 53/124 acceptance-link baseline. It adds no new requirement, AC ID, host certification or compatibility promise.

A host/format state-restore cell can be considered validated only after an owner-authorized prototype on a pinned host build passes the persistent payload round-trip, external-reference recovery, transient-identity reset, controller synchronization (VST3), failure atomicity, relevant host reset/deactivate/reload paths and separate bus activation/routing tests. Until then this is an evidence map and test plan only.

## 11. Official source register

Checked 2026-11-28. Vendor pages are version-sensitive; pin exact host versions before certification.

### Apple / Logic Pro / AU
- Logic Pro User Guide — Work with plug-in settings: https://support.apple.com/en-bh/guide/logicpro/lgcp4dcb0092/mac
- Logic Pro User Guide — Save projects and recover auto-saved versions: https://help.apple.com/logicpro/mac/10.1/en.lproj/lgcpce128e82.html
- AUAudioUnit.fullState: https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate
- AUAudioUnit.fullStateForDocument: https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument
- AUAudioUnit lifecycle: https://developer.apple.com/documentation/audiotoolbox/auaudiounit

### Ableton Live
- Live Manual 11 — Saving and Exporting / saving a Live Set: https://www.ableton.com/en/live-manual/11/live-concepts/
- Live Manual 12 — Using Plug-Ins / VST per-instance presets and AU/VST plug-in behavior: https://www.ableton.com/en/live-manual/12/using-plug-ins/
- Ableton Help — Transferring Projects to another computer; third-party plug-ins are not collected by Collect All and Save: https://help.ableton.com/hc/en-us/articles/209071909-Transferring-Projects-to-another-computer

### Bitwig Studio
- Bitwig User Guide — Working with Projects and Exporting; project-associated `plugin-states` folder: https://www.bitwig.com/userguide/latest/working_with_projects_and_exporting/
- Bitwig User Guide — Plug-in Handling and Options; sandbox modes, Reload Plug-in and project/audio-engine reload: https://www.bitwig.com/userguide/latest/vst_plug-in_handling_and_options/
- Bitwig User Guide — VST plug-ins and device presets: https://www.bitwig.com/userguide/latest/vst_plug-ins/

### REAPER
- REAPER User Guide v7.86 (November 2026) — FX chains, project load/offline options, and VST/VST3 compatibility settings: https://dlx.reaper.fm/userguide/html/

### Plug-in API references (not host certification)
- Steinberg VST3 Persistence: https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html
- Steinberg IComponent: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html
- Steinberg VST3 API documentation: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html

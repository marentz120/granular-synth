# Phase 3 MPE Reset, State Restore & Host Lifecycle Audit

**Date:** 2026-11-28 · **Version:** 0.1  
**Status:** Evidence-backed proposal. AR-03…AR-06 remain open. No implementation, host certification, callback-thread assumption or audible restore policy is approved.  
**Authority:** DEC-025/034/038/042; GS-MID-002, GS-MOD-004, GS-NFR-001; IF-03/IF-09; Q-P09/Q-T15.

## 1. Purpose

Controller reset, note release, sound termination, host render reset and persistent state restore are different operations. They must not share one undifferentiated “clear everything” path.

## 2. Reset taxonomy

| Operation | Scope/effect | MPE identity effect | Persistent state |
|---|---|---|---|
| CC 121 Reset All Controllers | Resets controller state on the addressed channel | Keep active note identities and channel leases | Unchanged |
| CC 123 All Notes Off | Requests note release; sustain may defer it | Retain identity until actual note/voice lifecycle ends | Unchanged |
| CC 120 All Sound Off | Silences notes on the addressed channel as soon as possible | Retire only identities/voices actually terminated in that scope | Unchanged; does not thaw Freeze |
| Internal Panic | Explicit engine recovery scope | Bounded invalidation of active bindings in declared scope | Does not delete patch, recording or Freeze payloads |
| AU reset() | Resets transitory rendering state | Invalidate transient note/voice bindings at a safe boundary | Does not delete persistent state |
| VST3 setProcessing(false) | Stops processing and resets transient processor state | Advance transient note epoch | Component/controller state unchanged |
| Deactivate/reconfigure | Ends one render configuration | Invalidate old ingress leases and stale commands | Persistent project state retained |
| State restore commit | Replaces persistent runtime graph at one safe point | Old identities are not serialized or rebound | Replace only after validation/admission |

### 2.1 MIDI Channel Mode distinctions

The MIDI 1.0 Detailed Specification assigns separate messages for All Sound Off (120), Reset All Controllers (121) and All Notes Off (123). It describes CC 120 as channel-scoped silence, CC 121 as resetting controller conditions including pitch bend and pressure, and CC 123 as note release. It also states that Hold/Sustain takes priority over Note Off and All Notes Off until the pedal is released.

Consequences:
- CC 121 does not clear SourceNoteIdentity, NoteInstanceId or active-note membership as if Note Off occurred. Canonical MPE reset defaults, especially Timbre/CC74, remain unspecified.
- CC 123 is not an immediate kill. If sustain is active, identity remains until sustain is released and the voice lifecycle ends.
- CC 120 is not global Panic. Apply only to the validated cable/event-bus, channel and zone scope; do not guess what an unknown master/member-channel role means.
- CC 120, CC 121, CC 123 and Panic never thaw or erase the DEC-042 Freeze snapshot.

Sources: [MIDI 1.0 Detailed Specification, Channel Mode Messages §§20–25 and Hold Pedal notes](https://freqsound.com/SIRA/MIDI%20Specification.pdf); [MIDI Association specifications](https://midi.org/specifications).

## 3. MPE lifecycle across reset

- **CC 121:** reset controller values using eventual project defaults; preserve active source identities, layer-local voices and channel lease. Do not affect other cables, event buses or zones.
- **CC 123:** request release; if sustain is active, identity remains until sustain release and actual voice completion. AU matching remains AR-03.
- **CC 120:** terminate only notes in the validated channel scope; retire only corresponding identities after termination; later stale expression is rejected.
- **Panic:** separate bounded recovery command with reserved delivery. It must not wait for a worker or erase recordings/Freeze.
- **Zone/cable/device change:** advance only the affected IngressNamespaceGeneration. Old channel leases cannot be rebound to a reused numeric channel. Whether still-active notes release or terminate remains AR-03.

## 4. Persistent versus transient state

Persist under DEC-025/DEC-042: patch/layer/project state, external library references, all required unsaved recording buffers, Freeze snapshot audio and metadata.

Never serialize as persistent note state:
- SourceNoteIdentity and NoteInstanceId tables;
- AU cable/zone/member-channel active leases;
- VST3 event-bus/noteId lookup entries;
- active voice handles, host event pointers, callback-owned lists, transport snapshots, stream formats or render scratch.

Saving must not clear live MPE identities. Restore rebuilds transient handles empty/invalid; it must not resurrect a host note ID or a stuck note.

Apple documents AUAudioUnit.fullState as a persistable preset snapshot and says transient properties such as stream formats should not be persisted. fullStateForDocument is document-oriented. DEC-025 still requires embedded unsaved recording audio; exact AU callback/container size and timing limits remain host-prototype gates. [Apple fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate) · [Apple fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument).

Steinberg documents separate VST3 component and controller states: restore component state to the processor first, then pass it to the controller via setComponentState, followed by controller-specific state. Controller sync must not generate duplicate host parameter writes. [Steinberg VST3 Persistence](https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Persistence.html).

## 5. Restore transaction

1. Parse/validate schema, bounds, chunk lengths, checksums, references and migrations off RT.
2. Reserve peak memory for old graph + candidate graph + embedded recordings/Freeze + staging + retirement backlog.
3. Prepare the complete candidate graph/controller payload off-thread; do not mutate active state incrementally.
4. Assign a new RuntimeStateEpoch, distinct from render-configuration and ingress-namespace generations.
5. Publish one complete transaction at a safe render boundary.
6. Invalidate old transient note identities and reject internal queued events tagged with the old epoch.
7. Synchronize VST3 controller using the documented processor-first sequence.
8. Retire old resources off RT only after no reader/voice/grain can reference them.

Any parse, migration, memory-admission, checksum or publication failure preserves the prior active state. Partial restore is not success.

**Host limitation:** host events do not carry this project's RuntimeStateEpoch. Internal generations protect our own handles and queued records, but cannot distinguish a late host event from a new event if the host reuses the same external key. Host event ordering and note-ID lifetime must be observed, not assumed.

## 6. Host lifecycle constraints

| API | Documented constraint | Candidate handling | Remaining evidence |
|---|---|---|---|
| AU reset() | Resets transitory rendering state | Bounded transient reset; preserve persistent recordings/Freeze | Caller thread, timing and recorder behavior |
| AU deallocateRenderResources() | Host calls after finishing rendering | Retire old render resources only after callbacks stop | Host teardown ordering |
| VST3 setProcessing(false) | May run on UI or processing thread; light, lock-free, allocation-free; resets inner state | Bounded reset-generation signal consumed before next render | Host call sequence |
| VST3 setActive(false)/setupProcessing | New setup occurs while inactive | End old render generation; prepare next off RT | Repeated activation/setup failure |
| VST3 IComponent.setState | UI-thread call permitted in Processing state | Stage off RT; publish atomically at safe point | Host concurrency/failure reporting |
| VST3 controller state | Controller receives component state before its own state | Sync UI/parameter representation only | Save/reopen integration |

Sources: [Apple AUAudioUnit lifecycle](https://developer.apple.com/documentation/audiotoolbox/auaudiounit), [Apple deallocateRenderResources](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/deallocaterenderresources()), [Steinberg IComponent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html), [Steinberg IAudioProcessor](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IAudioProcessor.html), [Steinberg call sequence](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Workflow%2BDiagrams/Audio%2BProcessor%2BCall%2BSequence.html).

## 7. Generation, threading and memory invariants

Keep three independent monotonic generations:
- RuntimeStateEpoch — committed persistent state.
- IngressNamespaceGeneration — one AU cable, VST3 event bus, Standalone input device/port or MPE zone.
- RenderConfigurationGeneration — sample rate, maximum block, bus arrangement and workspace.

Render owns active identity maps. Host callbacks copy bounded values and publish commands; they never clear render-owned maps directly. Reset/Panic uses reserved bounded delivery. Generation invalidation may be O(1), but voice cleanup must remain bounded and preallocated. No allocation, blocking, I/O, logging, host callback or final destruction on render.

Restore memory admission counts old and candidate graphs, embedded recording/Freeze chunks, staging and retirement. Generation wrap must never alias a live handle; quiesce/reinitialize safely off RT or reject admission.

## 8. Owner and engineering gates

| Gate | Question | Status |
|---|---|---|
| AR-03 | AU same-pitch Note Off, sustain, zone reconfiguration and channel reuse | Open |
| AR-04 | VST3 Pressure/Timbre Note Expression types and host delivery | Open |
| AR-05 | Public ±48-semitone metadata and exact conversion | Open |
| AR-06 | On state restore with active voices, terminate them at commit or let them finish as non-addressable tails under the old immutable runtime generation? | Owner decision required |
| Q-P09 | Exact host versions and state/MPE behavior | Open |
| Q-T15 / DEC-025 | State container, migration and embedded-audio limits | Open |

AR-06 changes audible behavior and cannot be inferred from DEC-025 or the existing MPE contract.

## 9. Phase 5 exit condition

Prove that CC 121 preserves note identity; CC 123 respects sustain; CC 120 is channel-scoped; resets invalidate stale identities without render allocation; save excludes host note IDs but embeds required unsaved audio; valid restore is atomic; failed restore preserves prior state; stale internal events cannot cross RuntimeStateEpoch; and host external note-key reuse/order is tested rather than assumed.

Fixture definitions are not passing results. No host is certified; no numeric capacity, performance threshold or benchmark result is introduced.

## 10. Product consistency

DEC-025, DEC-034, DEC-038, DEC-042, GS-MID-002 and GS-MOD-004 remain unchanged. No new R1 requirement, MIDI feedback, named-controller template, engine, layer or compatibility promise is introduced.

No DSP, Plugin Shell, host prototype, benchmark, framework selection, parameter-ID freeze or PR merge is authorized.


## 11. Cross-host state restore and reset evidence — 2026-11-28

Host evidence is consolidated in [PHASE3_CROSS_HOST_STATE_RESTORE_AND_RESET_EVIDENCE_MATRIX.md](PHASE3_CROSS_HOST_STATE_RESTORE_AND_RESET_EVIDENCE_MATRIX.md). The evidence map distinguishes:

- Logic's documented plug-in setting recall on project reopen and its UI-level Reset Setting command;
- Ableton Live Set/device settings and per-instance VST preset banks, separately from third-party plug-in binary collection;
- Bitwig's project-associated `plugin-states` folder and sandbox/reload controls;
- REAPER's FX-chain parameter settings, offline/online project-load options and optional VST/VST3 hard-reset-on-playback-start setting.

None of these host-level statements establishes the exact custom DEC-025 recording/DEC-042 Freeze payload round-trip, bus activation persistence, callback ordering or audible treatment of active voices. The matrix therefore keeps those items unverified and preserves AR-06 as an owner decision. No host prototype or reset test was run.

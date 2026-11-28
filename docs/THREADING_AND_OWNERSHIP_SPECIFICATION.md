# Threading and Ownership Specification

**Version:** 0.6 — state callback concurrency and coherent capture publication clarified  
**Status:** Proposed; requires architecture approval. No implementation is authorized.  
**Authority:** REQ-AUD-001…003, GS-NFR-001/005/006, DEC-020/025, Q-T08/Q-T09.

## 1. Core invariants

1. The audio callback has bounded work and never performs file/network I/O, allocation/deallocation, blocking waits, mutex acquisition, logging, UI calls or unbounded system calls.
2. Every object read by the audio thread is fully constructed before publication and remains alive until the audio thread has relinquished it.
3. Destruction of objects that may have been observed by the audio thread occurs only on a non-RT thread after explicit retirement.
4. Cross-thread communication has a documented producer, consumer, capacity, ownership transfer and overflow policy.
5. No correctness property depends on the UI polling at a particular rate.
6. Audio output remains deterministic for identical engine state, input, MIDI/event stream, sample rate and render mode.

## 2. Thread inventory

| Thread/context | Responsibilities | Ownership and prohibitions |
|---|---|---|
| Audio render (host/CoreAudio) | Render all four layers, voices, sequencer events, modulation, FX and output; consume bounded commands; publish snapshots | Sole writer of live DSP runtime during processing. No blocking, allocation, destruction, file I/O, logging or UI calls. |
| Message/UI | UI, controller mapping editor, preset selection, parameter gestures, state preparation | May allocate and perform UI work; never waits for audio completion. |
| Loader/analysis | Decode samples, validate files, prepare sample buffers, peaks and optional analysis; build immutable structural objects | Never mutates objects currently used by audio. |
| Housekeeping/state I/O | Save/load, disk writes, WAV export, state compression, deferred destruction, telemetry aggregation | Never blocks or runs on audio callback. |
| MIDI/device callback | Receive device MIDI and timestamp/translate events into bounded event handoff | No UI work, file I/O or blocking. If host delivers MIDI on audio callback, process within the audio event budget. |
| Optional RT workers | Parallel rendering only if later enabled and joined to host audio workgroup | Disabled for initial R1 unless profiling proves necessary; no independent scheduling assumptions. |

A host may call lifecycle and state methods on threads other than the UI thread. Adapters must marshal work to the correct non-RT executor and must not assume callback thread identity unless the API guarantees it.

## 3. Communication topology

### 3.1 Audio commands

The existing proposal uses an SPSC command queue, but UI, loader and housekeeping are independent potential producers. A single SPSC queue cannot safely accept multiple concurrent producers.

**Proposal:** use one bounded SPSC queue per producer domain, each with one clearly designated producer and the audio thread as consumer:
- UI/message → audio
- loader → audio
- MIDI/device → audio, only if events are not delivered directly in the host's process block
- housekeeping → audio only if a real use case exists

The audio callback drains queues in a fixed, documented order and processes at most a configured number of commands per block. Commands carry fully prepared immutable objects or bounded value payloads. They never contain callbacks, heap work or deferred work for the audio thread to perform.

Queue capacities must be fixed at initialization and selected from measured worst-case producer bursts. They must not grow dynamically.

### 3.2 Overflow policy

| Message class | Proposed behavior when queue is full |
|---|---|
| Continuous parameter update | Coalesce latest value by parameter ID in a bounded atomic mailbox; do not enqueue an unbounded stream. |
| Meter/UI snapshot | Overwrite latest snapshot; stale UI data may be skipped. |
| MIDI note/event | Never silently drop without accounting. Define bounded event capacity and explicit overflow counter/panic recovery; host block events take precedence over lower-priority UI commands. |
| Structural replacement (sample/FX/patch) | Keep the newest prepared request in a non-RT pending slot or reject with visible status; do not destroy active object or block audio. |
| Stop/panic/all-sound-off | Reserved high-priority path with guaranteed bounded delivery; must not be starved by ordinary commands. |
| Recording data | Fixed ring buffer; on exhaustion stop capture safely, preserve valid captured audio and report overrun. |

Final queue capacities and exact overflow thresholds require profiling and Phase 5 acceptance criteria. This document defines policy classes, not numeric capacities.

### 3.3 DSP-to-UI snapshots

Audio writes fixed-size snapshot buffers containing meters, playheads, record positions, CPU counters and state flags. UI reads snapshots at its own cadence. Snapshot data is observational only; the UI never mutates live DSP objects through a snapshot.

Use a proven triple-buffer or sequence-counter implementation with explicit memory-ordering tests. Avoid unbounded variable-size snapshot payloads.

## 4. Object ownership and lifetime

- **Immutable assets:** decoded sample buffers and analysis products are immutable after publication. Loader owns construction; engine receives a handle through a structural command.
- **Runtime graph:** audio thread owns the active runtime graph during processing. Structural replacement is built off-thread, then swapped at a safe block boundary.
- **Retirement:** the audio thread places retired handles into a bounded retirement queue. A non-RT collector destroys them. The audio thread must never release the final reference.
- **Queue full during retirement:** the audio thread must have a non-blocking fallback that retains the retired pointer in a preallocated bounded retirement slot/list. It must never delete it or block. Capacity and maximum number of simultaneous retirements must be proven.
- **Recording buffers:** audio thread owns write position and capture state; readers receive bounded immutable/atomic views with documented guard distance. File export reads a stable snapshot or a copied region off-thread.
- **Parameter registry:** immutable after initialization; runtime values are atomically published or audio-thread-owned according to the parameter class.
- **UI state:** UI owns presentation state. It sends commands/parameter gestures; it does not hold raw pointers into voice, grain or FX internals.

### 4.1 Rolling-buffer readers and Freeze source retention

A committed-frame boundary establishes which audio is valid; it does **not** guarantee that a rolling ring will keep those frames unchanged while an off-thread reader copies them. A reader must never access a region that the audio writer may concurrently overwrite.

Proposed contract for Freeze, state capture and export from a rolling source:
1. At the capture boundary, the audio thread publishes a bounded descriptor containing source generation, first frame, committed frame count and the requested region. It does not copy audio or wait for a reader.
2. Before an off-thread copy starts, a pre-reserved retention lease must protect the selected pages/region from overwrite for the entire read. The mechanism may use preallocated page pinning, segment rotation or an equivalent bounded design; the exact mechanism remains open.
3. The independently armed recorder continues writing into a separate writable region. The audio thread never writes into leased pages and never waits for lease release.
4. If the lease or required spare capacity cannot be reserved, Freeze preparation fails before the source switch. The previous source and recorder state remain unchanged, and a visible failure is reported. Do not fall back to an unsafe concurrent copy.
5. The non-RT reader validates the source generation and committed range, copies to an immutable destination, then releases the lease. Retired pages are reusable only after release.

A global Freeze request is one bounded transaction, not four independently published layer commands. The non-RT coordinator prepares four per-layer outcomes and publishes one transaction payload. At one audio safe point, all ready layer snapshots switch together; layers whose preparation failed remain on their prior source. Per-layer outcomes and partial global failure are observable. If the transaction itself cannot be published, no layer switches. **Owner-review item:** this proposal interprets DEC-042's visible partial failure as allowing ready layers to switch together while failed layers remain unchanged. DEC-042 does not explicitly resolve whether global Freeze should instead be all-or-nothing; this interpretation remains proposed and must be approved before implementation.

Avoid general-purpose shared_ptr destruction on the audio thread. If reference counting is used, its final-release behavior must be controlled and tested.

### 4.2 Replacement generations and retirement saturation

- Each replaceable asset slot has a monotonically increasing request generation assigned by its non-RT coordinator. The audio thread accepts only the currently authorized generation; a late completion from an older request is rejected and retired by its non-RT owner.
- Publication is an ownership-transfer point: before successful bounded-queue publication, the producer owns the prepared payload; after publication, the command/engine ownership contract owns it. Queue-full or command rejection must not leak or prematurely destroy the payload.
- Active voices/grains/sampler heads must retain a stable asset lifetime token independent of the slot's current handle. Replacing the slot does not invalidate readers already using the previous asset.
- The audio thread must not depend on a reference-count decrement being the final release. Final destruction is explicitly deferred to housekeeping.
- Retirement queue full is not permission to overwrite an older retirement entry. The fallback storage is bounded and preallocated; if all retirement capacity is occupied, admission of further structural replacements must be stopped/rejected before another swap can create an unretirable object. The exact handshake that applies backpressure to non-RT producers, fallback capacity and worst-case proof remain open engineering details.
- Every asset lifecycle transition exposes a non-RT status/generation so UI and state capture can distinguish requested, prepared, pending, active, superseded, rejected and retired. Status reporting must not expose raw runtime pointers.



## 5. Structural update protocol

1. Non-RT producer validates request and builds all required objects.
2. Producer checks that the destination queue/pending slot can accept the request.
3. A command is published with release semantics.
4. Audio consumes the command at a documented safe point and performs only bounded pointer/state swaps and precomputed fades.
5. Old objects are retired to the non-RT collector.
6. Producer receives completion/failure through a bounded status channel or snapshot; it does not wait synchronously.

A new sample must not replace the active sample until the new buffer is ready. A failed load leaves the previous sample active. FX/patch graph changes must have defined tail and crossfade behavior.

## 5.1 State capture and restore transactions

- A state-capture request is coordinated off the audio thread and produces an immutable snapshot descriptor with a monotonically increasing state generation. Each recording/Freeze payload is tied to its source generation and committed-frame range.
- A rolling recording reader must hold a bounded retention lease or read from a pre-reserved immutable staging copy. A committed-frame count alone is not permission to read while the writer may wrap and overwrite the region.
- The audio thread may publish bounded descriptors and advance ownership at a safe point; it never copies large audio payloads, waits for the serializer, performs compression, checksums, file I/O or allocates staging memory.
- The serializer owns immutable snapshot handles until every required embedded chunk has been written and verified or the save transaction has failed. A failed/cancelled save releases leases and staging through the non-RT owner path; it must not alter the active recording.
- Restore preparation is off-thread and owns all decoded candidate assets until a complete validated restore command is successfully published. The audio thread performs only the bounded safe-point graph/state handoff; failed preparation or publication leaves the active runtime unchanged.
- A partially prepared restore must never be exposed as a successful project state. Missing external references may be represented as explicit relink-required placeholders; corrupt required embedded audio must surface recovery status, not silent substitution.
- The exact host callback strategy (prebuilt snapshot versus bounded callback-time capture) is adapter-specific and must obey the host's documented contract. No callback may wait for disk I/O or global audio quiescence.

## 6. Parameter and event ordering

For each render block, define and test a deterministic order:
1. Read host transport/process context and host automation values.
2. Merge timestamped MIDI events and internal sequencer events by sample offset and deterministic tie-break rule.
3. Apply structural commands only at approved block/safe points.
4. Update parameter smoothing and modulation values.
5. Render voices/engines, layer FX, layer outputs and master mix.
6. Publish snapshots and retire objects.

The exact ordering of host automation, MIDI mapping, sequencer overrides and modulation must be specified per parameter class. This proposed order is a starting point; no event may be applied at an imprecise offset if the requirement promises sample-accurate timing.

## 6.1 Event-time execution contract — refinement

The following constraints refine §6 without selecting a product-level precedence policy that remains open:

1. **Block-relative time:** every timestamped event is represented by a validated sample offset in the half-open interval `[0, frameCount)`. Events outside the current block are rejected or retained by the adapter according to the host contract; they are never silently clamped to a different musical time.
2. **Stable merge:** host MIDI, host automation, device MIDI and sequencer events are merged by sample offset. Preserve source-local order for events with the same offset. Cross-source same-offset ordering must use an explicit deterministic tie-break field; the field must not depend on pointer address, thread arrival race or container iteration order.
3. **No invented precedence:** the ordering between simultaneous host automation, MIDI-mapped writes, sequencer parameter locks and UI-originated parameter changes is parameter-class-specific and remains an open design decision. The engine must not let whichever producer happens to enqueue last become an accidental precedence rule.
4. **Segmented render:** when a parameter/event requires sample-accurate application, render the block in bounded segments ending at the next event offset, apply the event at that exact boundary, then continue. If an event class is intentionally block-rate or control-rate, its declared update class must say so and acceptance criteria must not claim sample accuracy.
5. **Base value versus modulation:** base parameter writes and modulation contributions are distinct inputs to parameter evaluation. Apply the approved shared-per-layer macro base and per-voice modulation semantics (DEC-031); do not let a per-voice contribution overwrite the shared base value.
6. **Structural work:** sample/patch/FX graph replacement remains a safe-point structural command, not an ordinary sample-offset parameter event. Its transition must be bounded and its retirement remains non-RT.
7. **Overload:** event capacity and per-block work are finite. Preserve host-provided event order and account for every rejected/dropped event; panic/All Sound Off uses its reserved bounded path. Numeric capacity and exact recovery remain Phase 5 measurement/acceptance work.

| Event/source | Required time handling | Precedence status |
|---|---|---|
| Host automation | Preserve host-provided sample offset where exposed; otherwise apply at declared block/control boundary | Source-specific interaction with other writers remains open |
| Host MIDI / MPE | Preserve event offset and source-local order; retain note identity/zone context | Same-offset cross-source tie-break must be explicit |
| Internal sequencer | Generate deterministic offsets from shared transport and pattern timing; preserve step/event ordering | Parameter-lock interaction with host automation/MIDI remains open |
| Device MIDI | Timestamp/copy into bounded owned records; do not use thread arrival order as musical order | Clock timestamp quality and same-offset merge policy require host/device validation |
| Modulation | Evaluate from current base values and source state at the declared update class; per-voice sources remain voice-scoped under DEC-031 | Modulation sum/order and smoothing are parameter-class rules, not event queue arrival order |
| UI gestures | Publish bounded parameter updates; continuous values may coalesce only under the documented parameter policy | Never silently outrank sample-accurate host/sequencer events |

**Implementation gate:** before implementation, define the deterministic same-offset tie-break and per-parameter writer precedence matrix, then test block-boundary offsets (0 and final valid sample), simultaneous note-on/note-off, automation plus MIDI, parameter-lock plus automation, and variable block sizes. Do not infer these policies from the illustrative processing sequence in §6.

## 7. Reset and lifecycle behavior

The architecture must define bounded behavior for:
- prepare/release and repeated prepare calls;
- sample-rate and maximum-block-size changes;
- variable block sizes and zero-length callbacks where permitted;
- transport stop/start and host seek;
- plugin bypass and offline render;
- device loss/reconnect in Standalone;
- state restore while audio is stopped or active;
- shutdown with pending commands, recordings or retired objects.

No destructor that may run on an RT thread may perform blocking cleanup. Shutdown may use a non-RT quiescence protocol once the host has stopped processing.

## 8. Real-time verification

Required instrumentation/tests:
- allocation and deallocation detection on RT thread;
- lock acquisition and blocking-call detection where practical;
- queue overflow/underflow counters;
- maximum command count and callback execution time;
- retirement queue saturation tests;
- rolling-ring wrap while a Freeze/state snapshot lease is held, including lease exhaustion and safe rejection;
- global Freeze transaction publication under queue saturation and mixed per-layer preparation outcomes;
- randomized producer stress with deterministic audio output;
- ThreadSanitizer for non-RT concurrency tests where compatible;
- RealtimeSanitizer evaluation (Q-T09), without treating tool absence as permission to omit RT tests;
- 60-minute stress render and long-duration recording soak in Phase 7.

## 9. Open decisions and implementation gates

- Final queue capacities and maximum per-block work: Phase 5, after workload definition and profiling.
- Optional RT worker model: defer activation until R1 profiling (Q-T08).
- Exact lock-free primitive implementations and memory orders: engineering design review before implementation.
- Audio callback event capacity and overflow recovery: architecture + Phase 5 test threshold.
- Whether the host can call a given lifecycle method on a non-message thread: verify per SDK/format.

**Status:** This proposal establishes the ownership and bounded-communication contract needed before implementation. It does not set measured resource values or authorize code.

## 10. AU/VST3 callback execution and reconfiguration constraints

This section refines the generic thread model using official format documentation. It does not choose a framework or authorize host-shell code.

### 10.1 AU execution

- The host caches the AU render block before realtime rendering. Treat the resulting render invocation as the RT boundary; no adapter operation inside it may allocate, block, log or perform I/O.
- The host sets maximumFramesToRender before render-resource allocation and cannot change it while resources remain allocated. Preallocate for that maximum. A changed maximum or bus format requires a controlled deallocation/reconfiguration/allocation lifecycle; do not resize callback storage in internalRenderBlock.
- AU scheduleParameterBlock may be called from any thread, including an RT render thread. The wrapper must preserve the event timing while placing the normalized event into bounded storage. Do not connect this multi-context entry point directly to an SPSC queue unless a single-producer guarantee is established by the adapter.
- A failed AU input pull invalidates the current input data. Publish an explicit unavailable-input state for that call; never attempt to read stale or null sample memory.
- Host transport and MPE capabilities are queried through AU APIs but may not be available or valid in every host. Context absence is data, not a reason to invent a clock or MIDI capability.

### 10.2 VST3 execution

- VST3 process() and setProcessing() may execute on the audio thread. Both paths must be bounded and allocation-free.
- setupProcessing() establishes sample rate and maximum block size while the processor is inactive. Steinberg documents that these values cannot change while the component is active. Reconfiguration must stop processing, deactivate, prepare/setup the new configuration, reactivate and restart using generation checks.
- A VST3 process call may be issued with no audio buffers to flush parameters (numInputs = 0, numOutputs = 0). The adapter must classify this separately from an audio block, apply the permitted parameter/event updates, and avoid touching absent bus arrays or advancing audio/transport by an invented frame count.
- The host may omit trailing inactive buses from the per-call arrays. Use numInputs/numOutputs before indexing. For a bus entry that is present, honor its negotiated channel count; inactive channel sample pointers may be null. Do not equate declared-bus count, active-bus count and current array length.
- VST3 process context is optional. Request required fields through IProcessContextRequirements when using the applicable SDK; validate the returned context flags for each call.
- In offline VST3 operation, state callbacks may occur on the same thread as processing. Immutable snapshot/restore preparation remains required; do not assume state callbacks always arrive on a UI thread or that offline mode makes mutable shared-state access safe.

### 10.3 Cross-format concurrency consequences

- A format callback may have a different producer/thread topology than the generic UI/loader queues. Document every adapter ingress separately before selecting SPSC/MPSC/mailbox primitives.
- The normalized process context must include an explicit process kind and per-call bus-entry validity, not just frame count and nullable pointers.
- Sample-rate, maximum-block-size, bus-arrangement and render-capacity changes are lifecycle transactions. Prepare new resources off the render thread, advance the lifecycle generation, and publish only after the old configuration is no longer active under the host contract.
- State callbacks may be concurrent with realtime rendering in one format/context and sequential on the processing thread in another. Use immutable generation-consistent snapshots and host-specific callback rules; never solve this by blocking the render thread.

These constraints are API-derived. Exact host behavior, callback sequences and live-input/output exposure still require the authorized host matrix; no host prototype or benchmark was run.

### 10.4 Render-capacity generation and scratch ownership

Each prepared render configuration has one immutable capacity descriptor for the lifetime of that generation:

- `maxFramesPerBlock` (`B`);
- negotiated input bus/channel arrangement;
- negotiated output bus/channel arrangement;
- sample rate and any sample/work format that changes storage shape;
- monotonically increasing lifecycle generation.

AU derives `B` from host-set `maximumFramesToRender` before render-resource allocation. VST3 derives `B` from `ProcessSetup::maxSamplesPerBlock` while inactive. Standalone derives it from the configured device callback maximum. For an actual audio render of `N` frames, validate `N <= B` before touching scratch. No callback may resize, allocate, free or shrink a workspace.

A callback that exceeds `B` is a bounded adapter/configuration failure. It must not index beyond the prepared capacity or attempt recovery by allocating. The exact AU/VST3/device error mapping is adapter-specific and remains a validation item. A VST3 parameter-flush call with no audio buses is not an audio render: it may apply the permitted bounded parameter/event work, but it does not consume audio scratch or advance audio/transport.

**Ownership split:**
- Host input/output buffers are borrowed only for the current callback and are never retained by the engine.
- Engine-owned scratch and per-layer working planes are allocated/reserved off-thread before the generation becomes active.
- The implementation's resource ledger counts only actual owned/staged copies, not every borrowed bus view.
- A read-only physical input bus may back multiple logical per-layer source views. Such fan-out does not create four copies unless an explicit staging/conversion operation materializes them.
- Optional output buses may be absent/inactive. Their absence does not authorize allocation of replacement buffers during render; the engine follows the prepared internal routing and primary-master rules.

For float32 scratch, a conditional full-copy scenario of five stereo output buses is `40 × B` bytes; one additional stereo input staging copy is `8 × B`. These are planning equations, not required allocation topology. Use checked wide byte arithmetic and verify every allocation fits `size_t` before reservation/allocation. Float32 sample width and a 32-bit per-block frame-count type do not imply a 32-bit process. Q-T03 remains open; the current macOS architecture investigation is x86_64/arm64, with no 32-bit macOS target approved or implied.

Reconfiguration is a lifecycle transaction:
1. The adapter stops/deactivates or quiesces the old configuration according to the host/device API.
2. A non-RT owner constructs and admits the complete next generation.
3. A failed preparation leaves the prior generation intact where host lifecycle permits; otherwise the component remains in a defined inactive/error state.
4. The new generation is published only when its complete resources and routing map are ready.
5. Old scratch/graph resources are retired only after the host can no longer invoke a callback referencing them; destruction occurs off RT.

A lower actual frame count does not shrink capacity during render. A changed maximum, sample rate or bus arrangement is not a per-block event; it requires the documented host/device lifecycle. See `PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md` for byte formulas and planned boundary/reconfiguration tests.



## 11. Cross-format event, automation and transport ingress

The adapter boundary must account for the different producer and callback shapes of AU and VST3. The normalized contract is detailed in `PHASE3_CROSS_FORMAT_EVENT_TRANSPORT_PARITY_AUDIT.md`.

### 11.1 Borrowed host data

- AU render-event lists and VST3 `IEventList`, `IParameterChanges`, parameter queues and `ProcessContext` are callback-scoped host data. Read and translate them during the permitted callback; do not retain their pointers, interface objects or references after return.
- Copy only bounded values required by the shared core into preallocated storage. Preserve source identity, source-local order, sample timing, bus/channel identity and note identity where present.
- VST3 event lists and parameter queues are distinct input collections. Merge them with sequencer/device events by sample time without assuming that collection iteration order defines cross-source precedence.

### 11.2 Producer topology

- AU `scheduleParameterBlock` is documented as callable from any thread, including a realtime render thread. It is not inherently a single-producer ingress. An SPSC queue is valid only if the adapter proves a single producer; otherwise use a bounded design with a documented producer topology.
- Device MIDI is timestamped/copied into bounded owned records. Thread arrival order is never musical order.
- Host callback data already delivered for the current block should be consumed directly through the bounded adapter path rather than copied through an unnecessary asynchronous queue.

### 11.3 Event batches and automation curves

- Discrete events use validated block-relative sample offsets. Preserve order within one source.
- Different sources at the same sample offset are grouped into a timestamp batch. Do not use producer arrival, pointer identity or container order as a tie-break.
- AU parameter ramps and VST3 per-parameter automation queues are curve inputs, not ordinary step events. Preserve ramp duration, linear segments, implicit prior values and the VST3 previous-block anchor at relative position -1.
- The -1 automation anchor is signed continuity metadata, not a current-block event and not a queue index. It must not enter the unsigned discrete-event offset path.
- Same-parameter collisions across host automation, MIDI mapping, sequencer locks and UI remain blocked by AR-01. No winner is selected here.

### 11.4 Transport snapshot and process kind

- Cache AU host musical-context and transport-state callbacks before rendering. Their success and field availability are independent; the render path must not invent missing values.
- For VST3, request required ProcessContext fields through `IProcessContextRequirements` during setup and validate each per-call state flag. A null context or unset field flag is unavailable data.
- Publish one immutable field-valid transport snapshot per audio block. All four layers and the central sequencer consume that snapshot.
- A VST3 parameter-flush call with no audio buses is a control-only ingress. It must not touch render scratch, advance sequencer/transport time or increment rendered-frame counters.
- Cycle/loop discontinuity within a block is host-dependent. Do not assume all hosts split at the loop boundary; report insufficient intra-block timing unless host evidence proves otherwise.

No new queue capacity, event-drop policy, transport fallback policy, note-retrigger policy or cross-source precedence is approved by this section.


## 11. MPE note-identity state and bounded lifecycle

The MPE identity audit adds stateful note routing to the existing bounded event model.

### 11.1 Ownership

- The render domain owns the active SourceNoteIdentity table, AU member-channel groups, VST3 channelized-MPE member-channel groups, VST3 Note Expression host-note lookup and layer-local NoteInstanceId associations. The channelized and noteId paths remain distinct ingress identities.
- Host adapters copy callback-borrowed event fields into bounded records. They do not retain AU/VST3 event pointers or mutate the render-owned note map from an arbitrary host callback thread.
- AU MIDI scheduling may enter from multiple producer contexts. Its ingress must follow the documented producer topology; identity assignment occurs only when the ordered event is consumed by the render domain.
- UI/controller metadata for VST3 Note Expression types is queried outside render. The render path consumes only pre-resolved type IDs and normalized values.

### 11.2 Bounded operations

- Note lookup, source-to-layer fan-out, member-channel expression broadcast and identity retirement must have bounded worst-case work.
- Active-note lists, identity slots and channel-group membership storage are preallocated. No dynamic map insertion, heap allocation, blocking lock, logging or final destruction is allowed in render.
- Handles include a generation so a stale note/expression event cannot resolve to a newly reused identity-table slot.
- Channel-lease generations prevent a released AU voice from following expression on a later reuse of the same member channel.
- Capacity exhaustion is counted. It must not silently downgrade per-note expression to channel-wide pitch bend. Numeric capacity and exact overload recovery remain Phase 5 engineering gates.

### 11.3 Lifecycle ordering

- Note On allocates a source identity before layer fan-out; each receiving layer allocates its own NoteInstanceId.
- Per-note expression resolves the source identity and updates all associated live layer voices exactly once.
- VST3 Note Off moves the identity to released state but does not retire it while a release voice can still receive Note Expression.
- AU Note Off ends the member-channel association for that source note under the candidate protocol model. Snapshot the last expression onto its release voice before a channel can be reused; same-pitch matching and sustain interaction remain open under AR-03.
- Voice completion retires the layer-local NoteInstanceId. The source identity is retired only when no format-specific expression target remains.
- Panic and All Sound Off clear active mappings using bounded operations; they never wait for a worker.

See PHASE3_MPE_NOTE_IDENTITY_AND_LIFECYCLE_AUDIT.md. This section defines ownership and RT constraints, not numeric capacities or a final Note Off matching policy.

## 12. MPE fixture execution and observation boundaries

The detailed scenario catalogue is PHASE3_MPE_EVENT_NORMALIZATION_AND_LIFECYCLE_FIXTURES.md. Its execution model preserves the ownership rules in §§10–11:

- Host callbacks copy only bounded event fields. AU/VST3 event pointers, SDK interfaces and callback-owned lists are never retained.
- The render domain solely owns active SourceNoteIdentity records, channelized-MPE groups, VST3 Note Expression lookup and layer-local NoteInstanceId associations.
- AU scheduling and other multi-producer ingress publish copied records through the documented bounded topology; they do not mutate render-owned identity maps.
- Test observers consume immutable snapshots at explicit block boundaries, not live mutable tables.
- Identity lookup, generation validation, channel-group broadcast, DEC-034 fan-out and retirement use preallocated bounded storage. No allocation, deallocation, lock wait, logging or final destruction is allowed on render.
- Saturation fixtures verify counted exhaustion without identity alias or silent expression-scope changes. Exact capacity and recovery remain Phase 5/AR-02 gates.
- Panic and All Sound Off cleanup is bounded and does not wait for a worker. All Notes Off remains a distinct lifecycle operation.
- Offline shared-core fixtures do not certify host delivery. Host-specific raw event traces require a separately authorized prototype.

No queue capacity, identity-table size, overload priority, lock-free algorithm or host callback behavior is frozen here.

## 13. Reset, restore and generation ownership — 2026-11-28

- RuntimeStateEpoch changes only when a complete persistent state restore commits.
- IngressNamespaceGeneration changes for one AU cable, VST3 event bus, Standalone MIDI input or negotiated MPE zone.
- RenderConfigurationGeneration changes with sample rate, maximum block, bus arrangement or render workspace.

Do not collapse these into one counter. Stale records are rejected before identity lookup.

Host callbacks/non-RT coordinators publish bounded ResetRequest/RestoreTransaction records; they never clear render-owned maps directly. Render owns active identity tables, ingress leases and voice associations. CC 121 changes controller state only; CC 123 requests release and may be held by sustain; CC 120 is scoped sound termination; Panic is a separate reserved recovery path. Equivalent resets may be coalesced only when scope and semantics match.

Parse, validate, migrate, prepare assets/recordings and reserve memory off RT. Candidate state is immutable before publication; publish one transaction at a safe point. Failed restore leaves prior runtime/identity state authoritative. After commit, old identity handles cannot bind into the new epoch. VST3 controller synchronization follows processor-first component-state order.

VST3 setProcessing(false) may run on UI or processing thread and must remain light, lock-free and allocation-free. AU reset caller-thread behavior remains host validation. Retire old graph/assets off RT only after all readers/voices/grains release them.

Restore admission counts old graph + candidate graph + embedded recording/Freeze chunks + staging + pending retirement. Generation wrap must never alias a live handle; quiesce/reinitialize off RT or reject admission. See PHASE3_MPE_RESET_STATE_RESTORE_AND_HOST_LIFECYCLE_AUDIT.md. AR-06 remains open.


## 14. State container stream and migration ownership — 2026-11-28

- The state-capture coordinator owns the immutable, generation-consistent snapshot and its recording/Freeze retention leases. The serializer owns those immutable handles until all required chunks are emitted and verified or the save fails.
- The audio/render domain publishes only bounded capture descriptors and performs the documented safe-point handoff. It never serializes JSON, computes whole-payload digests, compresses audio, reads/writes host streams, performs file I/O or allocates state staging.
- Manifest construction, schema migration, checksum calculation, compression/decompression, chunk staging and restore preparation are non-render operations. They still require bounded memory admission and cancellation/failure ownership.
- Host state callbacks must be treated according to the actual host contract, not assumed to be a generic background thread. VST3 documents state interfaces as UI-thread operations, but offline-host behavior must not be assumed to be equivalent to a live UI callback; existing offline-processing caveats remain in force. If a host invokes state handling on a processing thread, no unbounded capture, compression, large allocation or audio-thread acknowledgement wait is permitted.
- The host adapter writes/reads the canonical container through the host-owned transport. For VST3 IBStream, every transfer uses a bounded int32-sized request and checks the reported byte count; short transfer, error and zero progress terminate the operation safely. For AU, the adapter validates the actual dictionary/Data materialization behavior and does not assume streaming or an unlimited payload.
- A host-owned stream is not a file transaction owned by the plug-in. The serializer cannot promise rename, fsync, rollback or preservation of the previous DAW project. The completion marker makes a partial container detectable; host-level durability is separately evidenced per host.
- The standalone file owner may use a temporary-file/replace transaction, but only on the non-RT file worker. The previous valid file remains authoritative until final replacement succeeds.
- Schema migration is deterministic and off-render. It operates on a validated metadata representation, preserves the original source state, and prepares a complete candidate before publication. A migration failure never mutates the active runtime.
- Restore publication remains one bounded safe-point transaction. The old runtime is retained until the render domain has relinquished it; retirement and final destruction remain non-RT. AR-06 still governs the audible handling of old voices and is not resolved by this section.

No transfer-slice size, callback deadline, queue capacity, maximum state size, codec, checksum algorithm or host threading guarantee is frozen here. See PHASE3_STATE_CONTAINER_AND_MIGRATION_AUDIT.md.

## 15. State I/O scheduling and capture watermark — 2026-11-28

- The recorder publishes a bounded commit record for each completed block/page, including source generation and committed-frame watermark. A capture descriptor identifies only fully committed ranges; it never reads an in-flight writer region.
- The capture coordinator owns generation validation and retention-lease acquisition. The serializer owns immutable handles until all required reads/transfers finish or fail.
- A host callback never waits for the render thread, disk I/O, global quiescence or an unbounded worker acknowledgement. Prefer a prebuilt immutable snapshot; a callback-time capture is allowed only where the actual host contract permits bounded non-render work.
- A prepared descriptor must match the latest committed-watermark publication observed at the capture boundary. A stale descriptor must not be returned as a successful DEC-025 document save.
- Required recording and Freeze chunks are acquired as one document-state transaction. Partial lease acquisition rolls back off-render; no incomplete snapshot is published.
- The recorder continues into separately reserved writable capacity while pages are leased. It never waits for lease release and never overwrites a leased page. If retention plus writable headroom cannot be guaranteed, capture fails without stopping or mutating the recording.
- Save jobs, capture descriptors and in-flight leases are bounded. Queue/admission saturation returns a bounded failure; it does not create an unbounded backlog.
- Cancellation does not release a lease until the serializer/copy reader confirms quiescence. Final release and destruction remain non-render.
- Save, Freeze, sample replacement and restore share one physical-allocation/retention ledger. Concurrent jobs are admitted against their aggregate peak, not independent feature budgets.

See `PHASE3_STATE_IO_SCHEDULING_AND_PEAK_MEMORY_ADMISSION_AUDIT.md` for the transaction state machine, host-specific scheduling, accounting model and Phase 5 fixtures. No queue capacity, lock-free primitive, callback deadline or numeric memory limit is selected here.


## 16. State callback concurrency and coherent capture publication — 2026-11-28

The [Cross-Host State Callback & Snapshot Freshness Evidence Matrix](PHASE3_CROSS_HOST_STATE_CALLBACK_AND_SNAPSHOT_FRESHNESS_EVIDENCE_MATRIX.md) refines the state-callback ownership boundary.

- VST3 component `getState/setState` are UI-thread operations in the normal realtime threading model and are permitted while the component is Processing. Do not infer that `process` has stopped; it may continue concurrently.
- VST3 offline get/set-state may run on the process thread according to supplemental Steinberg developer guidance. Do not synchronously wait for a worker or perform an unbounded capture on that path.
- AU state-property callback thread and render overlap are not specified by the reviewed public API. Treat them as unknown until host traces exist.
- A state callback reads one immutable capture descriptor. It never walks a mutable recording ring, reads an in-flight writer page, clears render-owned state, or acquires a blocking lock.
- The descriptor's capture boundary is a coherent vector containing RuntimeStateEpoch, a publication sequence, and the source generation/committed-frame watermark for every required recording and Freeze payload. Do not assemble it from unsynchronized independent source reads.
- If the prepared descriptor does not match the capture-boundary token, it is stale. Do not return it as successful DEC-025 state. A bounded refresh is allowed only if the callback context permits it and required leases are already obtainable without render acknowledgement.
- AU's optional dictionary property has no documented explicit getter error result. Returning nil is not assumed to abort a host save; this remains a compatibility test. VST3 `tresult` is checked, but host rollback behavior remains unverified.
- Restore parsing and candidate preparation remain separate from render-owned objects. Publish one complete candidate at the existing safe point; retire old state off-render after all readers release it.

No queue capacity, lock-free primitive, capture-token ABI, retry count, callback deadline or host thread guarantee is selected. AR-06 and Q-T15 remain open.


## 17. AU state getter result and CF-object ownership — 2026-11-28

- AUv3 **fullState** / **fullStateForDocument** expose nullable dictionary properties with no separate getter error result in their signatures. Their AUv2 bridge mapping for nil is not documented; do not assume nil aborts a host save.
- AUv2 **AudioUnitGetProperty** returns OSStatus. A successful Core Foundation object result is an owned reference that the caller must release. Ownership of that reference is distinct from ownership or copying of nested immutable Data backing.
- The non-render state adapter owns the returned state object through serialization/transfer and releases it on a non-render owner. Do not move potentially non-bounded CF object destruction onto the render callback.
- Apple's ClassInfoFromDocument fallback to ClassInfo is a restore rule. It does not authorize a save worker to substitute a preset snapshot for required DEC-025 document payloads.
- Any dictionary/Data materialization and actual copies are charged to aggregate state-I/O memory admission only when they are plug-in-owned/observed. Unknown host-internal copies remain an evidence gap.
- The getter callback context remains host-specific and unverified. No lock, callback deadline, OSStatus constant or copy factor is selected.

See **PHASE3_AU_STATE_FAILURE_SIGNALLING_AND_DEC025_COMPATIBILITY_AUDIT.md**.

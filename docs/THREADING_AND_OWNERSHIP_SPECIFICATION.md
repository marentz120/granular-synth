# Threading and Ownership Specification

**Version:** 0.2 — Freeze source-retention and transaction contract added  
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

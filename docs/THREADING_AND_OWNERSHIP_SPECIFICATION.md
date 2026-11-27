# Threading and Ownership Specification

**Version:** 0.1 — Phase 3 proposal  
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

Avoid general-purpose shared_ptr destruction on the audio thread. If reference counting is used, its final-release behavior must be controlled and tested.

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

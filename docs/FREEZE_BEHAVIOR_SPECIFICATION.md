# Freeze Behavior Specification

**Version:** 1.0 — Approved R1 product behavior  
**Status:** **Approved by owner on 2026-11-27 (DEC-042).** Product semantics are approved; detailed DSP implementation and resource validation remain Phase 3/5 work.  
**Authority:** DEC-020, GS-SMG-021, GS-MID-008, DEC-016/021, Audio/I/O Specification §8.

## 1. What is already approved

- Each of the four layers has an independent Freeze control.
- One global Freeze control acts across all four layers.
- Freeze is a required R1 live-performance operation and must be controller reachable.
- Freeze semantics were approved by the owner as Option A on 2026-11-27 (DEC-042); this document defines GRANULAR SYNTH behavior and does not claim benchmark parity.

## 2. Design problem

“Freeze” could mean materially different things:
- latch/capture the current live input or rolling recording buffer;
- stop recording-buffer writes while existing grains continue;
- hold existing grain/voice state and stop transport/modulation;
- freeze the post-FX audio output;
- capture a new immutable sample snapshot and continue normal note/sequence playback from it.

These behaviors are not interchangeable. The product must choose one semantic contract and specify its interactions with recording, transport, MIDI, modulation, voices and effects.

## 3. Approved R1 behavior — source snapshot Freeze

**Approved behavior (Option A — Source Snapshot Freeze):** Freeze latches the current source audio for the selected layer; it does not freeze the whole instrument's transport or DSP clock.

When a layer Freeze is engaged:
1. Capture a stable snapshot of the layer's current active source region/buffer at a defined capture boundary.
2. Publish the snapshot as an immutable source for that layer.
3. Stop writes/rolling updates to the frozen source snapshot; continue the underlying external input path only if the product requires monitoring, but do not alter the frozen material.
4. Existing grains/voices continue rendering normally.
5. New MIDI notes and sequencer events continue to trigger against the frozen snapshot.
6. Modulation, envelopes, LFOs, sequencer, MIDI and shared transport continue normally.
7. Layer FX and effect tails continue processing.
8. Thaw returns the layer to its previous live/rolling source and uses a short click-safe crossfade.
9. Freeze does not implicitly start or stop the independent recording operation. Recording destination and source taps remain governed by the recording model.

Global Freeze is the atomic equivalent of engaging Freeze on all four layers at the same logical boundary. If one layer cannot capture due to unavailable source or memory exhaustion, the UI/controller status must indicate partial failure; the system must not claim all four froze successfully.

**Why this is recommended:** it is musically useful for live granular performance, keeps the sequencer and controller workflow alive, and avoids making Freeze an undocumented transport-stop or master-output latch. It also gives the engine a clear immutable-buffer boundary.

## 4. Alternative semantics considered

| Option | Behavior | Advantages | Risks |
|---|---|---|---|
| A — Source snapshot (recommended) | Capture current source audio; continue grains, notes, modulation, transport and FX | Musical performance remains active; clear ownership boundary | Requires bounded snapshot memory and precise capture region |
| B — Grain/voice hold | Stop or hold current grain/voice state | Immediate “freeze the sound” effect | Ambiguous note-on/retrigger, CPU use and thaw behavior; does not clearly serve live-source capture |
| C — Post-FX output latch | Capture and loop/hold final layer output | Predictable output texture | Adds audio capture/looping stage, latency/phase concerns and bypasses source-level editing |
| D — Full engine pause | Stop layer DSP/transport/modulation while frozen | Simplest conceptual “pause” | Breaks live performance, sequencer and controller expectations; not recommended |

## 5. Approved contract details

### 5.1 Capture region
Approved rule (DEC-042): capture the layer's current valid source region, respecting active crop/loop bounds and excluding uncommitted frames. For rolling buffers, capture only committed frames. Numeric duration and minimum-resource requirements remain engineering limits; never read out-of-bounds frames.

### 5.2 Source types
The product must define behavior for:
- live external input;
- active recording buffer;
- committed recorded sample;
- static library sample;
- missing/disconnected input;
- mono versus stereo source.

Approved rule (DEC-042): for an already immutable library sample, report “already static” or disable redundant capture with a clear state; do not allocate a duplicate full copy.

### 5.3 Recording
Approved rule (DEC-042): Freeze does not stop an independently armed recorder. Recording continues at its defined tap; Freeze changes only the source consumed by the layer engine. The frozen snapshot is immutable and must not be modified by ongoing recording writes.

### 5.4 Memory and failure
- Snapshot storage must be allocated off the audio thread.
- Freeze engagement is a bounded command; the audio thread only switches to a ready immutable snapshot.
- If no memory is available, Freeze fails safely and preserves the previous source.
- Thaw uses the same safe publication/crossfade protocol.
- Snapshot lifetime is tracked and retired off the audio thread.

### 5.5 Controller and state
- Layer Freeze and global Freeze are mappable performance controls.
- Global Freeze status must reflect all four layer states, including partial failure.
- Approved rule (DEC-042): Freeze state and snapshot audio persist in plugin state; the frozen snapshot is embedded as an unsaved recording buffer under DEC-025, subject to the same unresolved state-size engineering policy.
- Panic/All Sound Off must silence voices/effect tails as already required; it does not silently thaw a frozen source.

## 6. State machine contract

`Live/Static Source → Preparing Snapshot → Frozen → Preparing Thaw → Live/Static Source`

- `Preparing Snapshot`: non-RT copy/build; source remains active until snapshot is ready.
- `Frozen`: immutable snapshot is the engine source; live recorder may continue independently.
- `Preparing Thaw`: original source is made ready and crossfaded back.
- A second Freeze/Thaw command while a transition is pending is coalesced to the latest requested target state.
- Device loss or invalid source does not invalidate the frozen snapshot; the layer continues from the last valid frozen data until thaw is requested.

## 7. Approved acceptance criteria

- Per-layer Freeze captures a stable source snapshot with no audio callback allocation or blocking.
- Global Freeze transitions all four layers at the same defined logical boundary.
- Existing grains/voices continue without unintended discontinuity.
- New notes and sequencer events behave according to the approved source-snapshot semantics.
- Transport, modulation and FX behavior match the approved contract.
- Recording continues/stops exactly as specified, with no source/destination race.
- Thaw is click-safe and restores the expected live/rolling source.
- Freeze failure due to unavailable source or memory exhaustion is safe, visible and recoverable.
- State save/restore preserves the approved Freeze state and snapshot policy.
- Controller-only Freeze/Thaw operation works in Standalone.

## 8. Approval and implementation boundary

The owner approved Option A as written on 2026-11-27. The approval includes the source-region rule, static-sample behavior, independent recording behavior, memory-failure handling, controller mapping, state persistence and panic behavior described above. Recorded in **DEC-042**.

This approval settles product behavior only. It does not approve a numeric snapshot-size limit, memory budget, buffer duration, copy algorithm, crossfade duration or implementation. Those are engineering details requiring Phase 3/5 specification and validation. Production Freeze code remains gated by architecture approval.

# Phase 3 R1 Cross-Module Interface Contracts

**Version:** 0.2 — Freeze retention and global transaction contracts added  
**Status:** Proposed contracts for owner review. No public API, framework choice, numeric capacity, or production implementation is approved by this document.  
**Baseline:** `GRANULAR_SYNTH_REQUIREMENTS.md` v1.0; DEC-017…DEC-042; companion Phase 3 specifications.

## 1. Purpose and contract rules

This document takes the domain-level 124/124 R1 mapping in `PHASE3_ARCHITECTURE_TRACEABILITY.md` one level deeper: it defines the information, ownership, timing, failure and validation obligations at the boundaries between architecture domains. It is not a class diagram and does not prescribe final C++ signatures.

Normative terms:
- **Must**: required to preserve an approved product requirement or real-time safety invariant.
- **Proposed**: recommended engineering contract requiring review and validation.
- **Open**: cannot be fixed without host evidence, measurement, or an owner decision.

Every interface must define its producer, consumer, data ownership, execution context, boundedness, ordering, failure behavior and observable validation. A pointer crossing a thread boundary is not an ownership contract by itself.

## 2. Canonical interface inventory

| Interface ID | Producer → consumer | Data contract | Ownership / lifetime | Timing and failure contract | Requirement coverage |
|---|---|---|---|---|---|
| IF-01 Host process → Engine render | AU/VST3/Standalone adapter → shared engine | Audio input/output channel views, sample rate, frame count, transport context, timestamped MIDI/parameter events, active-bus map | Host owns callback buffers; engine borrows them only for the current call | No retention after callback; support negotiated variable block sizes; inactive/unconnected buses handled explicitly; invalid context produces safe silence/status rather than out-of-bounds access | GS-PLT-001…004, GS-ARC-006, GS-MID-001…008, GS-SMG-020 |
| IF-02 Parameter registry → runtime/host/UI/state | Immutable registry → all domains | Canonical string ID, host ID if exposed, type, units, plain/normalized mapping, default, curve, smoothing class, scope and capability flags | Registry immutable after initialization; runtime values have one declared writer or atomic publication rule | Unknown IDs ignored with compatibility status; duplicate IDs or invalid metadata fail registry validation; host IDs never renumber or recycle | GS-PLT-004, GS-MOD-003…009, GS-MID-003, GS-PRS-001…009 |
| IF-03 UI/loader/state → audio structural command | Non-RT producer → audio consumer | Bounded command kind + fully prepared immutable payload + generation/version + completion token | Producer owns until successful publication; audio owns active payload after safe-point swap; replaced payload retired off RT | Fixed queue capacity; no allocation/build/destruction in callback; overflow rejects/coalesces by command class with visible status; commands applied in deterministic order at defined safe point | GS-SMG-003/004/020/021, GS-FX-001/002/004/006, GS-PRS-001…009 |
| IF-04 Audio → UI observation | Audio runtime → UI | Fixed-size versioned snapshot: meters, playheads, record positions, freeze/record state and counters | Audio publishes; UI reads a copy/snapshot; UI never receives mutable runtime pointers | Latest-value semantics; UI may skip stale frames; no wait or lock in audio callback | GS-UI-001…007, GS-SMG-005/020/021, GS-NFR-001/005 |
| IF-05 Loader → Sample Store → engine | Loader/decoder → immutable sample asset → layer/voice/grain | Channel layout, sample rate, frame count, decoded audio, crop/loop metadata, content hash, analysis references | Loader constructs; immutable after publish; engine holds explicit handle; final release/destruction off RT | Previous valid sample remains active until replacement is ready; malformed/missing asset rejected; no partial publication; memory admission checked before activation | GS-SMG-001…006, GS-SMP-001…004, GS-GRN-001…020 |
| IF-06 Input/engine → recorder → state/export | Host/device input or defined engine tap → recorder → persistence/export | Preallocated capture buffer/ring, committed-frame boundary, channel/rate metadata, overflow and tap metadata | One writer per recording buffer; readers receive a stable committed range protected for the full read by an immutable snapshot or bounded retention lease; export/state copy off-thread | No RT allocation, disk I/O or wait; committed frames are not assumed immutable if a ring can wrap; leased pages cannot be overwritten; if retention cannot be guaranteed, reject the snapshot safely and preserve valid capture | GS-SMG-007…021, GS-ARC-007, GS-NFR-001…003 |
| IF-07 Source/recorder → Freeze snapshot → layer engine | Source owner → snapshot preparation → audio safe-point switch | Immutable valid source region, crop/loop bounds, committed-frame count, source generation, retention-lease token and per-layer preparation status | Snapshot prepared off RT under a bounded source-retention lease; one global transaction payload carries four layer outcomes; snapshots are immutable and retired off RT | Rolling pages cannot be overwritten during copy; recorder continues in a separate writable region; no lease means safe pre-switch failure; all ready layers switch at one boundary, failed layers stay on prior source, transaction-publication failure switches none; thaw crossfades | GS-SMG-021, GS-SMG-003/019/020; DEC-042 |
| IF-08 Host/device/MIDI/Link → shared transport | Host adapter, MIDI clock adapter or Standalone clock → transport authority → sequencer | Tempo, PPQ/beat position, play/stop/continue/seek state, clock source and timestamped pulses | One active authority per runtime; adapters translate but do not independently advance sequencer | Deterministic arbitration; MIDI Clock Start/Continue/Stop semantics per DEC-039; timeout stops clock-dependent progression at current position; SPP ignored in R1 | GS-SEQ-001…012, GS-MID-005…008, DEC-019/039 |
| IF-09 MIDI input → MIDI router → voice/layer/sequencer | Host/device adapter → router | MIDI 1.0/MPE event, channel, note ID/member zone, validated block-relative sample offset, source-local sequence and stable cross-source ordering key | Event copied into bounded event domain; no references to host-owned ephemeral event memory | Offsets are in `[0, frameCount)`; preserve source-local order; cross-source tie-break is deterministic and independent of producer race/pointer identity; sample-accurate classes render at exact offsets; channel-sharing sends to all matching layers; panic has reserved bounded path; overflow counted and recoverable | GS-MID-001…008, GS-VOI-001…010, GS-SEQ-001…012 |
| IF-10 Layer engine → FX chain → layer/master mixer | Voice/engine and live-input wet path → per-layer FX → layer output/master; parallel dry live input → master per GS-ARC-007 | Stereo signal blocks, fixed channel layout, per-layer gain/mute/solo and FX state, plus a parallel dry-input feed | Audio-thread-owned runtime; no dynamic graph mutation during process | R1 FX order and tap positions fixed per signal-flow diagram; tail behavior explicit; four layer outputs and master routing remain deterministic | GS-ARC-001/006/007/008, GS-FX-001/002/004/006 |
| IF-11 Plugin host ↔ state serializer | Host state callback → snapshot serializer → host state blob; inverse on restore | Versioned manifest, patch/layer/sequencer state, embedded unsaved recordings, external asset refs/hashes and migration version | Snapshot assembled off RT; serialized bytes owned by host after callback; restore assets prepared off RT before publish | No disk I/O or blocking on audio thread; corrupt chunk isolated; missing external sample yields relink state; oversized state must fail explicitly or use validated chunking, never silently omit audio | GS-PRS-001…009, GS-SMG-019…021, DEC-025/042 |
| IF-12 DSP/engine → deterministic render/test harness | Headless harness → shared engine → rendered audio + metrics | Complete initial state, input buffers, ordered MIDI/automation/transport events, RNG seed, sample rate and render mode | Harness owns inputs and outputs; engine owns runtime during render | Same declared inputs/state/mode must produce deterministic output; equivalence excludes device I/O and explicitly declared host-specific differences | GS-PLT-004, GS-NFR-001…013, REQ-AUD-004/005, QA-002/005 |

## 3. Cross-interface invariants

### 3.1 Real-time boundary
- Audio callback must not allocate/deallocate, block, acquire contended locks, perform file I/O, log, call UI code or execute unbounded work.
- Every cross-thread queue has one documented producer/consumer topology, fixed capacity, overflow policy and counter. Multiple independent producers must not be routed into an SPSC queue.
- Audio-thread retirement must never perform final destruction. A bounded non-RT reclamation path and saturation fallback are required.
- A committed frame count proves validity, not lifetime: rolling-source readers require a retention lease or equivalent protection against overwrite for the full read.
- Global Freeze is one bounded transaction; four independent layer commands must not be allowed to land on different render boundaries. The current successful-subset behavior for mixed preparation results is proposed, not approved; owner review must resolve it against an all-or-nothing alternative before implementation.
- Host callback buffers and MIDI event memory are borrowed only for the callback unless copied into owned bounded storage.

### 3.2 Ordering and determinism
- Host transport/process context is sampled once per block.
- Host automation and timestamped MIDI events are merged with internal sequencer events using sample offsets and a documented deterministic tie-break.
- Structural changes occur only at declared safe points and cannot invalidate objects still in use.
- Parameter smoothing/modulation order is defined per parameter class; sample-accurate requirements cannot be implemented by block-end-only updates.
- RNG streams are seeded/splittable by stable runtime identity; offline tests provide explicit seeds.

### 3.3 Failure and overload
- Failure to load/prepare a replacement leaves the previous valid runtime active.
- Queue or event overflow is counted; note/event overflow recovery is explicit and panic/All Sound Off cannot be starved by ordinary work.
- Memory admission rejects new work before process instability; it does not evict in-use assets or silently truncate committed recordings.
- Missing external assets do not invalidate unrelated patch state.
- Global Freeze reports per-layer success/failure and never reports success if one layer failed.

## 4. Signal-flow proposal and validation gate

`PHASE3_API_AND_SIGNAL_FLOW_SPECIFICATION.md` §6–8 now defines the recommended R1 engineering defaults, pending owner review and host/UX validation:
1. Live input feeds each layer before its FX chain; mono input is centered across the stereo layer path under DEC-040.
2. A parallel dry live-input path is summed before the master as required by GS-ARC-007; master capture includes it.
3. Approved voice-chain order is engine + sub-oscillator → LPF → HPF; amp-envelope/pan placement remains a validation detail.
4. Per-layer recording and layer output taps: post-FX, pre-layer-fader; stems ignore fader/mute/solo.
5. Master recording/output: final stereo master after layer fader/mute/solo, immediately before host/device output.
6. Freeze snapshots the source region defined by DEC-042; this source tap is distinct from the post-FX recording tap. Recording continues independently.
7. Panic and FX-tail behavior follow DEC-042 and the DSP/acceptance specifications.

These are engineering proposals, not new approved product decisions. The remaining gate is to review and validate the proposed semantics in Standalone and target AU/VST3 hosts; it must not reopen approved DEC-040 or DEC-042 behavior.

## 5. Validation obligations by interface

| Interface | Minimum validation evidence |
|---|---|
| IF-01 | Host callback matrix: variable/zero-length blocks where permitted, sample-rate changes, inactive buses, null buffers, channel layouts, offline process |
| IF-02 | Registry uniqueness, range/curve round-trip, host-ID stability, descriptor migration and unknown-ID handling |
| IF-03 | Queue saturation, producer concurrency, ordering, safe-point swap, no RT allocation/destruction, replacement failure |
| IF-04 | Race/stress test, stale snapshot tolerance, no mutable runtime access from UI |
| IF-05 | Decoder format/bit-depth matrix, malformed files, large sample admission, replacement continuity, hash/relink |
| IF-06 | Recording soak, buffer-full behavior, committed-frame consistency, forced ring wrap during export/state/Freeze reads, retention-lease saturation and safe rejection, tap correctness |
| IF-07 | Per-layer/global Freeze timing, one-boundary transaction with mixed layer outcomes, transaction queue saturation, snapshot immutability, concurrent recording, thaw discontinuity, memory/source failure and state round-trip |
| IF-08 | Host transport, MIDI Clock, Link and internal-clock arbitration; DEC-039 start/continue/stop/timeout |
| IF-09 | MIDI ordering, shared-channel fan-out, MPE isolation, Program Change mapping, panic and bounded overflow |
| IF-10 | Four-layer isolation, FX routing/tails, master/layer output equality and recording tap tests |
| IF-11 | Host save/restore, embedded recordings, external relink/collect, interrupted save, corrupt chunks and oversized state behavior |
| IF-12 | Golden render reproducibility, host/standalone equivalence boundary, seeded RNG and metric capture |

These are validation obligations, not passing test results. Numeric capacities, CPU/RAM ceilings, latency thresholds and supported host versions remain assigned to Phase 3/5/7 evidence gates.

## 6. Requirements traceability refinement

The existing 124-row traceability table remains the authoritative inventory reconciliation. This document refines its interface column as follows:

- Platform and host lifecycle: IF-01, IF-11, IF-12.
- Layer/voice/grain/DSP: IF-01, IF-02, IF-03, IF-09, IF-10, IF-12.
- Sample management, recording and Freeze: IF-05, IF-06, IF-07, IF-11.
- Modulation and parameters: IF-02, IF-03, IF-04, IF-09.
- Sequencer and MIDI: IF-08, IF-09, IF-03.
- Effects and routing: IF-03, IF-10.
- Presets/library/state: IF-02, IF-05, IF-11.
- UI and controller workflows: IF-02, IF-04, IF-09.
- Non-functional requirements: all interfaces, especially IF-01/03/04/06/07/11/12.

**Audit status:** 12 cross-domain interfaces are defined at contract level. Candidate C++ API/class signatures and the proposed signal-flow/tap semantics are documented in `PHASE3_API_AND_SIGNAL_FLOW_SPECIFICATION.md`; they still require owner review and host/UX validation. Queue capacities, state container encoding, final host bus exposure and numeric resource budgets remain open. No implementation-readiness claim is made.

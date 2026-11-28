# Phase 3 R1 Cross-Module Interface Contracts

**Version:** 0.5 — cross-format event and transport parity added  
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
| IF-01 Host process → Engine render | AU/VST3/Standalone adapter → shared engine | Negotiated physical input-bus views, logical per-layer source routes, master/four optional layer output views, sample rate, actual frame count and prepared maximum capacity, transport context, timestamped MIDI/parameter events, active-bus map | Host owns callback buffers; engine borrows them only for the current call | No retention after callback; support negotiated variable block sizes; inactive/unconnected buses handled explicitly; invalid context produces safe silence/status rather than out-of-bounds access | GS-PLT-001…004, GS-ARC-006, GS-MID-001…008, GS-SMG-020 |
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

### 3.4 Host adapter normalization
- The host adapter is the only layer permitted to interpret AU/VST3/Standalone SDK lifecycle, bus and process-context objects. The shared core receives normalized frame count, sample rate, process mode, per-call bus map, transport snapshot and bounded engine-owned events.
- Bus identity and activation are distinct. A declared bus may be inactive or have no valid buffer for a callback; the engine must receive an explicit inactive/unavailable state rather than infer one from pointer presence.
- Primary master output remains active independently of optional layer outputs. An inactive layer output never removes that layer from the internal master mix; an unavailable layer bus is not compensated by summing the layer twice.
- Live-input unavailability is explicit and safe. The engine must not dereference absent input buffers, and the instrument path remains valid when external input is unavailable.
- Host transport is the sole plugin-context authority while valid. Plugin MIDI Clock cannot silently become a concurrent second master. Standalone selects exactly one authority among internal clock, MIDI Clock and Link.
- Lifecycle reconfiguration (bus map, sample rate, device, state restore, suspend/resume) is prepared off-thread and activated by a generation-checked safe-point command. Stale prepared configurations are rejected.
- Shutdown retains ownership of pending commands/recordings and performs final reclamation off the audio thread; no callback blocks waiting for worker completion.



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

### 3.5 AU/VST3 process and bus normalization — format-specific refinement

The generic rules in §3.4 are refined by the official format contracts:

- **AU:** maximumFramesToRender is host-set before render-resource allocation and cannot change while resources are allocated. The normalized context carries both actual frame count and allocated maximum. A failed input-pull result is represented as invalid/unavailable input for that call. AU parameter scheduling may enter from any thread; adapter ingress must have an explicit bounded producer topology.
- **VST3:** ProcessData provides per-call numInputs/numOutputs, numSamples, process mode, parameter changes, events and optional process context. A parameter-flush process call may have no audio buses (numInputs = numOutputs = 0); it is not an audio render. Sample rate and maximum block size are not reconfigured while the component is active.
- **VST3 bus arrays:** the adapter checks current input/output counts before indexing. An inactive bus entry may have null sample pointers; trailing inactive buses may be omitted from the arrays. For every entry that is present, channel count must match the negotiated arrangement. Declared, active, present and buffer-valid are separate facts.
- **VST3 default activation:** a default-active bus flag is a request only. The host may activate fewer buses; the primary master output must remain usable when optional layer stems are not activated.
- **VST3 process context:** request required fields with IProcessContextRequirements where supported, then validate the per-call context flags. Missing tempo/PPQ/transport fields remain unavailable, never synthesized by the adapter.
- **VST3 state callbacks:** offline getState/setState may run on the same thread as process; the state contract must not assume UI-thread-only invocation.

These details refine the normalized bus/process representation and validation obligations; they do not change DEC-020/026 or select Q-P06/Q-P09. See PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md §10 and PHASE3_PLATFORM_AND_PLUGIN_RESEARCH.md §8.

### 3.6 Render-capacity and transient-memory contract — IF-01 / IF-10

- Every active render-resource generation has a prepared maximum frame capacity `B`. AU derives it from `maximumFramesToRender`, VST3 from `ProcessSetup::maxSamplesPerBlock`, and Standalone from the configured device maximum.
- Each actual audio render supplies `N`; validate `N <= B` before any workspace access. No allocation, deallocation, resize or capacity mutation is allowed in the render callback. An over-capacity call is a bounded adapter/configuration failure, not a request to grow memory.
- Physical host input buses are represented separately from the four logical per-layer source routes. A route may alias a read-only physical input view, including a view shared by multiple layers. This does not imply four physical buses or four input copies.
- Master and layer output buffers are borrowed host views. The five stereo output buses represent ten channels when all are exposed, but they are not automatically engine-owned scratch. The memory ledger charges only actual internal working planes and staging copies.
- For float32 staging, `M = B × 4 × (C_in_staged + C_out_staged)`. Five stereo output copies conditionally cost `40 × B` bytes; one stereo input copy adds `8 × B`. This is an accounting example, not a required copy topology.
- Byte arithmetic uses checked wide values and validates conversion to `size_t` before allocation. Float32 sample width and 32-bit frame-count types do not imply a 32-bit process. Q-T03 remains open.
- VST3 zero-bus parameter flush is not an audio render and does not access scratch or advance audio/transport.
- Reconfiguration prepares a complete next capacity/bus generation off-thread, publishes only under the format/device lifecycle contract, and retires the previous generation after no callback can reference it.

Detailed equations, assumptions, conditional examples and planned tests are in `PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md`. No numeric memory budget, maximum block size or input-channel maximum is approved.



### 3.7 Cross-format event, automation and transport parity — IF-08 / IF-09 / IF-02

The format adapters preserve host semantics and normalize only the representation:

- AU absolute sample-time events and VST3 block-relative event offsets become validated discrete events in the shared block domain.
- AU parameter ramps and VST3 per-parameter automation queues become parameter curves, not flattened step events. AUValue uses the AU parameter's declared range/unit; VST3 ParamValue is normalized [0,1]. Resolve each host ID to the canonical parameter ID and convert through the immutable parameter descriptor. The VST3 previous-block anchor at relative position -1 remains signed continuity metadata.
- VST3 Note Expression preserves `noteId`; AU MPE maps cable/member-channel and note lifecycle to an internal note instance. Missing/ambiguous identity is explicit and never downgraded silently to channel-wide pitch bend.
- AU musical-context and transport-state callbacks have independent success/field validity. VST3 ProcessContext is optional and per-field state flags are authoritative. The normalized transport snapshot carries validity rather than guessed zero/default values.
- One transport snapshot is shared by the four layers and central sequencer. Valid host transport is the plugin authority; Standalone selects one authority. MIDI Clock is not a second plugin master.
- A VST3 no-audio-bus parameter flush is a control-only call and does not advance audio, transport or sequencer time.
- Cross-source events at one sample offset form a timestamp batch. Source-local order is retained, but writer precedence remains AR-01 and is not selected by this contract.
- Cycle/loop jumps inside a host block remain host-dependent; no intra-block musical continuity is claimed unless host evidence proves it.

See `PHASE3_CROSS_FORMAT_EVENT_TRANSPORT_PARITY_AUDIT.md` for API evidence, candidate normalized types and the Phase 5 validation families. This refinement does not change the 124-row product requirement inventory, the 53/124 acceptance-link baseline, or any approved product decision.

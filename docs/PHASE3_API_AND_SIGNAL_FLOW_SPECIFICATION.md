# Phase 3 API and Signal-Flow Specification

**Version:** 0.3 — Freeze snapshot concurrency contract added  
**Status:** Proposed for owner/architecture review. This document does not approve a framework, public C++ ABI, plugin topology, numeric queue capacity, DSP implementation or benchmark work.  
**Baseline:** Phase 2 merged baseline; DEC-015…DEC-042; IF-01…IF-12.  
**Companion documents:** `PHASE3_INTERFACE_CONTRACTS.md`, `THREADING_AND_OWNERSHIP_SPECIFICATION.md`, `SAMPLE_AND_STATE_LIFECYCLE_SPECIFICATION.md`, `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md`, `FREEZE_BEHAVIOR_SPECIFICATION.md`.

## 1. Purpose and decision boundary

This document converts the 12 cross-domain interface contracts into candidate C++-agnostic service boundaries and illustrative C++20-shaped signatures. The signatures are design notation, not approved source-level APIs. They intentionally avoid JUCE types so that the core architecture remains reviewable before Q-T01 is decided.

Three labels are used:

- **Required invariant** — necessary to preserve an approved product decision or real-time safety.
- **Proposed contract** — recommended interface and behavior; requires review.
- **Open decision** — must remain unresolved until owner choice, host evidence or measurement.

No class in this document is implemented. No capacity or performance figure is implied by an illustrative type.

## 2. Proposed module map

| Module | Responsibility | Owns | Must not own |
|---|---|---|---|
| `host/` | AU/VST3/Standalone adapters; translates host buffers, transport, events and state calls | Adapter-local host metadata and borrowed callback views | DSP runtime policy or persistent sample memory |
| `core/engine/` | Instance orchestration and render entry point | Active runtime graph on audio thread | UI widgets, file handles, decoder state |
| `core/parameter/` | Immutable parameter descriptors and normalized/plain conversion | Registry descriptors; audio-owned current values | Host-specific parameter objects |
| `core/events/` | Ordered MIDI, automation, sequencer and panic event stream | Bounded event records after copy/translation | Host-owned event pointers |
| `core/transport/` | Single transport authority and block time mapping | Current transport snapshot | Independent clocks in each layer |
| `engine/layer/` | Four layer runtimes, routing, gain, mute/solo, engine selection | Layer runtime state | Mutable UI state |
| `engine/voice/` | Voice allocation, envelopes, filters, sub-oscillator, per-note modulation | Preallocated voice state | Dynamic allocation during render |
| `engine/granular/`, `engine/sampler/` | R1 voice engines | Preallocated grain/playhead state | File decoding or sample mutation |
| `engine/modulation/` | LFOs, randoms, follower, matrix, macros and XY source | Runtime modulation state | Parameter registry mutation |
| `engine/fx/` | Per-layer R1 effect chain and tails | Audio-owned effect runtime | UI/plugin host objects |
| `sample/` | Decode, immutable sample assets, recording buffers and Freeze snapshots | Asset handles and non-RT preparation state | Active DSP graph mutation |
| `state/` | Versioned state snapshot, migration, serialization and relink | Immutable save/load snapshots and persistence work | Audio callback execution |
| `ui/` | Presentation, editing, controller mapping UX | UI model and observation snapshots | Mutable access to live DSP objects |
| `test/` | Deterministic offline render and validation harness | Test fixtures, input events and output files | Production runtime policy |

The four layers are a fixed product-level count (DEC-017). The internal module decomposition and names above are proposals.

## 3. Core data contracts

The following types are illustrative value/handle types. They are deliberately framework-free.

```cpp
using FrameCount = std::uint32_t;
using SampleOffset = std::uint32_t;
using SampleRateHz = double;
using ParameterId = std::string_view; // descriptor-owned stable storage
using LayerIndex = std::uint8_t;      // valid product values: 0..3

struct AudioBusView {
    float* channel[2];       // borrowed for current process call only
    std::uint8_t channels;   // negotiated channel count, validated by adapter
};

struct ConstAudioBusView {
    const float* channel[2];
    std::uint8_t channels;
};

struct ProcessBlock {
    AudioBusView mainOutput;
    AudioBusView layerOutput[4];       // inactive/unconnected buses are explicit
    ConstAudioBusView mainInput;
    ConstAudioBusView layerInput[4];   // only where host/device topology supplies it
    FrameCount frames;
    SampleRateHz sampleRate;
    const struct BlockContext* context; // borrowed; valid only during process()
};

struct TimedEvent {
    SampleOffset offset;
    std::uint32_t stableOrder;
    enum class Kind : std::uint8_t {
        NoteOn, NoteOff, Controller, PitchBend, ProgramChange,
        ParameterChange, Transport, Sequencer, AllSoundOff
    } kind;
    // Payload is a bounded tagged value in the concrete implementation.
};

struct BlockContext {
    // One immutable snapshot per process block.
    double tempoBpm;
    double ppqStart;
    bool playing;
    std::span<const TimedEvent> events; // valid only during current process call
};
```

**Lifetime rule:** all host buffers, spans and pointers in `ProcessBlock` and `BlockContext` are borrowed for the duration of `process()` only. An implementation must copy any event or data needed after the callback into bounded owned storage. No callback pointer may be retained.

The exact representation of optional buses, channel counts, sample formats and host-specific event payloads is delegated to the adapter specification and host validation.

## 4. Candidate service interfaces

### 4.1 Engine render boundary — IF-01

```cpp
class IAudioEngine {
public:
    virtual ~IAudioEngine() = default; // destruction is non-RT
    virtual void prepare(const EngineConfig&) = 0; // non-RT
    virtual void process(const ProcessBlock&) noexcept = 0; // RT
    virtual void reset(ResetReason) noexcept = 0; // RT-safe bounded reset only
    virtual void publishUiSnapshot(UiSnapshot&) const noexcept = 0;
};
```

Contract:
- `prepare` constructs/replaces rate-dependent runtime resources off the audio thread.
- `process` performs bounded work and never allocates, blocks, logs, performs I/O or destroys objects.
- `reset` is not a general graph rebuild. Structural preparation belongs off-thread and is published through IF-03.
- The host adapter clears/initializes output regions as required, validates bus availability and passes explicit active-bus information.
- The engine must safely render silence or a defined fallback for invalid/unconnected optional inputs; it must not dereference null or inactive bus memory.

### 4.2 Parameter registry and runtime — IF-02

```cpp
enum class ParameterScope : std::uint8_t { Global, Layer, Voice, Performance, Sequencer };
enum class UpdateClass : std::uint8_t { AudioRate, ControlRate, SpawnSampled, Stepped, Structural };

struct ParameterDescriptor {
    ParameterId id;
    ParameterScope scope;
    UpdateClass updateClass;
    double minimum;
    double maximum;
    double defaultValue;
    bool automatable;
    bool modulatable;
};

class IParameterRegistry {
public:
    virtual const ParameterDescriptor* find(ParameterId) const noexcept = 0;
    virtual double toPlain(ParameterId, double normalized) const noexcept = 0;
    virtual double toNormalized(ParameterId, double plain) const noexcept = 0;
};
```

Contract:
- Registry descriptors are immutable after initialization. The candidate scope set is Global, Layer, Voice, Performance and Sequencer, aligned with the proposed canonical-ID scope vocabulary in `PARAMETER_ID_SPECIFICATION.md`; adding a new scope requires an explicit schema/API revision.
- Stable string IDs are canonical. Numeric host IDs are adapter mappings and may never be recycled after release; the final scheme remains Q-T12.
- Unknown IDs are ignored safely and surfaced through non-RT diagnostics.
- Parameter changes carry a stable ID or pre-resolved registry index plus scope and value. The concrete representation must avoid string lookup in the innermost DSP loop.
- DEC-036 remains exactly ten macros per patch. DEC-031 shared macro base values and per-note modulation are preserved.
- Parameter smoothing and modulation evaluation order must be fully specified before implementation; this interface does not decide Q-T13 or the final host ID mapping.

### 4.3 Structural command publication — IF-03

```cpp
enum class CommandKind : std::uint8_t {
    ReplaceSample, ReplaceLayerRuntime, ReplaceFxChain, ReplacePatch,
    SetFreezeSource, ThawLayer, SetRecordingState, Panic
};

struct StructuralCommand {
    CommandKind kind;
    LayerIndex layer;
    std::uint64_t generation;
    const void* immutablePayload; // concrete typed handle in implementation
};

class IAudioCommandSink {
public:
    virtual bool tryPublish(const StructuralCommand&) noexcept = 0;
};
```

Contract:
- The command producer fully constructs and validates payloads off-thread.
- Successful publication transfers a documented retained handle/reference to the audio domain; failed publication leaves ownership with the producer.
- The audio thread applies commands only at documented safe points and never constructs payloads.
- The concrete implementation must use typed payloads, not untyped `void*`; the pointer above only illustrates the boundary.
- Separate producer domains require separate SPSC queues or a proven bounded MPSC implementation. The current threading proposal favors per-producer SPSC queues.
- Queue capacities, priority order and coalescing rules remain measurement/implementation details. Panic/All Sound Off requires a reserved, bounded delivery path.

### 4.4 UI observation — IF-04

```cpp
struct LayerUiState {
    float peakLeft, peakRight;
    float recordProgress;
    float playheadNormalized;
    bool recording;
    bool frozen;
    bool sourceAvailable;
};

struct UiSnapshot {
    std::uint64_t sequence;
    LayerUiState layer[4];
    float masterPeakLeft, masterPeakRight;
};

class IUiSnapshotSource {
public:
    virtual bool tryReadLatest(UiSnapshot& destination) const noexcept = 0;
};
```

Contract:
- Audio publishes fixed-size observational state; UI reads at its own cadence.
- Stale frames may be skipped. UI must never wait for a new snapshot.
- Snapshot fields are values only; no pointers to live runtime objects cross into UI.
- The concrete publication mechanism (triple buffer or sequence counter) must be proven race-free under the chosen memory model.

### 4.5 Immutable sample store — IF-05

```cpp
struct SampleMetadata {
    SampleRateHz sampleRate;
    std::uint64_t frameCount;
    std::uint8_t channelCount;
    std::uint64_t contentHash;
};

class ISampleAsset {
public:
    virtual const SampleMetadata& metadata() const noexcept = 0;
    virtual const float* channelData(std::uint8_t channel) const noexcept = 0;
};

class ISampleStore {
public:
    // Non-RT: decode/prepare and return an immutable owned handle.
    virtual PreparedSampleResult prepareAsset(const SampleRequest&) = 0;
};
```

Contract:
- Decoding, resampling/preparation, peak analysis and allocation occur off-thread.
- Published sample assets are immutable for their entire lifetime.
- Active sample replacement is atomic at an audio safe point; previous valid source remains active until replacement is ready.
- The engine retains an explicit lifetime handle while any voice, grain, sampler head or Freeze snapshot references the asset.
- The R1 granular duration target remains at least six minutes stereo at 48 kHz subject to measured feasibility and safe memory (DEC-028); this document does not claim feasibility.
- Long-file streaming for Sampler remains a proposed L2 strategy (Q-T05), not an R1 implementation decision.

### 4.6 Recorder and recording snapshots — IF-06

```cpp
enum class RecordTap : std::uint8_t {
    LayerInputPreEngine,
    LayerEnginePreFx,
    LayerPostFxPreLayerGain,
    LayerPostLayerGain,
    MasterPreOutput
};

struct RecordingDescriptor {
    RecordTap tap;
    SampleRateHz sampleRate;
    std::uint8_t channels;
    std::uint64_t committedFrames;
    std::uint64_t sourceGeneration;
};

class IRecorder {
public:
    virtual void processCapture(const ConstAudioBusView&, FrameCount) noexcept = 0;
    virtual RecordingStatus status() const noexcept = 0;
    virtual bool requestSnapshot(RecordingSnapshotRequest) noexcept = 0;
};
```

Contract:
- Capture buffers are preallocated; callback performs bounded writes only.
- Disk I/O, encoding, export and state serialization occur off-thread.
- A snapshot exposes only committed frames. Uncommitted/writer-owned frames are excluded from Freeze and serialization.
- Buffer exhaustion stops capture safely, preserves committed audio and reports an overrun; it must not silently wrap and corrupt an existing take.
- Independent recording continues during Freeze (DEC-042).
- **Recommended engineering defaults (pending owner review and host/UX validation):** per-layer recording uses `LayerPostFxPreLayerGain`; stereo master recording uses `MasterPreOutput`. These defaults are defined in §§7–8 and are not new product-level decisions. The enum retains other candidate positions for future use, but R1 does not expose additional recorder modes by implication.

### 4.7 Freeze source publication — IF-07

```cpp
struct FreezeSnapshot {
    SampleHandle source;
    std::uint64_t sourceGeneration;
    std::uint64_t firstFrame;
    std::uint64_t frameCount;
    std::uint64_t committedFrameCount;
};

class IFreezeController {
public:
    bool requestLayerFreeze(LayerIndex) noexcept;
    bool requestGlobalFreeze() noexcept;
    bool requestThaw(LayerIndex) noexcept;
    FreezeStatus status(LayerIndex) const noexcept;
};
```

Contract:
- Snapshot preparation copies or references the exact valid source region off-thread and publishes an immutable handle.
- A committed-frame count is a validity boundary, not a lifetime guarantee for a rolling ring. Before an off-thread copy, the coordinator must acquire a bounded retention lease that prevents the selected pages from being overwritten. Recording continues into a separate writable region; the audio thread never waits or copies the source. If a lease/spare capacity is unavailable, preparation fails before source publication and the previous source/recorder state remain unchanged.
- The snapshot carries and validates source generation, first frame and committed range; stale or overwritten generations are rejected rather than read concurrently.
- The audio thread switches source at a safe logical boundary. Existing grains/voices continue; new MIDI/sequencer events use the frozen snapshot.
- Modulation, envelopes, LFOs, MIDI, sequencer, shared transport, layer FX and effect tails continue.
- Recording continues independently at its defined tap.
- Thaw crossfades to the prior live/rolling source.
- Global Freeze is represented by one bounded transaction payload containing four per-layer preparation outcomes; it must not be split into four independently queued layer commands. At one safe point, all ready layers switch together; failed layers retain their prior source and expose per-layer failure. A transaction-publication failure changes no layer. The UI reports partial failure and never reports full global success unless all four switched. **Owner review required:** the proposed behavior applies the successful subset when one or more layer preparations fail; DEC-042 requires visible partial failure but does not explicitly choose successful-subset versus all-or-nothing switching.
- Static library samples do not incur redundant copies.
- Snapshot persistence follows DEC-025. Memory exhaustion or missing source preserves the previous valid source.
- This interface implements DEC-042; it does not reopen Freeze product semantics. Exact crossfade duration, memory budget and copy algorithm remain engineering values to validate.

### 4.8 Transport authority — IF-08

```cpp
struct TransportSnapshot {
    double tempoBpm;
    double ppqPosition;
    bool playing;
    enum class Source : std::uint8_t { Host, Internal, MidiClock, AbletonLink } source;
};

class ITransport {
public:
    TransportSnapshot snapshotForBlock(const HostTransportInput&) noexcept;
    void acceptMidiClock(const TimedEvent&) noexcept;
};
```

Contract:
- Exactly one active transport authority feeds the shared sequencer for a given runtime.
- Layer engines do not independently advance musical time.
- DEC-039 semantics are preserved: Start resets shared sequencer; Continue resumes; Stop halts at current position; after two expected beat intervals without pulses, clock-dependent progression stops at current position; SPP is ignored in R1.
- Host/device/Link arbitration and discontinuity handling require final specification and host tests; this interface does not settle Q-P/host precedence.

### 4.9 MIDI and event router — IF-09

```cpp
struct RoutedEvent {
    TimedEvent event;
    std::uint8_t targetLayerMask; // bits 0..3
};

class IMidiEventRouter {
public:
    void routeHostEvents(std::span<const TimedEvent>,
                         BoundedEventWriter&) const noexcept;
    void routeDeviceEvent(const DeviceMidiEvent&,
                          BoundedEventWriter&) const noexcept;
};
```

Contract:
- Host event memory is borrowed only during the process callback; device events are copied into bounded owned records before later use.
- Stable ordering is by sample offset, then explicit source/order tie-break.
- If multiple layers share a MIDI channel, all matching layers receive the event (DEC-034).
- MPE per-note pitch bend range is 48 semitones (DEC-038); zone negotiation remains a host/device integration detail.
- Program Change selects only an explicitly assigned item in the active performance bank; unmapped values are ignored (DEC-033).
- MIDI Clock behavior is owned by transport, not duplicated inside the router.
- Panic/All Sound Off must be deliverable under overload.

### 4.10 Layer graph, FX and mixer — IF-10

```cpp
class ILayerProcessor {
public:
    virtual void processVoices(const LayerProcessContext&, StereoBlock&) noexcept = 0;
    virtual void processInput(const LayerProcessContext&, StereoBlock&) noexcept = 0;
    virtual void processEffects(const LayerProcessContext&, StereoBlock&) noexcept = 0;
    virtual void processOutput(const LayerProcessContext&, StereoBlock&) noexcept = 0;
};

class IMasterMixer {
public:
    virtual void mixLayers(const LayerBlockView[4], StereoBlock& master,
                           LayerBlockView[4] layerOutputs) noexcept = 0;
};
```

Contract:
- Four layer processors are independent; the central sequencer and shared transport remain instance-level.
- R1 provides four independent serial FX slots per layer (AC-FX-001), with user reordering (AC-FX-002), and seven available effect types approved by DEC-023: Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator and Compressor. Four slots are the chain capacity; seven is the effect-type inventory. Per-effect algorithms, parameter ranges and tail behavior remain subject to the Effects Specification and validation.
- Layer output and master output must be deterministic and must not alias unsafely.
- FX tails continue through Freeze; All Sound Off silences voices/effect tails without thawing source.
- Live-input routing and dry monitoring path must be explicit. Mono live input is centered and available to both sides of stereo layer processing (DEC-040); explicit LL/RR duplication is not the default.
- Recommended recording tap and mute/solo semantics are consolidated in §7, §8 and §13; they remain proposed pending host and UX validation.

### 4.11 State snapshot and serializer — IF-11

```cpp
struct StateSnapshot {
    std::uint32_t schemaVersion;
    ImmutablePatchState patch;
    ImmutableSequencerState sequencer;
    std::array<RecordingSnapshotHandle, 4> unsavedRecordings;
    std::array<ExternalAssetReference, 4> libraryAssets;
};

class IStateService {
public:
    StateSnapshot captureSnapshot(const StateCaptureRequest&); // non-RT
    SerializedState serialize(const StateSnapshot&);           // non-RT
    RestorePlan inspectAndMigrate(const SerializedState&);     // non-RT
    RestoreResult prepareRestore(const RestorePlan&);           // non-RT
};
```

Contract:
- Unsaved recording buffers are embedded in plugin state; library samples remain external references with relink/collect support (DEC-025).
- State capture must obtain immutable/committed snapshots without asking the audio thread to block.
- Restore decodes and prepares assets off-thread; the active runtime is replaced only through IF-03 safe-point publication.
- Missing external samples do not invalidate unrelated patch/sequencer state.
- Corrupt or unsupported chunks produce explicit recovery status.
- No silent omission of unsaved audio is permitted. State size/chunking and host limits require explicit failure or a validated bounded policy; exact encoding remains open.

### 4.12 Deterministic render harness — IF-12

```cpp
struct RenderFixture {
    StateSnapshot initialState;
    std::span<const TimedEvent> events;
    std::span<const ConstAudioBusView> inputBlocks;
    std::uint64_t randomSeed;
    SampleRateHz sampleRate;
};

class IOfflineRenderer {
public:
    RenderResult render(const RenderFixture&, MutableAudioOutput&) const;
};
```

Contract:
- Test harness owns all input/output buffers and fixture state for the render duration.
- Identical state, inputs, ordered events, seed, sample rate and render mode must produce deterministic output within the declared numerical tolerance.
- Device I/O and explicitly host-specific differences are outside standalone/plugin bit-equivalence claims.
- No benchmark implementation or performance measurement is authorized by this document. Q-T07 still requires separate owner authorization for a benchmark spike.

## 5. Proposed render-block ordering

This ordering is a candidate deterministic contract. Sample-accurate events split the block into segments; it is not permission to defer all events to block boundaries.

1. Validate block/bus views and acquire one transport snapshot.
2. Merge host automation/MIDI with bounded device events and sequencer events using sample offsets and deterministic tie-break.
3. Drain bounded structural commands at the declared safe point; publish/retire immutable handles without destruction on RT.
4. For each event segment: apply due parameter/event changes; update modulation according to each parameter's declared update class; render voices/engines.
5. Render layer live-input path and synth voices according to the finalized layer signal graph.
6. Apply per-layer FX and layer output controls.
7. Capture per-layer recording at the selected tap(s); capture master recording at its selected tap.
8. Mix four layers and any approved dry-input path to master; write connected host outputs only.
9. Publish fixed-size UI/meter/record/freeze observations.
10. Advance bounded counters and complete the callback.

**Ordering caveat:** steps 5–8 cannot be finalized until the tap/routing decisions in §8 are approved. The recorder tap is a point in the graph, not a post-hoc copy of an arbitrary buffer.

## 6. Recommended voice and layer signal flow (proposal)

The approved GS-ARC-007 signal chain requires a per-voice engine + sub-oscillator path through LPF then HPF, a per-layer four-slot FX/M-S/patch-volume/layer-mix/wet path, and a parallel dry live-input path summed before the master. The following is the recommended R1 topology for review; it preserves those requirements while making recording and output taps explicit.

```text
MIDI / Sequencer / Note Events
             |
       Voice Allocator
             |
     +-------+----------------------+
     |                              |
Granular Voice                 Sampler Voice
(grain scheduler/pool)         (playhead/loop)
     |                              |
     +---------------+--------------+
                     |
              Voice Source Sum
                     |
             Sub-Oscillator Mix
                     |
                  LPF Stage
                     |
                  HPF Stage
                     |
              Amp Envelope
                     |
                  Voice Pan
                     |
              Layer Voice Sum
                     |
       +-------------+-------------------+
       |                                 |
Layer Live Input                    Synth Voices
       |                                 |
Input Gain / Mono Centering              |
       +---------------+-----------------+
                       |
                 Layer Input Sum
                       |
                Four-Slot FX Chain
                       |
                  M-S / Patch Gain
                       |
                Layer Mix / Wet Path
                       |
                Post-FX Layer Signal
                       |
          +------------+-------------------+
          |                                |
Layer Stem Output + Recording       Layer Fader / Mute / Solo
(post-FX, pre-fader)                        |
                                           +----> Master Mixer <---- Dry Live Input
                                                      |
                                               Stereo Master
                                                      |
                                        Master Output + Recording
                                               (pre-output)
```

Notes:
- This is the recommended topology for architecture review; it remains a proposal pending owner review and host/UX validation. Numeric DSP behavior and framework-specific routing remain unapproved.
- The approved GS-ARC-007 voice chain is engine + sub-oscillator → LPF → HPF. The proposed placement of amp envelope and pan after the filters and before layer summation remains an engineering detail to validate against voice-level acceptance fixtures.
- Live input has a wet path into its layer before the four-slot FX chain and a separate dry path summed before the master, as explicitly required by GS-ARC-007. Mono input is centered under DEC-040. The dry path is intentional product behavior, not an optional bypass; its gain staging and monitoring behavior require validation.
- Layer output buses and master output mapping must follow the negotiated plugin bus topology. The AU input/component strategy remains open (Q-P06).
- Freeze captures the approved source snapshot for that layer. It does not freeze the FX chain, transport, modulation or recording process.

## 7. Recording tap semantics — recommended R1 defaults (proposal)

The recorder interface supports these candidate positions:

| Candidate | Signal captured | Main consequence |
|---|---|---|
| Layer Input Pre-Engine | Raw layer source/input before voice engine | Useful for source capture; excludes synthesis and layer FX |
| Layer Engine Pre-FX | Sum of engine/live input before layer FX | Captures performed layer without effect tails |
| Layer Post-FX Pre-Layer-Gain | Wet layer output before layer output gain/mute | Captures effect processing; independent of layer fader |
| Layer Post-Layer-Gain | Final layer signal after gain/mute/solo logic | Matches what layer contributes to mix |
| Master Pre-Output | Final master mix before host output | Captures wet layer mix plus the required parallel dry live-input path |

**Recommended R1 architecture default (not a new product decision):** define two fixed recording sources:
- Per-layer recording: Layer Post-FX Pre-Layer-Gain, so a layer take includes its effect character but can be mixed later.
- Stereo master recording: Master Pre-Output, so the recorded mix matches the master signal sent to the host.
- Keep raw/input capture as a separately selectable source only if product requirements and UI scope justify it; do not silently add a third recorder mode.

Validation questions (not unresolved product semantics):
1. Confirm in Standalone and each target host that the per-layer tap is post-FX and pre-layer-fader, excluding fader/mute/solo.
2. Confirm the master tap is the final stereo master before host/device output and includes both wet live input routed through layer FX and the parallel dry live-input path required by GS-ARC-007.
3. R1 has no approved master FX chain; do not introduce one in this specification.
4. Verify that an armed layer continues recording its stem while muted or solo-isolated.

Freeze and recording relationship is not open at product level: recording continues independently during Freeze (DEC-042). The recommended recording taps are defined here; implementation equivalence remains a validation gate.

## 8. Output bus and dry-input routing

Proposed output model:
- Main output: stereo master.
- Four optional stereo layer outputs, one per layer.
- A layer output represents the defined final layer signal at its selected bus tap. It must not be double-counted into the master if the host separately routes layer outputs.
- Main output continues to provide the master mix even when optional layer buses are inactive, unless a host-specific topology explicitly requires a different mapping.
- Inactive/unconnected outputs are ignored safely; the engine must not write through null bus pointers.
- Live input feeds the layer FX path and the required parallel dry-to-master path under GS-ARC-007. Validate gain staging, monitoring latency, channel mapping and feedback safety in Standalone and target hosts.

**Recommended default:** each layer output is post-FX and pre-layer-fader, matching the per-layer recording tap and preserving independently mixable stems for DAW workflows. Master output applies layer fader/mute/solo. This remains a proposed architecture default pending host validation; it does not change the approved five-output product model.

## 9. Real-time ownership and safe-point sequence

Structural replacement follows this lifecycle:

1. Non-RT producer validates request and builds a complete immutable payload.
2. Producer acquires a bounded publication slot/queue entry.
3. On success, ownership is transferred according to the command contract; on failure, producer retains and may retry/coalesce/reject.
4. Audio thread consumes the command at a deterministic safe point and swaps the active handle.
5. The old handle is placed in a bounded retirement path; audio never performs final destruction.
6. Housekeeping thread destroys retired resources and signals completion/status through non-RT observation.

No command may require the audio thread to allocate, decode, copy a large buffer, construct an effect, open a file or wait for another thread. Freeze snapshot creation is off-thread; only the source-handle transition occurs on audio.

## 10. Validation matrix (planned, not executed)

| Area | Required evidence before implementation-readiness |
|---|---|
| Process boundary | Null/inactive buses, mono/stereo layouts, variable blocks, sample-rate change, offline process, host buffer aliasing |
| Event ordering | Same-offset host automation/MIDI/sequencer ordering; sample-accurate offsets; panic under saturation |
| Parameters | Stable ID uniqueness, host mapping, normalized/plain round-trip, unknown-ID migration, automation continuity |
| Ownership | No RT allocation/lock/destruction; queue saturation; retire-queue saturation; replacement failure preserves active graph |
| Sample lifecycle | Large-file decode, immutable handle lifetime, replacement during active grains, missing/corrupt assets |
| Recording | Each tap independently verified with impulse/routing fixtures; committed-frame boundary; overrun; save during active recording |
| Freeze | Source snapshot region, active voices continue, new notes use snapshot, rolling-ring retention lease and ring-wrap safety, concurrent recording, one global transaction boundary with mixed per-layer outcomes, partial failure, thaw crossfade and state restore |
| Routing | Four layer stems plus master; no double count; mute/solo/fader semantics; direct input monitor; FX tails |
| State | Embedded unsaved audio, external relink, corrupt chunks, interrupted save, unsupported schema, host size limits |
| Determinism | Same fixture/seed gives repeatable render; standalone/plugin equivalence within declared boundary |

These are obligations, not evidence of passing tests. No benchmark code, production DSP, plugin shell or UI implementation is included or authorized.

## 11. Open decisions carried forward

| Decision | Current status | Required next evidence/owner action |
|---|---|---|
| Q-T01 framework/language | Proposal only | Compare framework-free C++ core and selected host framework; licensing and host API proof |
| Q-T02 core independence | Proposal only | Approve boundary and dependency policy |
| Q-T03 macOS minimum / Universal 2 | Proposal only | Verify supported host/OS and toolchain matrix |
| Q-T07 grain budget | Open | Separate explicit authorization for isolated benchmark spike; measure on owner-approved reference machine |
| Q-T08 parallel rendering | Proposal only | Keep disabled for initial R1 unless profiling later justifies it |
| Q-T09 RT tooling | Proposal only | Choose allocation detector/RTSan feasibility after toolchain |
| Q-T12 parameter IDs/host exposure | Proposal only | Owner approval required before first public release |
| Q-P06 AU live-input/component strategy | Open | Validate AU component types and live-input behavior in target hosts |
| Q-P09/Q-P10 host/reference machine | Open | Owner supplies or approves host/device matrix and reference machine |
| Recording tap positions | Proposed defaults defined in §§7–8 | Owner review plus host/UX validation; verify against Audio I/O and lifecycle specs |
| Dry input and mute/solo capture semantics | Proposed defaults defined in §§7–8 | Validate monitoring, stem independence and master correspondence |
| Exact filter/sub-osc/amp order | Proposed order defined in §6 | Validate against voice-level acceptance fixtures before implementation |
| Layer output tap position | Proposed post-FX/pre-fader tap | Validate host buses, stems and Standalone live workflow |
| Queue capacities and retirement bounds | Open | Derive from burst analysis and tests; do not invent numeric limits |
| State container/chunking and maximum size | Open | Host evidence and DEC-025-compliant serialization design |
| FX slots and inventory | Four serial slots per layer (AC-FX-001/002); seven effect types (DEC-023) | Validate per-effect algorithms, parameter ranges, tail semantics and signal routing; no unresolved slot-count conflict |

## 12. Phase 3 exit impact

This document advances IF-01…IF-12 from abstract contracts to candidate service boundaries, typed data concepts, lifecycle rules and a visible signal-flow proposal. It does **not** satisfy Phase 3 exit criteria by itself.

Still required:
1. Owner review of the explicit recording/routing recommendations and open choices.
2. Concrete class/API traceability for each of the 124 R1 rows, including exact ownership of each behavior.
3. Owner-reviewed signal-flow proposal, then host-validated recording tap and bus semantics.
4. Host/component evidence for AU live input and VST3 buses.
5. Final parameter ID and framework decisions.
6. RT queue and retirement design with capacities justified by analysis/testing.
7. Q-T07 measurement only after separate explicit authorization.
8. Updated architecture audit and PR description after the above evidence is complete.

**Audit statement:** this is a proposed API and signal-flow design artifact. It contains no implementation and makes no claim that the product is implementation-ready.


## 13. Recommended routing defaults — consolidated

For consistency across the engine, recorder and plugin adapters, the following defaults are recommended:

| Signal | Recommended position | Fader / mute / solo effect |
|---|---|---|
| Per-layer recording | Post-FX, pre-layer fader | Not affected; muted/solo-isolated layer can still record its isolated stem |
| Layer output bus | Post-FX, pre-layer fader | Not affected; host receives a mixable stem |
| Master recording | Final stereo master, pre-host output | Includes layer faders, mute/solo and any live input routed through layers |
| Live input | Per-layer wet feed before layer FX plus parallel dry feed summed before master | Wet feed affects layer FX/stem; dry feed contributes directly to master as required by GS-ARC-007 |
| Dry live-input path | Required by GS-ARC-007; summed before master | Parallel dry feed is intentional; validate gain staging and avoid unintended double-counting in host bus adapters |
| Voice processing | Engine + sub-oscillator → LPF → HPF → proposed amp envelope/pan → layer sum | Preserves GS-ARC-007 filter order; validate envelope/pan placement |

These are coherent engineering defaults, not changes to the approved product feature set. They require validation against Standalone monitoring latency, host bus behavior and the recording UX. If host constraints require a different AU/VST3 topology, adapters must preserve the same semantic signal taps where possible and document any limitation.

# Plugin Component and Bus Specification

**Version:** 0.6 — AU and VST3 component decision gates clarified  
**Status:** Proposed; host validation and owner approval required. No plugin implementation is authorized.  
**Authority:** DEC-020/026, GS-PLT-001/004/005, GS-ARC-006, GS-SMG-020, Q-P06/Q-P09/Q-T01/Q-T03.

## 1. R1 product commitments

- macOS Standalone, AU and VST3 are R1 formats (DEC-026).
- Standalone is a complete instrument with audio/MIDI device selection and live input.
- Every layer can receive live external audio; this applies to Standalone and plugin contexts (DEC-020).
- Output model is stereo master plus four stereo layer outputs (GS-ARC-006).
- Four layers share the same engine/state implementation and one transport/sequencer.
- CLAP is optional, not an R1 release requirement.
- Host limitations must be reported honestly; unsupported input/output configurations cannot be silently represented as supported.

## 2. Shared core boundary

All formats instantiate the same framework-independent engine and state model. Format adapters translate host lifecycle, audio buses, MIDI events, transport, parameter automation and state serialization into the shared core contract.

Proposed layers:
- `core/`: real-time primitives and DSP utilities, no plugin-framework types.
- `engine/`: voices, Granular/Sampler engines, modulation, FX, transport and sequencer.
- `sample/`: immutable prepared samples and recording buffers.
- `state/`: parameter registry, patch/multi/library state, serialization and migration.
- `host/`: JUCE wrapper or direct AU/VST3 adapters plus Standalone device host.
- `ui/`: editor and Standalone presentation using snapshots/commands.

The same core must be buildable and renderable headlessly.

## 3. Proposed bus model

### 3.1 VST3

Candidate instrument component:
- Main event input: MIDI event bus.
- Main stereo audio output: master mix, default active.
- Four stereo auxiliary audio outputs: Layer 1–4, optional/deactivatable where the host permits.
- Optional live-input bus collection, with one stereo input as the minimum host-validation configuration. This is not a product-level maximum: DEC-020 does not impose a fixed product cap on live-input channels/sources. The adapter must normalize the host's negotiated input bus/channel collection and expose explicit per-layer source routing; a read-only input view may be shared by multiple layers without copying. Actual additional input availability is host/device-specific and must be validated.

The VST3 API supports multiple dynamic audio input/output buses and event buses. That API capability does not guarantee identical host UI/routing behavior.

Instrument identity is communicated through the VST3 plug-in class subcategory (candidate: `Instrument|Synth|Sampler`), not by selecting a separate processor base interface. The SDK's `AudioEffect` helper can declare audio and event buses and may be used for an instrument-classified component; its class name does not make the host-visible plug-in an insert effect. See `PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md`.

The baseline is one 16-channel event input bus, with internal channel-to-layer routing. The four product layers are not automatically four VST3 event buses or Units. The live-audio input bus type (`kMain` versus `kAux`) remains open until the target hosts prove full-band source delivery. Do not claim VST3 simple mode or a dual Fx/Instrument category without host evidence.

### 3.2 Audio Unit

Current validation candidate (not selected):
- Retain `aumu` as the first AU prototype candidate because R1 is a MIDI-playable Granular/Sampler instrument and instrument-track identity is the clearest primary workflow.
- Test one optional full-band stereo input bus as a baseline configuration only. The generic AU inputBusses API does not prove that hosts expose this bus on an `aumu` component or route it to the engine's sample-source path.
- A sidechain/detector/modulation input is not a pass unless the raw audio samples are available to the layer's Granular/Sampler source path.

Alternative:
- Test `aufm` as a MIDI-responsive audio-processing component. Its audio-effect role is a natural fit for live audio input, but it must also pass the complete R1 instrument workflow: Note On/Off, velocity, channel routing, MPE, sequencer events, required outputs, automation and state.
- A dual `aumu` + `aufm` package is a last-resort product option only if evidence proves that distinct required workflows cannot be served by either single component.

AU component type (`aumu`/`aufm`) and AU generation (v2/v3) are separate decisions. Neither is selected here. Sharing a core/state schema does not guarantee host-level substitution or automatic state transfer between distinct component identities.

### 3.3 Standalone

- User selects input/output device and channel configuration.
- Default master output is stereo.
- Four stereo layer outputs can be routed to available device channels when the device supports them.
- Mono input is centered and available to both sides of stereo layer processing (DEC-040).
- Device loss/reconnect, unavailable input, sample-rate changes and channel-count changes have explicit safe states.

## 4. Required host capability matrix

The release host matrix proposed in Q-P09 is Logic Pro, Ableton Live, Bitwig and Reaper. Exact versions and minimum supported versions remain to be set.

**Evidence status:** the public-documentation cross-check is maintained in `PHASE3_HOST_CAPABILITY_EVIDENCE_MATRIX.md`. It distinguishes multi-output routing from full live-audio source input and documents format applicability (notably, Bitwig does not support AU). Vendor documentation is not a host test for GRANULAR SYNTH; no host/format cell is certified until the prototype evidence described in that matrix exists.

For every host × format, test:

| Capability | Required evidence |
|---|---|
| Scan and instantiate | Host scans, loads and identifies the component correctly |
| MIDI input | Note, CC, pitch bend, channel pressure, poly pressure and MPE behavior |
| Host transport | Tempo, PPQ, play/stop, loop/seek and sample-accurate event offsets where exposed |
| Live audio input | Correct routing to each layer, mono/stereo mapping and no feedback |
| Main output | Stereo master output |
| Layer outputs | Four independent stereo layer outputs where supported; documented fallback where not |
| Parameter automation | Stable IDs, gestures, read/write and automation playback |
| State | Save session, close host, reopen, restore embedded recordings and relink external samples |
| Offline render | Deterministic render with documented host/offline differences |
| Bypass/latency | Correct bypass and latency reporting; R1 Granular/Sampler zero-added-latency |
| Lifecycle | Suspend/resume, sample-rate and buffer-size changes, project close and shutdown |

A host/format cell is not considered supported until all required tests for that cell pass or a specific non-supported capability is documented and approved.

## 5. Transport and MIDI adapter contract

- Plugin formats consume host transport when valid and available.
- Standalone supports internal clock, external MIDI Clock and Ableton Link.
- Host transport and external MIDI Clock arbitration must not create two active clock masters in plugin contexts.
- MIDI Clock output and sequencer MIDI output are not R1 commitments.
- Program Change is handled only where the host delivers it; mapping behavior follows DEC-033.
- MPE member-channel and per-note expression routing follows DEC-038; host negotiation remains a validation item.

## 6. Parameter and state adapter contract

- Host numeric IDs map to canonical string IDs via the approved parameter manifest.
- No adapter may assign IDs based on registration order.
- Host state serialization must implement DEC-025: embed unsaved recordings; reference library files with relink/collect.
- AU/VST3 save/restore callbacks must not cause disk I/O or unbounded waiting on the audio thread.
- Host parameter gestures and automation must preserve identical semantic values across Standalone/AU/VST3.
- Host-specific unsupported operations must be surfaced rather than silently ignored.

## 6.1 Normalized host-process contract (adapter boundary)

Every format adapter must translate its host callback into one framework-independent process description. This is a proposed core contract, not a final C++ ABI:

| Field | Normalized meaning | Required handling |
|---|---|---|
| Frame count | Current callback's valid frame range and process kind | Preserve the format's actual frame count. VST3 parameter-flush calls may contain no audio buses (numInputs = 0, numOutputs = 0); do not infer a zero-frame audio block from missing buses. AU render capacity is bounded by the host-set maximumFramesToRender. |
| Sample rate | Current processing rate | Detect changes and request non-RT reconfiguration; never rebuild DSP graphs in the callback |
| Input/output bus map | Bus identity, activation, channel count and buffer validity for this call | Inactive/unconnected buses are explicit; never infer activity from a non-null pointer alone |
| Event view | Borrowed host MIDI/parameter/transport events with timestamps | Copy only required event data into bounded engine-owned records before callback return |
| Transport snapshot | One coherent host transport observation for this block | Missing/invalid fields are flagged; adapters do not fabricate tempo/PPQ |
| Process mode | Realtime, offline, pre-roll, suspended or host-specific mode | Map explicitly; deterministic engine behavior receives mode as input |
| Lifecycle generation | Adapter generation for start/stop/reconfigure/restore transitions | Stale prepared configurations cannot activate after a newer lifecycle generation |

The adapter owns host SDK objects and host callback buffers. The shared core receives only normalized views and engine-owned bounded event records; no host SDK type crosses into `core/` or `engine/`.

### Bus activation and routing rules

1. Treat bus identity separately from bus activation. A declared optional bus may be inactive, have zero channels, or be represented by absent/null buffers depending on host/API; each case has an explicit normalized state.
2. Build the per-call routing map from the negotiated bus arrangement. Never assume bus index equals layer index unless the adapter's stable mapping explicitly establishes it.
3. The master output is the primary stereo output. Layer 1–4 outputs are independent optional outputs; an inactive layer output must not disable that layer's contribution to the master.
4. If a host cannot expose a layer output, the layer still contributes once to the master. Do not emulate an unavailable output by adding a second copy into the master.
5. If the live-input bus is unavailable or unconnected, the adapter supplies an explicit unavailable-input state; the core does not read a null/uninitialized buffer. Live-input-dependent features report unavailable while the instrument path remains usable.
6. Channel layouts are validated before rendering. Mono input follows DEC-040 centering; unsupported channel counts must be rejected or mapped by an explicitly documented adapter policy, never guessed.
7. Bus arrangement changes are structural lifecycle changes. Prepare new routing state off-thread and publish at a safe point; do not resize buffers or allocate in the process callback.

### Clock and transport authority by format

- **AU/VST3:** valid host transport is the authoritative transport source while supplied by the host. MIDI Clock arriving through plugin MIDI input must not become a second active master. If host transport is absent/invalid, the adapter reports that condition; fallback policy must be explicit per format/host and must not silently switch clocks mid-playback.
- **Standalone:** exactly one selected authority advances shared transport at a time: internal clock, external MIDI Clock or Ableton Link. Switching authority is an explicit control/lifecycle event with deterministic phase policy; no two clocks advance transport concurrently.
- The engine receives a single normalized transport snapshot/clock authority and does not arbitrate host SDK objects itself.
- DEC-039 MIDI Clock Start/Continue/Stop/timeout behavior applies to the MIDI Clock authority; it does not override valid host transport in plugin contexts.

### State and lifecycle callback rules

- Host state callbacks may be synchronous and may be called while processing is stopped or active. The adapter must follow the actual format/host contract and use the immutable state snapshot protocol; it must not wait for audio quiescence or perform disk I/O in a realtime callback.
- Start/stop, suspend/resume, sample-rate change, buffer-size change, bus-arrangement change, state restore and device loss each advance or validate a lifecycle generation. A stale prepared graph/configuration cannot be published after a newer transition.
- On shutdown, stop accepting new work, preserve ownership of pending commands and recordings, drain/retire resources on non-RT executors, and only then destroy the shared core. The host callback must not synchronously join a worker that may wait on audio.

## 7. Required prototype and validation work

Before choosing the final AU/VST3 component topology:
1. Build a minimal instrument prototype with MIDI input, stereo output and one optional stereo input as a baseline configuration only; do not treat this as the product's maximum input topology.
2. Test the `aumu` candidate in Logic Pro and at least one additional target AU host: prove a full-band input bus reaches the layer source path while MIDI instrument behavior and five-output semantics remain intact.
3. If `aumu` fails the live-input gate, test `aufm` in the same hosts for full-band audio input plus Note On/Off, MPE, sequencer, five outputs and save/reopen. Compare the actual insertion workflow; do not assume MIDI CC support proves note-event support.
4. Consider dual AU components only if both single-component candidates fail to cover distinct required workflows and the dual arrangement passes state/UX/certification review.
4. Test VST3 input plus five stereo outputs (master + four layers) in each proposed host.
5. Test bus activation/deactivation and channel arrangements after session save/reopen.
6. Test host state save/restore with a deliberately sized embedded recording and external sample reference.
7. Test parameter automation and MPE delivery.
8. Record host/version/OS, component identifiers, bus arrangement and observed limitations.

This prototype is an architecture validation spike. It is not production DSP implementation. It requires explicit owner authorization before code is written.

## 8. Decisions still open

| Item | Recommendation | Status |
|---|---|---|
| R1 formats | AU + VST3 + Standalone | Approved by DEC-026 |
| VST3 live-input bus | Optional stereo input on instrument component | Proposed; validate |
| VST3 instrument identity | Instrument class subcategory; one 16-channel event bus; no automatic per-layer Units | Proposed; host discovery, routing and MPE validation required |
| VST3 outputs | Stereo master + four stereo layer outputs | Required product output; host exposure validate |
| AU component | `aumu` first validation candidate; `aufm` fallback; dual only on demonstrated distinct workflow need | Open — no subtype selected |
| AU live input | Must satisfy DEC-020 through a tested host workflow | Open |
| Host matrix | Logic, Live, Bitwig, Reaper | Proposed Q-P09; exact versions open |
| MIDI/transport behavior | Adapter-specific mapping to shared transport | Open details |
| Offline rendering equivalence | Define exact deterministic boundary | Open |

**Status:** This proposal translates the approved format and live-input requirements into testable architecture options. The evidence-based decision gate is in `PHASE3_AU_COMPONENT_STRATEGY_AUDIT.md`. It does not select final AU component types or claim host certification.


## 9. Recommended output and live-input routing semantics

The product output model remains one stereo master plus four optional stereo layer outputs. Recommended semantic tap positions:

- Each Layer 1–4 output is post-layer-FX and pre-layer-fader/mute/solo. This provides stable stems for DAW mixing and aligns the layer output with its recording tap.
- The stereo master output is the sum of the four layer signals after layer gain, mute and solo logic, plus the required parallel dry live-input path (GS-ARC-007).
- Master recording captures the same stereo master signal immediately before it is handed to the host/device output.
- Each layer's live input feeds that layer before its FX chain; mono input is centered across the stereo layer path (DEC-040).
- A parallel dry live-input path is also summed before the master, as required by GS-ARC-007. This is intentional product routing, not an optional bypass. Master recording includes the dry path; layer stem outputs remain post-FX/pre-layer-fader. Host adapters must prevent accidental duplicate summing of layer buses.
- When a host does not expose all optional layer buses, the main stereo master remains usable. The adapter must not sum layer buses into the master a second time.

These are recommended signal semantics, not a decision to select AU instrument or music-effect component types, nor proof that every host exposes all five outputs. AU/VST3 validation must confirm whether the semantic tap model can be preserved in each target host.

## 10. Format-specific callback and bus constraints — official API cross-check

This section records constraints visible in the Apple Audio Unit and Steinberg VST3 public API documentation as of 2026-11-27. It refines the normalized adapter contract; it does not select AUv2/AUv3, an AU component subtype, a VST3 bus topology beyond the approved product intent, or a framework.

### 10.1 Audio Unit

- AUAudioUnit exposes separate inputBusses and outputBusses; an AUv3 implementation supplies its internalRenderBlock. The host should fetch/cache the AU render block before starting realtime rendering.
- The host sets maximumFramesToRender before allocateRenderResources(). Apple documents that this value cannot change while render resources are allocated. The adapter must size/preallocate callback resources for the negotiated maximum and handle each actual render frame count within that capacity. A maximum-frame or format change is a lifecycle reconfiguration, not an in-render resize.
- scheduleParameterBlock is documented as safe to call from any thread context, including realtime render threads; scheduled events are delivered to the AU internalRenderBlock. Preserve the event's host timing when translating it. This API guarantee does not authorize unbounded queues, allocation or blocking in the shared core.
- If an AURenderPullInputBlock returns an error, Apple says the input data must be assumed invalid. Normalize this as unavailable/invalid input for the current call; never read the supplied buffer.
- The AU API exposes host musical-context and transport-state callbacks and an MPE capability flag. These API surfaces do not prove that every host supplies valid context or supports the same MIDI/live-input workflow; missing/invalid host information remains explicit.
- AUAudioUnit can host AUv2 or AUv3 components and may be instantiated in-process or in a separate extension process. The shared-core contract must not depend on sharing raw pointers across the host/extension boundary.
- The existence of generic AU input/output bus APIs does not establish that an aumu component supports the required live-input workflow in every target host. Q-P06 remains open pending host evidence.

### 10.2 VST3

- ProcessSetup supplies sample rate and maxSamplesPerBlock; ProcessData supplies the actual numSamples, process mode, input/output bus counts and arrays, parameter changes, events and optional process context. For normal audio processing, the actual block may vary from call to call but must remain within the configured maximum.
- Steinberg documents that sample rate and maximum block size cannot change while the audio component is active. A change requires a controlled stop/deactivation, a new setupProcessing(), then reactivation/restart. The adapter must use the lifecycle-generation protocol; it must not resize render resources inside process().
- The host may call IAudioProcessor::process() without audio buffers to flush parameter changes; Steinberg describes this as numInputs = 0 and numOutputs = 0. Treat it as a parameter/process flush, not as an audio render, and do not dereference input/output arrays.
- Bus counts in ProcessData are authoritative for the arrays present in that call. Steinberg's AudioBusBuffers documentation requires channel-buffer arrays for bus entries, while inactive bus sample pointers may be null. The processing FAQ also permits trailing inactive buses to be omitted; therefore never index a declared bus unless its index is within the current numInputs/numOutputs. For a present bus entry, validate numChannels against the negotiated arrangement. A silence flag is not permission to dereference an invalid/null buffer.
- kDefaultActive is a request, not a guarantee: the host may activate a smaller subset of buses. The approved product intent remains one primary stereo master plus four optional layer outputs. Keep the master as the primary/main output; layer stems are optional auxiliary outputs and host exposure/activation remains a Q-P09 validation item.
- ProcessData::processContext is optional. For VST3 3.7 and later, IProcessContextRequirements lets the processor request the context fields it needs; Steinberg warns that without this interface the host may provide no process-context information. Even when requested, validate context flags and never fabricate tempo/PPQ when the host does not provide valid fields.
- Parameter-change queues carry sample-offset information. Preserve host-provided offsets and source order; this does not resolve the open AR-01 precedence between host automation, MIDI, sequencer locks and UI-originated writes.
- Steinberg documents process() and setProcessing() as possible audio-thread calls; avoid allocation in both. In realtime use, state callbacks normally occur on the UI thread, but in offline processing getState()/setState() may occur on the same thread as process(). The state adapter must not assume a single callback thread identity or expose mutable runtime state unsafely.

### 10.3 Required normalized bus/process representation

The shared core must not receive one ambiguous active boolean derived from pointer presence. The adapter's per-call view must distinguish, at minimum:

- declared bus identity and stable adapter mapping;
- whether a bus entry is present in the current callback;
- host activation/connection state when the format exposes it;
- negotiated channel arrangement and channel count;
- whether each sample-buffer address is valid for this call;
- process kind: audio render versus parameter flush/control-only call;
- actual frame count and negotiated maximum.

The exact C++ representation remains open. This refinement is necessary to preserve VST3's omitted-trailing-bus and null-inactive-buffer cases without weakening AU's invalid-input handling.

### 10.4 Validation consequences

The host matrix must explicitly cover: VST3 parameter flush with no audio buses; an inactive middle bus with a later active bus; trailing inactive buses omitted from ProcessData; kDefaultActive not honored; block-size/sample-rate reconfiguration while inactive; invalid AU pull-input result; AU maximum-frame reconfiguration; missing AU/VST3 transport context; and offline VST3 state callbacks. These are host-adapter contract tests, not evidence that any target host has passed them.

**Sources:** Apple AUAudioUnit, maximumFramesToRender, renderBlock, scheduleParameterBlock, AURenderPullInputBlock; Steinberg VST3 ProcessData, AudioBusBuffers, IAudioProcessor, IProcessContextRequirements, and Processing FAQ. Exact URLs are listed in PHASE3_PLATFORM_AND_PLUGIN_RESEARCH.md §8.

### 10.5 VST3 instrument component and host integration

The VST3 instrument-classification, event/audio-bus and MPE evidence is consolidated in `PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md`. Key constraints:

- VST3 uses `IComponent` + `IAudioProcessor` for the processing component and a separate `IEditController`; the processor must work when the controller is not instantiated.
- Instrument identity is class subcategory metadata. `AudioEffect` is an SDK bus/processing helper, not the instrument/effect product category.
- One 16-channel event input bus is the candidate baseline. Internal routing must preserve DEC-034 so every layer assigned to the same channel receives the event.
- The live-audio bus must deliver raw full-band samples. Its `kMain` versus `kAux` bus type remains open pending host tests.
- One main stereo master plus four auxiliary stereo layer outputs remains the required semantic model. `kDefaultActive` is not a guarantee; inactive outputs must not remove or double-sum a layer in the master.
- MPE per-note pitch must use note identity and VST3 Note Expression; ordinary channel pitch bend is not sufficient evidence for DEC-038's per-note ±48-semitone behavior.
- Four product layers do not automatically require four VST3 Units. Do not publish one-to-one routing metadata when a MIDI channel can trigger multiple layers.
- Do not claim simple mode or Fx|Instrument dual categorization without target-host evidence.

This is a documentation-level adapter refinement only; no VST3 category, bus type, Unit model or host compatibility cell is approved.

## 11. Render capacity, bus fan-out and transient memory

The host maximum block size determines the fixed render-resource capacity for each lifecycle generation:

| Format | Capacity source | Required lifecycle |
|---|---|---|
| AU | `maximumFramesToRender` | Set before `allocateRenderResources`; a changed maximum requires the AU render-resource lifecycle, not an in-render resize. |
| VST3 | `ProcessSetup::maxSamplesPerBlock` | Change only while inactive; deactivate, call `setupProcessing`, then reactivate/restart under a new lifecycle generation. |
| Standalone | Configured audio-device maximum | Prepare a new device/render configuration off callback and publish only after the old configuration is quiescent. |

Let `B` be the prepared maximum frames and `N` the actual render frames. Require `N <= B` before any scratch access. Scratch capacity is prepared off-thread and never grows/shrinks in `process` or AU render. A call beyond capacity enters a bounded adapter failure path; exact SDK status mapping and host recovery require validation. A VST3 zero-bus parameter-flush call is not an audio render and must not touch audio scratch or advance transport.

The product output model has five stereo buses when all are exposed: one primary master plus four optional layer stems. This is ten output channels, but the host-owned buffers are borrowed views and are not automatically ten channels of engine-owned scratch. If an implementation materializes copies of all five outputs, the float32 payload is `5 × 2 × B × 4 = 40B` bytes. One additional stereo input staging copy adds `8B` bytes. More generally:

`M_transient = B × 4 × (C_in_staged + C_out_staged)`

where channel counts include only data actually copied into engine-owned staging/working buffers. Internal per-layer work planes, FX workspaces and guard/alignment memory must be itemized separately. Direct writes to valid host output buffers and read-only fan-out of one input bus do not incur a second full-bus copy.

The R1 product policy does not set a fixed maximum input channel/source count. A one-stereo-input test is a baseline, not a limit. The adapter must distinguish physical host input buses from logical per-layer source views: the same read-only physical input may feed multiple layers, and each layer's source routing is independent. Do not imply four separate host input buses or four duplicate input buffers merely because the engine has four layer source views.

All byte-size calculations use checked wide arithmetic and validate conversion to `size_t` before allocation. Float32 sample width is distinct from process architecture width. The current macOS target investigation concerns x86_64/arm64; Q-T03 remains open and no 32-bit macOS target is approved or implied.

The detailed equations, illustrative 1,024/4,096-frame calculations, admission ledger and validation obligations are in `PHASE3_MEMORY_ADMISSION_AND_RENDER_CAPACITY_AUDIT.md`. These examples do not approve a block-size range, host topology, memory budget or implementation.


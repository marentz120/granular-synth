# Phase 3 Platform and Plugin Research

**Research date:** 2026-11-27; AU component strategy update: 2026-11-28  
**Status:** Evidence note. This document does not approve a framework, deployment target, plugin component set or host compatibility claim.

## 1. JUCE 8 licensing — current public terms

JUCE's current public pricing page lists:

| Tier | Annual revenue/funding limit | Public price |
|---|---:|---:|
| Starter | Up to $20,000 | Free |
| Indie | Up to $300,000 | $40 per user/month or $800 perpetual |
| Pro | No limit | $175 per user/month or $3,500 perpetual |

The JUCE 8 EULA explains that the revenue/funding calculation uses the licensee's preceding 12 months and has entity/affiliate rules. The displayed tier table is not a substitute for reviewing the full EULA and determining which legal entity and revenue streams apply.

Sources:
- JUCE pricing: https://juce.com/get-juce/
- JUCE 8 EULA: https://juce.com/legal/juce-8-licence/

**Architecture implication:** JUCE remains a credible candidate for AU, VST3 and Standalone, but commercial viability cannot be concluded until the intended product/distribution model (Q-P03), licensee identity and relevant revenue/funding are established and the EULA is reviewed (Q-L04). Do not lock the framework based only on the free Starter tier.

## 2. JUCE build and deployment claims

Do not use a generic repository README or an old JUCE release's deployment minimum as the R1 support commitment. The exact JUCE 8 tag, Xcode version, macOS SDK, deployment target and required APIs must be pinned together in the build decision record. The framework's nominal minimum does not prove that the selected host matrix or all dependencies support that OS.

**Architecture implication:** the proposed macOS 12+ Universal 2 target remains a candidate, not an approved minimum. Confirm it against the pinned JUCE 8 release, current Xcode/SDK, required Apple APIs, host versions and an actual Intel/Apple Silicon test matrix. Universal 2 is retained as an R1 requirement unless the owner changes scope.

## 3. Current VST3 SDK evidence

The current Steinberg VST3 SDK documentation identifies the 3.8.x line and lists macOS 10.14–26 with x86, x86_64 and Apple Silicon architectures. The VST3 model supports multiple dynamic audio I/O buses, but hosts may activate/deactivate buses and may pass null buffers for inactive buses; processing must respect the bus index and negotiated channel arrangements. Steinberg's multi-timbral guidance also makes clear that multi-output routing and track-to-output association require deliberate implementation rather than simply declaring extra buses.

Sources:
- VST3 SDK and system requirements: https://github.com/steinbergmedia/vst3sdk
- Multiple dynamic I/O: https://github.com/steinbergmedia/vst3_dev_portal/blob/main/src/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.md
- Complex/multi-timbral structures: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Complex%2BStructures/Index.html
- VST3 process and buffer behavior: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html

**Architecture implication:** the proposed stereo master plus four stereo layer outputs is compatible with the VST3 bus model, but cannot be assumed to work uniformly across hosts. The adapter must correctly handle inactive buses, variable block sizes, parameter-flush process calls and input/output buffer aliasing. Host validation remains mandatory.

## 4. Apple Audio Unit component model

Apple's Audio Unit documentation identifies four component types:
- `aufx`: effect
- `augn`: generator
- `aumu`: instrument
- `aufm`: music effect

Apple's archived extension guide describes an instrument as an audio-creation unit that responds to MIDI, and a music effect as an audio-modification unit that responds to MIDI. Current Apple documentation directs developers to Audio Toolbox's `AUAudioUnit` APIs for AUv3 implementation.

Sources:
- Apple Audio Unit types (archived guide): https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/AudioUnit.html
- Apple AUAudioUnit: https://developer.apple.com/documentation/audiotoolbox/auaudiounit
- Apple Audio Components: https://developer.apple.com/documentation/audiotoolbox/audio-components

**Architecture implication:** the proposal to expose an `aumu` instrument and a separate `aufm` music-effect component for live audio input is technically plausible as a design option, but it is not automatically the correct product choice. It can create two insert identities and different host workflows. Validate whether the required live-input workflow can be supported in the selected AU implementation and target hosts before deciding to ship two AU components. Do not describe the legacy Audio Unit framework as the current implementation API.

## 5. VST3 bus model

Steinberg's current VST3 SDK documentation describes dynamic multiple audio I/O buses, including input and output buses, and a separation between the processing component and edit controller. The SDK's AudioEffect API provides methods to add audio input/output buses and event input/output buses.

Sources:
- VST3 SDK overview: https://github.com/steinbergmedia/vst3sdk
- VST3 AudioEffect reference: https://steinbergmedia.github.io/vst3_doc/vstsdk/classSteinberg_1_1Vst_1_1AudioEffect.html
- VST3 multiple dynamic I/O: https://github.com/steinbergmedia/vst3_dev_portal/blob/main/src/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.md

**Architecture implication:** a VST3 instrument with a stereo input bus and multiple stereo outputs is supported by the API model. This does not establish that every host exposes all buses in the same way. The product must define required/default bus activation and fallback behavior, then test each supported host/format combination.

## 6. Evidence-based candidate assessment

| Candidate | Evidence-based assessment | Open gate |
|---|---|---|
| C++20 + JUCE 8 + framework-free DSP core | Strong candidate: one framework can provide Standalone, AU and VST3 shells; core remains portable and independently testable. | Q-T01/Q-T02/Q-L04/Q-P03 |
| C++ + iPlug2 | Still a valid alternative; this research pass did not verify its current release, licensing details or host-specific live-input behavior. | Requires a like-for-like build and licence review |
| Raw AU/VST3 SDKs | Maximum control, but substantially more integration and maintenance work across two plugin APIs and Standalone. | Requires a quantified implementation/maintenance comparison |
| Rust/nih-plug | Does not directly satisfy the R1 AU requirement according to the existing investigation; wrapper route introduces another dependency. | Not preferred for R1 without new evidence |

**Recommendation for the next architecture iteration:** retain C++20 + JUCE 8 as the leading candidate, keep the DSP/state core framework-independent, and make the framework decision conditional on (a) a reviewed licence fit, (b) successful minimal host/component prototypes, and (c) a reproducible build/test workflow. This is a recommendation, not an approved decision.

## 7. Evidence gaps that remain

- No current Logic Pro, Ableton Live, Bitwig or Reaper test has been run for the proposed AU/VST3 input/output topology.
- No prototype has verified the AU live-input path, component identity, MIDI behavior or automation/state behavior.
- No machine/OS reference configuration has been fixed for CPU or memory measurements.
- No build spike has verified Universal 2, plugin validation or host scan/load behavior.
- No legal review has established whether JUCE Starter/Indie/Pro terms fit the intended product entity and commercial plan.

**Conclusion:** current public documentation supports JUCE as a serious candidate and confirms the relevant AU/VST3 extension points. It does not remove the need for licensing review and host validation. No framework or component architecture is approved by this note.

## 8. Official AU/VST3 process, bus and callback constraints — 2026-11-27

This pass cross-checked the proposed normalized host contract against Apple's current Audio Toolbox documentation and Steinberg's VST3 3.8 API/developer documentation. The findings refine adapter requirements; they do not establish that a target DAW supports the proposed topology.

### 8.1 Apple Audio Unit

- AUAudioUnit exposes inputBusses and outputBusses, an internalRenderBlock, render-resource allocation/deallocation, host transport/musical-context callbacks and an MPE capability property.
- The host sets maximumFramesToRender before allocating render resources. Apple explicitly says it cannot be changed while render resources are allocated. Buffer capacity must therefore be prepared before rendering; changes require a lifecycle transition.
- The host should fetch/cache renderBlock before realtime rendering.
- scheduleParameterBlock is documented as safe from any thread context, including realtime render threads, and schedules events for delivery to the internal render block. The adapter must still preserve bounded work and the engine's event ordering contract.
- An AURenderPullInputBlock error means the input data should be treated as invalid. This supports an explicit per-call unavailable-input state.
- The generic AU API can bridge AUv2 and AUv3 components and can instantiate an AUv3 component in-process or in a separate extension process. The shared core must not assume pointer-sharing across that boundary.
- These APIs do not prove that an aumu instrument receives live audio input in every host or that all optional output buses are exposed. Q-P06/Q-P09 remain open.

**Official Apple sources:**
- AUAudioUnit: https://developer.apple.com/documentation/audiotoolbox/auaudiounit
- maximumFramesToRender: https://developer.apple.com/documentation/audiotoolbox/auaudiounit/maximumframestorender
- renderBlock: https://developer.apple.com/documentation/audiotoolbox/auaudiounit/renderblock
- scheduleParameterBlock: https://developer.apple.com/documentation/audiotoolbox/auaudiounit/scheduleparameterblock
- AURenderPullInputBlock: https://developer.apple.com/documentation/audiotoolbox/aurenderpullinputblock

### 8.2 Steinberg VST3

- ProcessSetup establishes sample rate and maximum samples per block; ProcessData supplies the actual block size, process mode, bus counts/arrays, parameter changes, events and optional process context. Actual audio block sizes may vary up to the configured maximum.
- Steinberg's Processing FAQ states that sample rate and maximum block size cannot change while the component is active. A change requires stopping processing and deactivating before setupProcessing(), then reactivating/restarting.
- The host may call IAudioProcessor::process() without audio buffers to flush parameters; the FAQ describes this as numInputs = 0 and numOutputs = 0. This must be normalized as a parameter-flush call, not mistaken for an audio render.
- ProcessData bus arrays are per-call. AudioBusBuffers requires channel-buffer arrays for entries that are present, but inactive bus sample pointers may be null. Steinberg's FAQ also states that trailing inactive buses may be omitted; the adapter must check numInputs/numOutputs before indexing and must not equate declared bus count with current array length.
- kDefaultActive is a request rather than a guarantee. The host may activate only a subset of the declared buses. This reinforces the approved fallback: the primary stereo master remains usable even when optional layer outputs are not activated.
- ProcessData::processContext is optional. VST3 3.7+ provides IProcessContextRequirements to request required context flags; without it, Steinberg warns the host may provide no process context. Returned validity flags must still be checked.
- VST3 sample-accurate parameter queues provide event sample offsets; preserve these offsets and source order. The cross-source precedence remains AR-01.
- Steinberg documents process() and setProcessing() as possible audio-thread calls. In realtime contexts state callbacks normally occur on the UI thread, but offline getState()/setState() may occur on the same thread as process().

**Official Steinberg sources:**
- ProcessData: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1ProcessData.html
- AudioBusBuffers: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1AudioBusBuffers.html
- IAudioProcessor: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IAudioProcessor.html
- Processing FAQ: https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Processing.html
- Multiple Dynamic I/O: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.html
- Parameter automation: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Parameters%2BAutomation/Index.html
- IProcessContextRequirements: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.7.0/IProcessContextRequirements.html

### 8.3 Architecture consequences and remaining evidence gaps

1. The normalized process model must distinguish declared bus identity, per-call array presence, host activation/connection, channel arrangement and sample-pointer validity. One nullable pointer or one active flag is insufficient.
2. Process kind must distinguish an audio render from a VST3 parameter-flush call with no audio buses.
3. AU maximum render frames and VST3 maximum block size/sample rate are lifecycle configuration values, not mutable render-time capacities.
4. AU parameter scheduling can enter from any thread; VST3 process/setProcessing may be audio-thread calls. Adapter queue topology must be explicit before selecting SPSC/MPSC/mailbox primitives.
5. VST3 process context is optional and must be requested/validated; AU host context callbacks may also be absent or invalid.
6. No host prototype was run. The behavior of Logic Pro, Ableton Live, Bitwig and Reaper for the proposed live-input and five-output layout remains unverified. Q-P06/Q-P09 are still open.

No framework, component subtype, bus activation default, deployment target or host compatibility claim is approved by this research update. No code, prototype or benchmark was performed.

## 9. Target-host capability evidence cross-check — 2026-11-28

The R1 host candidates in Q-P09 were checked against current official documentation from Apple/Logic Pro, Ableton, Bitwig and Cockos/REAPER. The detailed host × format matrix, evidence vocabulary, validation backlog and source register are in `PHASE3_HOST_CAPABILITY_EVIDENCE_MATRIX.md`.

Findings that materially refine the prior host assumptions:

- **Logic Pro:** official documentation confirms multi-output Audio Unit instruments and aux-channel-strip routing. Its documented side-chain source menu can select audio tracks, software-instrument tracks, hardware inputs and buses, but Apple states that the side-chain signal controls the plug-in and is not processed by it. This is not proof of a general `aumu` live-audio source input.
- **Ableton Live:** official documentation confirms up to 16 stereo outputs from one multi-out plug-in and external side-chain routing for third-party AU/VST plug-ins that expose side-chain functionality. The side-chain path is not proof of the full granular source-input requirement.
- **Bitwig Studio:** official documentation confirms multi-out VST chains and access to their outputs. Bitwig explicitly states that it does not support Audio Units, so the Bitwig AU cell is N/A while VST3 remains relevant. Its Audio Sidechain/modulator and plug-in side-chain support do not prove a raw main audio input bus to the granular engine.
- **REAPER:** official documentation lists VST3 and AU on macOS, multichannel tracks up to 128 individually routable channels, pin connectors, and a multi-output VSTi example with four stereo outputs. This is strong routing evidence, but the exact five-stereo-output topology and live-input delivery to an instrument remain untested.

Across all four hosts, public manuals do not prove that the exact optional-bus activation, per-layer source mapping and session save/reopen behavior required by GRANULAR SYNTH works. Nor do they establish the exact maximum-block reconfiguration sequence that the plug-in will observe. These remain prototype evidence items, not assumptions.

**No scope change:** DEC-020 (live input in Standalone and plug-in contexts), DEC-026 (AU + VST3 R1), and GS-ARC-006 (stereo master + four stereo layer outputs) remain unchanged. No framework, AU component subtype, bus default, host version, compatibility claim or prototype was approved. No code or benchmark was run.


## 10. AU component strategy audit — 2026-11-28

The detailed comparison and decision gate are maintained in `PHASE3_AU_COMPONENT_STRATEGY_AUDIT.md`. This section records the evidence that changes the architecture recommendation.

### 10.1 Component semantics

Apple's current Audio Toolbox documentation defines:
- `kAudioUnitType_MusicDevice` (`aumu`): an instrument such as a sampler or synthesizer that responds to MIDI and creates notes/audio.
- `kAudioUnitType_MusicEffect` (`aufm`): an effect unit that can respond to MIDI control messages, typically mapping MIDI messages to DSP parameters.

Apple's archived Audio Unit guide describes the same role distinction: `aumu` is an audio-creation instrument; `aufm` modifies an audio stream while responding to MIDI. The modern `AUAudioUnit` API exposes generic input/output bus arrays, MIDI scheduling, host context and MPE capability, but that generic surface does not establish host support for an input bus on a particular `aumu` component.

Official sources checked 2026-11-28:
- https://developer.apple.com/documentation/audiotoolbox/kaudiounittype_musicdevice
- https://developer.apple.com/documentation/audiotoolbox/kaudiounittype_musiceffect
- https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/AudioUnit.html
- https://developer.apple.com/documentation/audiotoolbox/auaudiounit
- https://developer.apple.com/documentation/avfaudio/avaudiounitmidiinstrument
- https://developer.apple.com/documentation/avfaudio/avaudiouniteffect/init%28audiocomponentdescription%3A%29

### 10.2 Revised recommendation

Keep `aumu` as the first validation candidate because R1 is a MIDI-playable instrument, but do not treat the subtype as selected. It passes only if the target host exposes a full-band audio input bus to the Granular/Sampler source path while preserving note/MPE behavior, the master plus four layer outputs, automation and state persistence.

If `aumu` fails that gate, test `aufm` against the entire R1 instrument workflow, not just MIDI CC. A music-effect component is a semantically natural live-audio processor, but note-triggered voice behavior, MPE, sequencer events and host insertion UX remain unproven.

Do not ship both components solely because both types exist. A dual component package is justified only if tests demonstrate two distinct required workflows that cannot be served by one component, and if each component passes its own host/state/automation certification. Sharing the core or state schema does not imply host-level component substitution.

### 10.3 Scope and evidence boundary

- Q-P06's product requirement is answered by DEC-020; the AU subtype/topology implementation remains open.
- AU component type and AUv2/AUv3 generation are separate decisions.
- No current evidence certifies `aumu` or `aufm` live-input routing in Logic, REAPER or Ableton for GRANULAR SYNTH.
- No AU host prototype, code, plugin shell or benchmark was run in this research pass.
- VST3's single instrument-component candidate is unchanged; this audit does not infer AU type semantics for VST3.


## 11. VST3 instrument component audit — 2026-11-28

The detailed decision gate is maintained in `PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md`. This research pass corrects an important terminology risk: VST3 does not use a distinct processor interface solely for instruments. The processor implements `IComponent` and `IAudioProcessor`; the edit controller is a separate `IEditController`. Instrument/effect identity is communicated through class subcategories. The SDK's `AudioEffect` helper can declare audio and event buses and should not be confused with the plug-in's host-visible category.

Evidence checked:
- The VST3 API documentation describes the processor/controller separation and notes that a host may process without instantiating the controller: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html
- The AudioEffect SDK helper exposes audio input/output and event input/output bus declaration methods: https://steinbergmedia.github.io/vst3_doc/vstsdk/classSteinberg_1_1Vst_1_1AudioEffect.html
- Instrument subcategories include `Instrument|Synth|Sampler`: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/group__plugType.html
- Dynamic I/O supports multiple audio/event buses, but the host controls bus activation and may not honor the default-active request: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.html
- Complex instrument structures document simple mode, Units and host routing associations: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Complex%2BStructures/Index.html
- Steinberg's FAQ documents the `Fx|Instrument` subcategory and a historical instrument-mode audio-input caveat for Steinberg sequencers up to 5.5: https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Miscellaneous.html
- VST3 MIDI documentation maps MPE to Note Expression, not ordinary channel pitch bend: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/About%2BMIDI/Index.html
- `INoteExpressionController` and `NoteExpressionValueEvent` define note-scoped expression and note-ID association: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.5.0/INoteExpressionController.html and https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1NoteExpressionValueEvent.html

Architecture implications:
1. Keep one instrument-classified VST3 component as the first validation candidate; `Instrument|Synth|Sampler` is a candidate category, not a selected value.
2. Use one 16-channel event input bus as the baseline and route events internally to layers. DEC-034 does not require four event buses: multiple layers on the same channel must all receive the matching event.
3. Declare one full-band audio input for the first validation configuration. The `kMain` versus `kAux` bus type remains open until raw audio is proven to reach the Granular/Sampler source path in each target host.
4. Preserve one main stereo master output plus four auxiliary stereo layer outputs. Default activation is a request; the host may activate only a subset.
5. Do not claim VST3 simple mode: the current product topology includes multiple output buses, multiple MIDI channels and MPE. Do not claim four VST3 Units; product layers are internal engine layers unless a concrete host workflow requires host-visible units.
6. DEC-038's per-note ±48-semitone pitch behavior requires note identity and Note Expression handling. Channel pitch bend alone does not prove per-note behavior. Wrapper MPE translation interfaces are not the same as the direct VST3 host event contract.

No VST3 host prototype was run. Logic Pro, Ableton Live, Bitwig and REAPER still require exact-version tests for instrument discovery, full-band input routing, MPE Note Expression, five outputs, bus activation and save/reopen. No framework, category, bus type, simple-mode flag or host compatibility claim is approved.

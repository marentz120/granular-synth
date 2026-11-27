# Phase 3 Platform and Plugin Research

**Research date:** 2026-11-27 (official AU/VST3 callback cross-check added 2026-11-27)  
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

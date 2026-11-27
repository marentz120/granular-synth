# Phase 3 Platform and Plugin Research

**Research date:** 2026-11-27  
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

The current JUCE GitHub README advertises macOS deployment targets as low as macOS 10.11 for x86_64 and arm64, and lists Xcode 12.4 / macOS 11 Apple Silicon as build requirements. These are framework-level minima, not a recommendation for GRANULAR SYNTH and not proof that every required API, host or dependency supports those targets.

Source: https://github.com/juce-framework/JUCE

**Architecture implication:** the repository's proposed macOS 12+ Universal 2 target is a product/platform decision to validate, not a technical minimum forced by JUCE. The actual minimum should be selected from the required APIs, host matrix, signing/toolchain constraints and available test machines. Intel support must be an explicit cost/benefit decision because Universal 2 is an R1 requirement in GS-PLT-002 unless the owner changes scope.

## 3. Apple Audio Unit component model

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

## 4. VST3 bus model

Steinberg's current VST3 SDK documentation describes dynamic multiple audio I/O buses, including input and output buses, and a separation between the processing component and edit controller. The SDK's AudioEffect API provides methods to add audio input/output buses and event input/output buses.

Sources:
- VST3 SDK overview: https://github.com/steinbergmedia/vst3sdk
- VST3 AudioEffect reference: https://steinbergmedia.github.io/vst3_doc/vstsdk/classSteinberg_1_1Vst_1_1AudioEffect.html
- VST3 multiple dynamic I/O: https://github.com/steinbergmedia/vst3_dev_portal/blob/main/src/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.md

**Architecture implication:** a VST3 instrument with a stereo input bus and multiple stereo outputs is supported by the API model. This does not establish that every host exposes all buses in the same way. The product must define required/default bus activation and fallback behavior, then test each supported host/format combination.

## 5. Evidence-based candidate assessment

| Candidate | Evidence-based assessment | Open gate |
|---|---|---|
| C++20 + JUCE 8 + framework-free DSP core | Strong candidate: one framework can provide Standalone, AU and VST3 shells; core remains portable and independently testable. | Q-T01/Q-T02/Q-L04/Q-P03 |
| C++ + iPlug2 | Still a valid alternative; this research pass did not verify its current release, licensing details or host-specific live-input behavior. | Requires a like-for-like build and licence review |
| Raw AU/VST3 SDKs | Maximum control, but substantially more integration and maintenance work across two plugin APIs and Standalone. | Requires a quantified implementation/maintenance comparison |
| Rust/nih-plug | Does not directly satisfy the R1 AU requirement according to the existing investigation; wrapper route introduces another dependency. | Not preferred for R1 without new evidence |

**Recommendation for the next architecture iteration:** retain C++20 + JUCE 8 as the leading candidate, keep the DSP/state core framework-independent, and make the framework decision conditional on (a) a reviewed licence fit, (b) successful minimal host/component prototypes, and (c) a reproducible build/test workflow. This is a recommendation, not an approved decision.

## 6. Evidence gaps that remain

- No current Logic Pro, Ableton Live, Bitwig or Reaper test has been run for the proposed AU/VST3 input/output topology.
- No prototype has verified the AU live-input path, component identity, MIDI behavior or automation/state behavior.
- No machine/OS reference configuration has been fixed for CPU or memory measurements.
- No build spike has verified Universal 2, plugin validation or host scan/load behavior.
- No legal review has established whether JUCE Starter/Indie/Pro terms fit the intended product entity and commercial plan.

**Conclusion:** current public documentation supports JUCE as a serious candidate and confirms the relevant AU/VST3 extension points. It does not remove the need for licensing review and host validation. No framework or component architecture is approved by this note.

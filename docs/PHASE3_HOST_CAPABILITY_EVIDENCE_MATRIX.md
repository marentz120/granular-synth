# Phase 3 Host Capability Evidence Matrix

**Version:** 0.1 — public host-documentation cross-check, 2026-11-28  
**Status:** Evidence inventory only. No host has been certified for GRANULAR SYNTH. No plugin prototype, SDK implementation, or compatibility promise is approved.  
**Authority:** Q-P09 proposed R1 host list; DEC-020 (live input), DEC-026 (AU + VST3 R1 formats), GS-ARC-006 (stereo master plus four stereo layer outputs), GS-SMG-012/020 (live input), and the candidate topology in `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md`.  
**Research boundary:** Public manufacturer documentation was reviewed on 2026-11-28. Exact supported host versions remain open. Host feature documentation is not evidence that this specific plug-in topology works.

## 1. Evidence vocabulary

| Label | Meaning |
|---|---|
| **D — documented** | Official host documentation explicitly describes the host-level capability. |
| **P — partial** | Official documentation describes a related capability, but not the full GRANULAR SYNTH requirement. |
| **U — unverified** | No sufficient official evidence was found for the required behavior; a minimal host test is needed. |
| **N/A — not supported** | The host vendor explicitly says the format is not supported. |
| **API only** | The plug-in format API supports the concept, but this does not prove host implementation or user-visible routing. |

A **D** entry never means GRANULAR SYNTH itself has passed a host test. All final support claims require a recorded prototype result for a pinned host version, OS, architecture, plug-in format and bus arrangement.

## 2. R1 requirements under examination

The candidate plug-in topology is:
- one MIDI-capable instrument instance;
- one stereo master output;
- four independent stereo layer outputs (five stereo outputs total);
- a minimum validation case of one optional stereo live-input bus;
- routing of an available physical input source to any layer, including fan-out of one read-only source to multiple layers;
- no requirement that there be one physical input bus per layer;
- master mix remains complete if optional layer outputs are inactive;
- each layer's contribution must appear only once in the master mix.

The one-stereo-input case is a **minimum test fixture**, not a product-wide input limit. DEC-020 imposes no fixed product-level cap on live input sources/channels. Host/device constraints and the actual topology must be reported rather than hidden.

## 3. Host × format evidence matrix

| Host | AU | VST3 | Officially documented multi-output behavior | Live-input evidence relevant to this product | Current evidence status |
|---|---|---|---|---|---|
| **Logic Pro** | **D** — AU instruments supported | **N/A** — Logic's documented third-party plug-in path is AU/AUv3; VST3 is not a Logic target | **D** — Logic supports multi-output Audio Unit instruments that expose multi-output configurations; extra stereo pairs are routed through aux channel strips. The guide does not establish a numeric maximum or prove this exact five-stereo-output component. | **P** — Logic can select audio tracks, software-instrument tracks, hardware inputs and buses as side-chain sources. Apple explicitly describes the side-chain signal as controlling the plug-in, not being processed by it. This does **not** prove a general instrument audio-input bus for granular processing. The `aufm` route remains an alternative to validate, not an approved choice. | Outputs: partial-to-strong host evidence; five-pair exact layout U. Full live input: U. Save/reopen bus activation: U. |
| **Ableton Live** | **D** — AU supported on macOS | **D** — VST3 supported on macOS | **D** — Live documents access to up to 16 stereo outputs from one multi-out plug-in. The proposed five stereo outputs are within that documented host limit. | **P** — Live documents external side-chain routing for third-party VST/AU plug-ins that expose side-chain functionality. This is not evidence of a general audio-input bus feeding the instrument's granular source path. | Outputs: documented host capability. Full live input: U. Save/reopen bus activation: U. |
| **Bitwig Studio** | **N/A** — Bitwig explicitly states it does not support Audio Units | **D** — VST plug-ins supported | **D** — Bitwig documents multi-out VST plug-ins, dynamically added output chains, and access to their channels from other tracks or Audio Receiver devices. The guide does not state a numeric output ceiling for this topology. | **P** — Bitwig documents audio side-chain sources from project signals and side-chain support for VST plug-ins equipped for it. The audio-sidechain modulator is a modulation source, not proof of raw audio being delivered to the instrument's main source input. | VST3 outputs: documented host capability; exact five-pair layout U. AU: N/A, not a compatibility defect. Full live input: U. Save/reopen bus activation: U. |
| **REAPER** | **D** — AU supported on macOS | **D** — VST3 supported | **D** — REAPER documents multi-output VSTi routing, a four-stereo-output instrument example, per-plug-in pin connectors, and up to 128 individually routable track channels. This strongly supports host-side multichannel routing, but does not prove this exact five-stereo-output plug-in in both AU and VST3. | **P** — REAPER documents plug-in pin/input-output routing and side-chaining, including side-chain routing for plug-ins without native side-chain support. This supports a route to test, but exact instrument input-bus delivery, AU/VST3 behavior and per-layer source mapping remain unverified. | Outputs: strong host evidence; exact five-pair topology U. Input: partial; exact instrument live-source path U. Save/reopen bus activation: U. |

### Interpretation

1. **Bitwig AU must be marked N/A**, not as a failed GRANULAR SYNTH implementation. Bitwig's official support page explicitly says Audio Units are not supported; Bitwig remains a VST3 host in the R1 matrix.
2. Logic, Live, Bitwig and REAPER each document some form of multi-output plug-in routing. This makes the five-stereo-output design plausible, but only Ableton's published 16-stereo-output figure explicitly exceeds the proposed count. Logic and Bitwig do not publish an exact ceiling in the cited material; REAPER's manual example demonstrates four stereo pairs while its track routing supports many more channels. The exact plug-in declaration must still be tested.
3. **Side-chain routing is not equivalent to a full audio input bus.** A side-chain signal may be exposed only to a detector/modulator path. GRANULAR SYNTH requires live audio to be available as a source for granular processing. Do not count side-chain documentation as proof of that requirement.
4. No reviewed host document proves that optional bus activation, input/output mapping and project routing for this exact plug-in persist correctly after save, close and reopen.
5. No reviewed host document provides a reliable host-specific guarantee for the exact sequence of block-size changes that the plug-in will receive. The AU/VST3 API lifecycle rules remain authoritative; actual host transitions require a prototype test.

## 4. Per-host validation backlog

The following tests are required before any host/format cell can be labelled supported. They are test design only; no test IDs, pass thresholds or results are assigned here.

| Test area | Logic Pro AU | Live AU/VST3 | Bitwig VST3 | REAPER AU/VST3 |
|---|---|---|---|---|
| Scan, instantiate, reopen | Required | Required for both formats | Required | Required for both formats |
| Five stereo outputs | Verify master + four layer pairs; create aux strips; confirm each pair is independent | Verify all five pairs; route each sub-output to separate audio tracks | Verify five chains and route each output to separate tracks | Verify five pairs using plug-in outputs and track routing/pins |
| Master fallback | Deactivate optional layer outputs; confirm master still contains each layer once | Same | Same | Same |
| Live input bus | Distinguish `aumu` instrument input from side-chain and test `aufm` only if separately authorized | Test actual input bus separately from side-chain selector | Test VST3 input bus separately from Audio Sidechain modulator | Test actual plug-in input pins on an instrument track, not merely side-chain routing |
| Per-layer source mapping | Feed one stereo input to each layer in turn; test shared read-only fan-out | Same | Same | Same |
| Mono input | Verify DEC-040 centering to both sides of layer processing | Same | Same | Same |
| Session persistence | Save/reopen with each optional bus active/inactive and routing restored | Same | Same | Same |
| Block/lifecycle change | Record actual maximum render capacity and every observed reconfiguration path; verify no callback resize | Record actual host block behavior and VST3 `setupProcessing` lifecycle | Same | Same |
| State/recordings | Restore embedded unsaved recording and external sample reference | Same | Same | Same |

For every test record: host product and exact version/build, macOS version, CPU architecture, AU/VST3 identifier and version, negotiated bus layout, active buses, channel counts, actual block sizes, source routing, session save/reopen outcome, logs and reproducible project file.

## 5. Required signal-level checks

The host prototype must use distinguishable deterministic signals rather than relying only on meters or UI labels:

- Assign a unique tone or coded test signal to each layer output.
- Verify the master contains the four layer contributions according to the product mixer state and that each auxiliary output contains only its corresponding layer stem.
- Deactivate one or more auxiliary outputs and verify the layer remains audible in the master exactly once.
- Feed a known live input and verify that the granular source path reads it; an audible side-chain detector response alone is not sufficient.
- Route one physical input source to more than one logical layer and verify fan-out does not duplicate or mutate the source buffer.
- Repeat with mono input to verify DEC-040.
- Save the session with a non-default bus activation/routing arrangement, close the host, reopen and verify restoration.
- Exercise stop/deactivate/reconfigure/reactivate transitions at the host's observed block/sample-rate changes. Do not resize or allocate from the render callback.

## 6. Open decisions and non-decisions

- Q-P09 remains open for exact minimum host versions/builds and certification scope.
- Q-P06 remains open for AU component topology (`aumu`, `aufm`, or another validated arrangement) and VST3 component details.
- Q-T01/Q-L04 remain open; no framework or licence choice is made here.
- Q-T03/Q-T17 remain open; this matrix does not set the macOS minimum, architecture policy, sample-rate range or block-size range.
- DEC-020 remains intact: live input is an R1 product capability in Standalone and plug-in contexts.
- DEC-026 remains intact: AU and VST3 are R1 formats. Bitwig's lack of AU support is a host-format applicability limitation, not a reason to remove AU from the product.
- GS-ARC-006 remains intact: stereo master plus four stereo layer outputs is the target. Host fallback must be explicit and must not silently discard the layer's master contribution.
- No AU component subtype, bus activation default, host compatibility claim, product scope change, implementation, prototype or benchmark is approved by this document.

## 7. Official source register

All links below were checked on 2026-11-28. Vendor pages can change; pin exact host versions before release certification.

### Apple / Logic Pro
- Logic Pro User Guide — Use multi-output instruments: https://support.apple.com/en-lamr/guide/logicpro/lgcp8e887f45/mac
- Logic Pro User Guide — Side-chain source routing and signal purpose: https://support.apple.com/et-ee/guide/logicpro/lgcpbc21a1fd/mac
- Apple Audio Toolbox — AUAudioUnit buses and input/output capabilities: https://developer.apple.com/documentation/audiotoolbox/auaudiounit
- Apple Audio Toolbox — maximumFramesToRender lifecycle: https://developer.apple.com/documentation/audiotoolbox/auaudiounit/maximumframestorender

### Ableton Live
- Ableton Help — Using multi-out plug-ins (up to 16 stereo outputs): https://help.ableton.com/hc/en-us/articles/209773065-Using-multi-out-plug-ins
- Ableton Help — Sidechaining a third-party plug-in: https://help.ableton.com/hc/en-us/articles/209775325-Sidechaining-a-third-party-plug-in
- Ableton Help — AU/VST3 plug-in formats on macOS: https://help.ableton.com/hc/en-us/articles/115000349184-VST-AU-plug-in-doesn-t-appear-in-Live-s-Browser

### Bitwig Studio
- Bitwig User Guide — Plug-ins and multi-out chains: https://www.bitwig.com/userguide/latest/vst_plug-ins/
- Bitwig Support — Multi-out VST plug-ins: https://www.bitwig.com/support/technical_support/how-do-i-use-multi-out-vst-plug-ins-27/
- Bitwig User Guide — Audio Sidechain modulator: https://www.bitwig.com/userguide/latest/modulator/
- Bitwig Support — Why Audio Units are not supported: https://www.bitwig.com/support/technical_support/why-doesnt-bitwig-studio-support-audio-units-13/

### REAPER
- REAPER official product page — supported plug-in formats and multichannel track routing: https://www.cockos.com/reaper/about.php
- REAPER User Guide v7.40 — Multi Output VSTi and 4-stereo-output example: https://dlz.reaper.fm/userguide/ReaperUserGuide740c.pdf
- REAPER Effects Guide — plug-in pin connectors and channel mapping: https://dlz.reaper.fm/userguide/REAPEREffectsGuide2021.pdf

### Plug-in API references (not host certification)
- Apple AUAudioUnit: https://developer.apple.com/documentation/audiotoolbox/auaudiounit
- Steinberg VST3 Processing FAQ: https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Processing.html
- Steinberg VST3 Multiple Dynamic I/O: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.md

## 8. Exit condition

A host/format cell is supportable only when:
1. the exact host version and macOS environment are pinned;
2. the intended component is scanned and instantiated;
3. the required output topology and fallback behavior pass;
4. live audio is proven to reach the granular source path, not just a side-chain detector;
5. session save/reopen restores bus activation, routing and state;
6. block/lifecycle changes are safe and produce no callback allocation/resizing;
7. MIDI, transport, automation and state tests required by `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md` pass;
8. every unsupported capability is documented and approved rather than silently omitted.

Until then, this matrix is an evidence map, not a certification report.


## 9. VST3-specific host evidence

The combined format table above is intentionally high-level. The VST3-specific instrument, full-band input, MPE, five-output, dynamic-bus and save/reopen evidence is expanded in [PHASE3_VST3_HOST_CAPABILITY_MATRIX.md](PHASE3_VST3_HOST_CAPABILITY_MATRIX.md). That document separates host-documented facilities from product-specific proof and records the current REAPER VST3 side-chain channel-count caveat. Neither matrix certifies a host or resolves Q-P06/Q-P09.


## 10. MPE host-delivery evidence — 2026-11-28

The detailed format/host audit is recorded in [PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md](PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md). The key cross-format finding is that host MPE support does not imply one common event representation:

- **Logic Pro / AU — P:** Apple documents MIDI Mono Mode, with each note and its control messages received on a separate channel and a configurable pitch range. The reviewed guide does not prove automatic MPE recognition for every third-party AU or the exact `supportsMPE` query path.
- **Ableton Live / AU — D at host API level:** Live queries `kAudioUnitProperty_SupportsMPE` (AUv3: `supportsMPE`) and, in MPE mode, sends channelized MIDI 1.0 per-note channels, Pitch Bend, CC74 and Channel Pressure; RPN 6 communicates zone configuration.
- **Ableton Live / VST3 — P:** VST3 has no MPE capability flag used by Live. Automatic recognition requires Ableton's recognized-plug-in list; manual MPE mode is available. The documented event path is channelized MIDI 1.0, not proof of VST3 `noteId` Note Expression delivery.
- **Bitwig / VST3 — D/P:** Bitwig documents VST3 per-note expressions, plug-in Use MPE/Force MPE controls and a configurable pitch-bend range. Exact event representation in each mode still requires a host trace.
- **REAPER / AU and VST3 — U:** official documentation reviewed confirms AU/VST3 formats and multichannel MIDI routing, but not an MPE plug-in handshake, zone contract or VST3 Note Expression delivery guarantee.

Architectural consequence: preserve both `MpeChannelKey` and `Vst3HostNoteKey` as distinct ingress identities that normalize to `SourceNoteIdentity`. Do not fabricate a VST3 note ID from a MIDI member channel. DEC-038 remains the product authority for ±48 semitones; each host must be configured and tested at minimum/center/maximum rather than assumed to use the product range by default.

This is evidence refinement only. It does not change DEC-026, DEC-034 or DEC-038; does not resolve Q-P06/Q-P09 or AR-03…AR-05; and does not certify any host or authorize a prototype.

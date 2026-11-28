# Phase 3 VST3 Host Capability Matrix

**Version:** 0.1 — official host-documentation cross-check, 2026-11-28  
**Status:** Evidence inventory only. No host is certified for GRANULAR SYNTH. No host prototype, SDK implementation, or compatibility promise is approved.  
**Authority:** DEC-020 (live input), DEC-026 (AU + VST3 R1 formats), DEC-034 (same-channel layer fan-out), DEC-038 (MPE per-note pitch range), GS-ARC-006/007 (master + four layer outputs and live input), Q-P06 and Q-P09.  
**Scope:** VST3 only. Logic Pro is retained in the comparison for clarity but is not a VST3 target; its documented third-party plug-in path is Audio Units.

## 1. Evidence vocabulary

| Label | Meaning |
|---|---|
| **D — documented** | Official host documentation explicitly describes the general host capability. It does not certify this product's exact topology. |
| **P — partial** | A related facility is documented, but it does not prove the full R1 behavior. |
| **U — unverified** | No sufficient official host evidence was found for the exact behavior; host testing is required. |
| **N/A** | The format is not applicable to this host's documented plug-in path. |
| **API only** | The VST3 API can express the capability; this does not prove host implementation. |

A **D** cell is not a GRANULAR SYNTH pass result. The distinction between raw full-band input and side-chain/modulator routing is mandatory: a side-chain detector response is not proof that the plug-in can use the supplied samples as a granular source.

## 2. Required VST3 validation topology

The first host-validation fixture is the candidate topology from `PHASE3_VST3_INSTRUMENT_COMPONENT_AUDIT.md`:

- One instrument-classified VST3 component.
- One event input bus with 16 MIDI channels as a candidate baseline.
- One stereo full-band audio input bus for the minimum live-source fixture; `kMain` versus `kAux` remains unresolved.
- One main stereo master output and four auxiliary stereo layer outputs.
- Internal four-layer routing, including DEC-034 same-channel fan-out.
- MPE per-note expression with note identity; DEC-038 requires a ±48-semitone per-note pitch range.
- Optional layer outputs may be inactive, but the master must still contain every layer exactly once.

The one-stereo-input fixture is not a product-level input cap. DEC-020 retains live input as an R1 requirement in plug-in contexts, with no fixed product-level source/channel maximum.

## 3. Host × VST3 evidence matrix

| Host | Instrument discovery | Full-band live-input bus | MPE to external plug-in | Five stereo outputs | VST3 bus activation | Session save/reopen |
|---|---|---|---|---|---|---|
| **Logic Pro** | **N/A** — Logic's documented third-party plug-in path is AU/AUv3, not VST3. | N/A | N/A | N/A | N/A | N/A |
| **Ableton Live** | **D** — VST3 instruments can be placed on MIDI tracks; Live documents that they receive MIDI and output audio. | **P** — Live documents third-party VST/AU external side-chain routing. That documents a side-chain path, not proof that the full-band input is available to the granular source reader as raw samples. | **P** — Live supports plug-in MPE mode, but VST3 has no MPE capability flag for automatic recognition; automatic mode requires Ableton's recognized-plug-in list, or the user can enable/save MPE mode manually. In MPE mode Live sends channelized MIDI 1.0 (per-note channels, pitch bend, CC74 and channel pressure), not evidence of VST3 NoteExpression noteId delivery. | **D** — Live documents up to 16 stereo outputs from one multi-out plug-in; five stereo outputs fit within that stated host limit. | **U** — exact activation/deactivation behavior for this VST3 input and four optional output buses is not documented in the reviewed host guide. | **P** — plug-in parameter configurations and Set state are saved, but persistence of this exact VST3 bus activation and routing arrangement is unverified. |
| **Bitwig Studio** | **D** — Bitwig documents adding multi-out VST plug-ins to instrument tracks. | **P** — Bitwig documents audio side-chain/modulator sources and VST plug-in routing, but the modulator path is not proof that a raw full-band input bus reaches the plug-in's granular source path. | **D/P** — Bitwig documents VST3 per-note expressions and plug-in Use MPE/Force MPE controls with a configurable pitch-bend range. The reviewed guides do not specify the exact event representation in each mode; noteId, channelized MIDI 1.0, Pressure/Timbre types and ±48 conversion still require a prototype trace. | **D/P** — multi-out VST chains and dynamically added output chains are documented. The exact five-stereo-pair declaration and independent stem behavior remain unverified. | **U** — host handling of the exact optional VST3 buses and activation state is not documented in the reviewed material. | **P** — Bitwig documents project-associated plug-in state. Persistence of this exact bus activation/routing arrangement is unverified. |
| **REAPER** | **D** — REAPER lists VST3 instruments and documents inserting virtual instruments on tracks. | **P** — REAPER supports multichannel track/pin routing and documents a VST3 side-chain channel-count request. It explicitly notes that it has no dedicated side-chain buses. This is a route to test, not proof of a distinct full-band instrument input bus. | **U** — official REAPER material reviewed documents AU/VST3 formats and up to 128 MIDI buses, but no MPE plug-in handshake, zone contract or VST3 Note Expression input guarantee. Raw channel preservation and any Note Expression events require a prototype trace. | **P** — the guide documents multi-output VSTi routing with a four-stereo-output example and multichannel pin connectors. A fifth independent stereo pair is not explicitly demonstrated. | **U** — the reviewed host documentation does not establish support for the exact VST3 dynamic bus activation lifecycle required here. | **P** — REAPER project save and plug-in state persistence are documented generally; exact restoration of this VST3 bus activation and pin-routing topology remains unverified. |

### 3.1 Interpretation and constraints

1. **Logic Pro is not a VST3 certification target.** Keep Logic in the overall format comparison for AU; do not report its VST3 cells as failed GRANULAR SYNTH tests.
2. **Ableton Live has the strongest published multi-output evidence** for the proposed five-pair output topology: its guide states a limit of 16 stereo outputs. This does not prove that every output bus will be exposed or that master fallback behaves correctly.
3. **Ableton side-chain is not automatically a granular source input.** The third-party side-chain workflow depends on plug-in side-chain support. The host test must confirm that the plug-in receives usable raw samples, not only a detector/trigger path.
4. **Bitwig's VST3 per-note-expression statement is meaningful host-level evidence**, but it does not settle the plug-in's Note ID association or DEC-038 range mapping. Its Audio Sidechain modulator is a modulation source, not proof of a raw audio input buffer.
5. **REAPER's VST3 side-chain workflow has a specific integration wrinkle:** the current guide says there are no dedicated side-chain buses and that the user may need to request the VST3 bus channel count so the plug-in interprets routed channels correctly. The prototype must test actual channel layout and source access.
6. **VST3 dynamic I/O is host-controlled.** Steinberg documents that the host activates/deactivates buses and may provide fewer bus buffers when trailing buses are inactive. A plug-in's default-active request is not a guarantee. Host-specific behavior must be tested.
7. Generic project/session saving is not proof that bus activation, routing, and optional output selection survive close/reopen. Treat those as separate acceptance observations.

### 3.2 MPE delivery-path correction

The MPE column above must be read with the format distinction in [PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md](PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md):

- Ableton Live's VST3 MPE mode is documented as channelized MIDI 1.0 (member-channel Note On/Off, Pitch Bend, CC74 and Channel Pressure). Live does not receive a VST3 MPE capability flag; automatic recognition requires Ableton's recognized-plug-in list, while manual MPE mode is available. Do not infer VST3 `noteId` delivery from this path.
- Bitwig documents VST3 per-note expressions and plug-in MPE controls, but the exact event representation and Note ID lifecycle in each mode still need a trace.
- REAPER's multichannel MIDI routing is not itself proof of MPE zone negotiation or VST3 Note Expression delivery.
- `INoteExpressionController` is a VST3 expression-type declaration interface, not an MPE zone handshake.

The VST3 adapter must preserve both possible ingress paths: explicit VST3 Note Expression identity and MIDI 1.0 channelized MPE identity. See the host-specific fixture in the linked audit.

## 4. Host prototype test plan

No prototype was run in this documentation pass. The following are test obligations, not assigned test IDs or passing results.

| Test | Required observation | Failure classification |
|---|---|---|
| Instrument discovery | Plug-in scans, is categorized as an instrument, instantiates on a MIDI/instrument track and receives note events | Host discovery / category incompatibility |
| Full-band live input | Route a known audio signal into the declared VST3 input; verify the actual input samples reach the layer source reader and can be recorded/granulated | Live-input host limitation or adapter/topology defect |
| Side-chain distinction | Confirm that a detector/modulator-only signal is not incorrectly counted as full-band source access | Evidence classification defect |
| MPE | Send overlapping notes with distinct note identities and per-note pitch/pressure/slide; verify independent response and ±48-semitone pitch range | Host event delivery or adapter translation defect |
| Five outputs | Feed distinguishable layer signals; verify master plus four independent stems, with no double-counting | Output declaration/routing defect |
| Optional bus permutations | Exercise master-only, master + selected layer outputs, and all outputs; record actual bus indices/counts and buffer validity on every process call | Bus activation/arrangement defect |
| Master fallback | Deactivate one or more layer outputs; verify each layer remains in master exactly once | Product routing invariant failure |
| Save/reopen | Save with non-default input/output activation and routing; close the project/host; reopen and verify plug-in state, bus activation, pins and routing | Session persistence defect |
| Reconfiguration | Exercise stop/deactivate, arrangement change, setup, reactivate and processing; verify no callback allocation, resizing, stale bus pointer or use of inactive buffers | Lifecycle / real-time safety defect |

For every run record: host product and exact version/build, macOS version, CPU architecture, VST3 binary/version and class ID, active/inactive buses, bus indices and channel arrangements, observed process input/output counts, sample rate, block sizes, MIDI/MPE event trace, project file, logs, and save/reopen outcome.

## 5. API-level constraints to preserve

- VST3 permits multiple audio/event buses and dynamic I/O, but the host controls activation.
- The processor must use the actual per-call `numInputs`/`numOutputs` and bus buffers; it must not assume every declared optional bus is present.
- `kDefaultActive` is a request only. Do not infer activation from declaration metadata.
- Bus arrangement changes follow the VST3 lifecycle; no scratch resize or graph rebuild may occur in `process()`.
- MPE per-note pitch requires note identity/Note Expression semantics; channel-wide pitch bend is insufficient.
- A candidate input bus type (`kMain` or `kAux`) must be judged by raw-source behavior in each host, not by naming convention.

## 6. Product consistency and unresolved decisions

This matrix does not alter the approved product scope:

- DEC-020: live external input remains required in plug-in contexts.
- DEC-026: AU and VST3 remain the R1 formats.
- DEC-034: all layers assigned to the same MIDI channel receive matching events.
- DEC-038: MPE per-note pitch range remains ±48 semitones.
- GS-ARC-006/007: master plus four layer outputs and live-input behavior remain required.

Still open:
- Q-P06: final VST3 component category and input bus type.
- Q-P09: exact minimum host versions/builds and certification scope.
- Host prototype evidence for full-band input, five outputs, dynamic bus activation and save/reopen.
- Exact MPE Note Expression mapping and ±48 range behavior in each certified host.

No framework, category string, bus type, host compatibility promise, product-scope change, code, prototype or benchmark is approved by this document.

## 7. Official source register

Checked 2026-11-28. Host documentation is version-sensitive; pin the exact target build before certification.

### Apple / Logic Pro
- Logic Pro User Guide — Work with Audio Units in Logic Pro: https://support.apple.com/en-am/guide/logicpro/lgcp22a0dab0/mac

### Ableton Live
- Live Manual 12 — Using Plug-ins (VST3 instruments on MIDI tracks): https://www.ableton.com/en/live-manual/12/using-plug-ins/
- Ableton Help — Using multi-out plug-ins (up to 16 stereo outputs): https://help.ableton.com/hc/en-us/articles/209773065-Using-multi-out-plug-ins
- Ableton Help — Sidechaining a third-party plug-in: https://help.ableton.com/hc/en-us/articles/209775325-Sidechaining-a-third-party-plug-in
- Live Manual 12 — Editing MPE and MPE in external plug-ins: https://www.ableton.com/en/live-manual/12/editing-mpe/

### Bitwig Studio
- Bitwig Support — Multi-out VST plug-ins on instrument tracks: https://www.bitwig.com/support/technical_support/how-do-i-use-multi-out-vst-plug-ins-27/
- Bitwig User Guide — VST plug-ins and multi-out chains: https://www.bitwig.com/userguide/latest/vst_plug-ins/
- Bitwig — VST3 per-note expressions: https://www.bitwig.com/de/modern-foundations/
- Bitwig User Guide — Audio-based modulators: https://www.bitwig.com/userguide/latest/modulator/
- Bitwig Release Notes — project-associated plug-in state: https://downloads.bitwig.com/stable/1.1.7/Release-Notes-1.1.7.html

### REAPER
- REAPER User Guide v7.86 (November 2026) — current guide, including VST3 side-chain channel-count behavior, Multi Output VSTi and pin connectors: https://dlx.reaper.fm/userguide/html/
- REAPER official product page — supported plug-in formats and routing overview: https://www.cockos.com/reaper/about.php

### Steinberg VST3 API (API capability only; not host certification)
- Multiple Dynamic I/O and bus activation: https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.html
- IComponent::activateBus and component state: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IComponent.html
- IAudioProcessor bus arrangements and setupProcessing: https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IAudioProcessor.html

## 8. Exit condition

A host/format cell may be called supported only after an owner-authorized prototype passes the relevant signal-level, MPE, bus activation, master fallback, lifecycle and save/reopen tests on a pinned host build and macOS environment. Until then this document is an evidence map, not a certification report.


## 9. VST3 state persistence and process lifecycle evidence — 2026-11-28

The VST3 host cells are refined by [PHASE3_CROSS_HOST_STATE_RESTORE_AND_RESET_EVIDENCE_MATRIX.md](PHASE3_CROSS_HOST_STATE_RESTORE_AND_RESET_EVIDENCE_MATRIX.md):

- **Ableton Live:** Live Set device/control settings and per-instance VST preset banks are documented; exact opaque processor/controller state round-trip with DEC-025 recording/Freeze payload remains unverified.
- **Bitwig:** project-associated `plugin-states` storage and sandbox/reload controls are documented; exact VST3 component/controller restore order and transient-voice behavior remain unverified.
- **REAPER:** FX-chain parameter settings and optional hard-reset-on-playback-start behavior are documented; exact per-instance opaque VST3 state and embedded-audio round-trip remain unverified.

Steinberg's VST3 persistence sequence is an **API contract**, not proof that a given host has been tested. Bus activation/routing after save/reopen remains unverified for the exact GRANULAR SYNTH topology. No host is certified.

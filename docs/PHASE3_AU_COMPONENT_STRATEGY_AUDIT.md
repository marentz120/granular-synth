# Phase 3 AU Component Strategy Audit

**Version:** 0.1 — evidence-based decision gate  
**Reviewed:** 2026-11-28  
**Status:** Architecture research and recommendation only. No AU component subtype, AU generation (v2/v3), framework, host compatibility claim or implementation is approved.  
**Authority:** DEC-020, DEC-026, GS-ARC-006, GS-SMG-020, Q-P06, Q-P09.  
**Related documents:** `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md`, `PHASE3_PLATFORM_AND_PLUGIN_RESEARCH.md`, `PHASE3_HOST_CAPABILITY_EVIDENCE_MATRIX.md`, `PHASE3_ARCHITECTURE_DECISION_REGISTER.md`.

## 1. Executive finding

The R1 AU component decision is not simply a naming choice. The two relevant Apple component roles express different host workflows:

- **`aumu` / Music Device:** an instrument that responds to MIDI and creates notes/audio.
- **`aufm` / Music Effect:** an audio effect that processes an audio stream and can respond to MIDI.

GRANULAR SYNTH is both a MIDI-playable instrument and a processor of live external audio. DEC-020 requires per-layer live input in plug-in contexts; DEC-026 requires AU and VST3. Neither product decision selects `aumu`, `aufm`, or a pair of AU components.

Apple's modern `AUAudioUnit` API exposes input and output bus arrays and MIDI-related capabilities, but that generic API surface does **not** prove that a host will expose an arbitrary audio input bus on an `aumu` component, nor that it will route that input to a sampler/granular source path. Conversely, the `aufm` type is explicitly an audio-processing role that responds to MIDI, but the type definition alone does not prove that every target host routes note events, MPE and the required multi-output topology to it.

**Conclusion:** retain `aumu` as the first validation candidate because the product's primary DAW identity is a playable instrument. Do not select it yet. The candidate passes only if a real host exposes a usable full-band audio input bus while preserving MIDI instrument behavior and the required outputs. If it fails, test `aufm` against the complete R1 instrument workflow. Ship both only if neither single-component workflow meets all R1 requirements and host evidence demonstrates a genuine need for two insertion roles.

## 2. Evidence and what it does — and does not — establish

### 2.1 Apple component roles

Apple documents the Music Device type as an instrument such as a sampler or synthesizer that responds to MIDI and creates notes. Apple documents Music Effect as an effect that can respond to MIDI control messages, typically to map MIDI messages to DSP parameters. Apple's archived Audio Unit guide also describes the conceptual distinction: `aumu` is audio creation driven by notes; `aufm` is audio modification that also responds to MIDI.

Sources checked 2026-11-28:

- Music Device: https://developer.apple.com/documentation/audiotoolbox/kaudiounittype_musicdevice
- Music Effect: https://developer.apple.com/documentation/audiotoolbox/kaudiounittype_musiceffect
- Audio Unit component-role guide: https://developer.apple.com/library/archive/documentation/General/Conceptual/ExtensibilityPG/AudioUnit.html
- Legacy Audio Unit programming guide (component semantics): https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/AudioUnitProgrammingGuide/TheAudioUnit/TheAudioUnit.html

These sources establish the intended component roles. They do not certify DAW insertion, audio-input routing, note-event routing, MPE, multi-output activation or session persistence for GRANULAR SYNTH.

### 2.2 Modern AU API boundary

The current `AUAudioUnit` API exposes `inputBusses`, `outputBusses`, `internalRenderBlock`, MIDI scheduling, host musical/transport context and an MPE capability property. Apple also documents that hosts may instantiate AUv2 or AUv3 components through the AUAudioUnit interface.

Sources:

- AUAudioUnit: https://developer.apple.com/documentation/audiotoolbox/auaudiounit
- Music Device type: https://developer.apple.com/documentation/audiotoolbox/kaudiounittype_musicdevice
- Music Effect type: https://developer.apple.com/documentation/audiotoolbox/kaudiounittype_musiceffect
- AVAudioUnitMIDIInstrument initializer (requires Music Device type): https://developer.apple.com/documentation/avfaudio/avaudiounitmidiinstrument/init%28audiocomponentdescription%3A%29
- AVAudioUnitEffect initializer (accepts Music Effect type): https://developer.apple.com/documentation/avfaudio/avaudiouniteffect/init%28audiocomponentdescription%3A%29

Important boundary:
- Generic AU input-bus APIs are not proof that `aumu` hosts expose an input bus in a usable way.
- A bus exposed as a sidechain/control input is not automatically equivalent to a full-band sample-source input. The test must prove that raw audio reaches the engine source path at the expected sample offsets and channel layout.
- MIDI support on `aufm` must be tested with Note On/Off, velocity, channel routing, pitch bend and the R1 MPE requirements; MIDI CC response alone is insufficient.
- AU component type and AU generation are separate axes. This audit does not choose AUv2 versus AUv3.

## 3. Candidate strategies

| Strategy | Product/host advantage | Main risk | Current disposition |
|---|---|---|---|
| **A. One `aumu`** | Natural instrument-track identity; MIDI note and performance workflow aligns with Granular/Sampler as playable instruments; one AU entry and one state identity | Host may not expose a usable full-band input bus on an instrument component; DEC-020 would be unmet in that host if live audio cannot reach the engine | **First validation candidate, not selected** |
| **B. One `aufm`** | Natural audio-in/audio-out processing role while retaining MIDI responsiveness; live audio source path is semantically direct | Host may treat it as an effect rather than an instrument; MIDI note/MPE routing, sequencer operation, instrument-track discoverability and five-output behavior may vary | **Fallback candidate; must pass the complete R1 instrument test** |
| **C. Both `aumu` and `aufm`** | Offers separate instrument and MIDI-controlled audio-processing insertion workflows if both are genuinely needed | Two AU entries, two host identities, duplicated certification and support matrix, user confusion, state interchange/migration risk and greater release maintenance | **Only if single-component options fail for distinct required workflows** |

### 3.1 Why `aumu` is the first test, not an approved choice

R1 is a playable instrument with two note-driven engines, four independent layers, MIDI/MPE, a central sequencer and five semantic output pairs. Instrument-track discoverability is therefore a strong product requirement. A single `aumu` would also minimize component count and state-identity complexity.

However, this advantage is conditional. An `aumu` that cannot receive host-routed live audio as a full-band source does not satisfy DEC-020 in plug-in contexts. Do not reinterpret a detector-only sidechain or a modulation tap as a passing live-input implementation unless the raw audio is actually available to the layer's Granular/Sampler source path.

### 3.2 When `aufm` becomes the preferred single component

Prefer a single `aufm` only if tests show all of the following in the target AU hosts:

1. The host supplies the required live audio input to the AU as a full-band source.
2. The AU receives the MIDI note events needed to play Granular and Sampler voices, not only CC parameter control.
3. Required channel/MPE expression and the central sequencer's event workflow remain usable.
4. The host can expose the master output and the required layer outputs, or an explicitly approved fallback preserves product behavior.
5. Session save/reopen, state restoration and parameter automation work without losing the source routing or sound state.
6. The effect insertion workflow is acceptable for a product whose primary use is also instrument performance.

If any of these conditions fail, `aufm` is not a valid single-component substitute.

### 3.3 When two AU components are justified

A dual-component release is justified only when evidence shows:
- `aumu` is the best/required workflow for MIDI instrument tracks but cannot meet the required live-audio path in one or more target hosts; **and**
- `aufm` can meet the live-audio + MIDI processing workflow in those hosts; **and**
- both workflows are genuinely required by R1 users rather than being implementation workarounds.

If both are shipped:
- both must instantiate the same framework-independent engine and canonical state schema;
- the AU component subtype remains distinct and host sessions must not be assumed to substitute one component for the other;
- preset/state interchange must be explicitly designed and tested; sharing a core or serialization library alone does not guarantee host-level component substitution;
- product names and descriptions must clearly distinguish Instrument and Live Audio Effect roles;
- each component receives its own host scan, MIDI, bus, automation, save/reopen, MPE and release certification.

## 4. Required prototype comparison — not authorized yet

The following is the minimum evidence plan for a future owner-authorized host prototype. This section does not authorize code or a prototype.

### 4.1 Test matrix

| Test | `aumu` | `aufm` | Pass evidence |
|---|---|---|---|
| Component discovery and insertion | Instrument category/track | Music-effect category/track | Exact host version, component description, insertion path and scan result |
| MIDI Note On/Off | Required | Required | Notes trigger/release Granular and Sampler voices correctly |
| Velocity, CC, pitch bend, channel routing, MPE | Required | Required | R1 MIDI/MPE event matrix, including ±48-semitone per-note pitch bend |
| Full-band live audio input | Required bus/source test | Required main audio input test | Raw mono/stereo samples reach the per-layer source path; not merely a sidechain detector/modulator |
| Four-layer source routing | Required | Required | One physical input can be routed/read by each layer according to the normalized source map |
| Master + four layer stereo outputs | Required | Required | Five independent stereo pairs or documented host limitation and owner disposition |
| Master fallback | Required | Required | Inactive layer outputs do not remove or double-sum the layer's master contribution |
| Host transport and sequencer | Required | Required | Valid host tempo/PPQ/play-state, sample-offset events and controller/MIDI operation |
| Automation | Required | Required | Stable parameter IDs, gestures, read/write and automation playback |
| Save/reopen and state | Required | Required | Embedded unsaved recordings restored; external library samples relinked; routing/bus activation survives |
| Offline render / lifecycle | Required | Required | State and render behavior remain valid through offline processing, stop/start and reconfiguration |

### 4.2 Candidate host order

- **Logic Pro:** mandatory first AU test; exercise instrument-track routing and a MIDI-controlled audio-effect insertion path.
- **REAPER or Ableton Live:** second AU host; choose the exact version under Q-P09 before certification. REAPER is a useful candidate because it supports both AU and VST3 on macOS; Ableton is valuable for the intended live/electronic workflow.
- **Bitwig:** AU is not applicable; keep it in the VST3 matrix only.
- Repeat the exact same core test fixture for both AU component candidates. Do not infer one component's success from the other's.

For each test record host version/build, macOS version, CPU architecture, component type/subtype/manufacturer, bus arrangements, active buses, MIDI event classes, input source path, output routing, session save/reopen result and reproducible project evidence.

## 5. Decision gate

Use this ordered gate; do not skip a failed condition:

1. **Test `aumu` with a real input bus.** If it passes the full R1 instrument, live-input, MIDI/MPE, output and state matrix in the required AU hosts, retain one `aumu`.
2. **If `aumu` fails the full-band live-input requirement, test `aufm`.** If `aufm` passes every R1 instrument and live-input requirement in the required hosts and its insertion workflow is acceptable, use one `aufm`.
3. **If neither single component passes, compare the dual-component workflow.** Ship both only if each component provides a distinct required workflow and both pass their own certification matrix.
4. **If no tested arrangement can satisfy DEC-020 in a required AU host, report the host/format limitation explicitly.** Do not silently claim AU live input is supported, and do not weaken DEC-020 in architecture documentation. Present the conflict to the owner before proposing a product-scope change.

This gate is an architecture recommendation. The owner retains final approval of the AU component topology.

## 6. Product-consistency check

| Approved constraint | Audit treatment |
|---|---|
| DEC-015: studio and live performance are both primary | Keeps both instrument-track and live-audio workflows in the evaluation |
| DEC-016/021: Standalone controller-only live operation and generic MIDI control | Requires MIDI operation and sequencing to remain intact; no named-controller dependency introduced |
| DEC-017/018: four layers, Granular and Sampler in R1 | Both component candidates must expose the same four-layer engine |
| DEC-019: central sequencer in R1 | Both candidates must preserve the sequencer and note/event workflow |
| DEC-020: live input per layer in Standalone and plug-in contexts | Full-band source-input test is a hard gate; sidechain presence alone is insufficient |
| DEC-026: AU and VST3 are R1 formats | AU strategy audit does not change VST3 or remove AU from R1 |
| GS-ARC-006: stereo master plus four stereo layer outputs | Both candidates must test the same five-pair output semantics |
| DEC-025: embedded unsaved recordings and external library references | Both candidates must preserve the same state contract |

## 7. Open items

- Q-P06 product behavior is answered by DEC-020; the AU component topology is still open.
- Q-P09 exact host versions/builds and certification scope remain open.
- No `aumu` or `aufm` host prototype has been run.
- No evidence currently proves AU live-input routing to the Granular/Sampler source path in Logic, REAPER or Ableton.
- No evidence currently proves the required five stereo output pairs, MPE and session persistence for either AU candidate.
- AUv2 versus AUv3 remains separate and open; this audit does not select it.
- Framework/licensing (Q-T01/Q-L04), deployment target (Q-T03), parameter IDs (Q-T12) and numeric resource budgets remain unchanged.

## 8. Review outcome

**Recommendation:** keep `aumu` as the first AU validation candidate, not as an approved implementation choice. Make live-input routing, MIDI/MPE, five-output behavior and save/reopen evidence the pass gate. Keep `aufm` as a fully tested fallback and permit a dual-component strategy only on demonstrated need.

No code, plugin shell, host prototype, benchmark, product-scope change or merge is authorized by this audit.

# Phase 3 MPE Host Compatibility Evidence Audit

**Date:** 2026-11-28  
**Version:** 0.1  
**Status:** Official-documentation evidence map only. No host/format cell is certified; no plug-in prototype, product compatibility promise, or implementation is approved.  
**Authority:** GS-MID-002, GS-MOD-004, DEC-026, DEC-034, DEC-038, AR-03…AR-05, Q-P06 and Q-P09.  
**Scope:** MPE and per-note-expression delivery to a third-party GRANULAR SYNTH instrument in Logic Pro, Ableton Live, Bitwig Studio and REAPER.

## 1. Executive finding

The reviewed hosts do not expose one uniform MPE delivery contract. The most important distinction is between:

1. **MIDI 1.0 MPE channelization:** each note is carried on an MPE member channel; pitch bend, channel pressure and CC74 are scoped to that channel. The plug-in must reconstruct note/channel lifecycles. This is the native AU MIDI 1.0 path and is also the documented input path used by Ableton Live when a plug-in is in MPE mode.
2. **VST3 Note Expression:** VST3 event data can address a note by `noteId`, and the optional `INoteExpressionController` interface advertises supported expression types. This is a separate VST3 facility, not a synonym for MIDI 1.0 MPE or a guarantee that every host sends Note Expression events.

A shared core must therefore accept two normalized ingress forms: channel-scoped MPE identities and explicit VST3 host-note identities. Neither adapter may assume that the other form is present. Steinberg documents Note Expression as an optional interface and per-note event mechanism; Ableton explicitly documents that its VST3 MPE mode sends channelized MIDI 1.0 messages and that VST3 has no API flag for automatic MPE recognition. [Steinberg Note Expression](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.5.0/INoteExpressionController.html) · [Ableton plug-in MPE developer guide](https://ableton.github.io/plugins/mpe-support-plugins).

## 2. Evidence labels

| Label | Meaning |
|---|---|
| **D — documented host capability** | Official host documentation explicitly describes the relevant general behavior. It is not a GRANULAR SYNTH test result. |
| **P — partial** | The host documents a related MPE workflow, but not the complete plug-in event contract or product behavior. |
| **U — unverified** | No sufficient official host documentation was found for the specific behavior. |
| **N/A** | The format is not a supported target for that host. |
| **API only** | The format API represents the behavior; host delivery is not established. |

A host-level **D** never means that the plug-in has passed a signal-level or event-level test.

## 3. Host × format evidence matrix

| Host / format | MPE recognition or mode | Documented input semantics | Note identity evidence | DEC-038 ±48 semitones | Current conclusion |
|---|---|---|---|---|---|
| **Logic Pro / AU** | **P** — Logic documents MPE controller use with instruments that support MIDI Mono Mode. Its guide does not establish that every third-party AU is automatically recognized through `supportsMPE`. | MPE notes and their per-note messages arrive on separate MIDI channels; base-channel messages affect all notes. Logic documents up to 15 note channels and a per-instrument Mono Mode Pitch Range that must match the controller. | **Channel-scoped only** in the documented MIDI Mono workflow; no AU noteId is documented. Exact third-party AU event delivery and cable/zone behavior remain unverified. | The host exposes a pitch-range setting, but the correct ±48 setting and exact third-party AU mapping must be configured and tested. | **P** — viable candidate; third-party AU behavior needs a prototype. |
| **Ableton Live / AU** | **D** — Live queries AU's `kAudioUnitProperty_SupportsMPE`; AUv3 exposes `AUAudioUnit.supportsMPE`. Manual MPE mode is also available. | In MPE mode Live sends one note per MIDI channel where possible, per-note pitch bend, CC74 slide and channel pressure. It signals zone configuration with RPN 6. | **MIDI 1.0 channelized identity**, not a VST3-style noteId. The AU adapter must use cable/zone/member-channel/note lifecycle. | Live's reviewed developer guide does not establish a product-specific ±48 setting; the plug-in/controller pitch-bend range must be validated. | **D for host MPE mode; P for product parity** — property query and event semantics are documented, but no GRANULAR SYNTH result exists. |
| **Ableton Live / VST3** | **P** — Live supports plug-in MPE mode, but VST3 has no API capability flag for automatic MPE recognition. Automatic recognition requires Ableton's recognized-plug-in list; the user can enable MPE manually or save a default configuration. | Live sends channelized MIDI 1.0 MPE: per-note channels, pitch bend, CC74 slide and channel pressure; RPN 6 announces the active zone. | **No VST3 Note Expression noteId is established by this Live MPE path.** The adapter must support the MIDI channel/member-channel lifecycle even in a VST3 build. | No universal Live-side ±48 configuration is established by the reviewed guide; test actual bend endpoints against the plug-in's signed-semitone conversion. | **P** — usable MPE path, but recognition/list inclusion and channelized adapter behavior are release/test dependencies. |
| **Bitwig Studio / VST3** | **D/P** — Bitwig states that it supports per-note expressions as part of VST3 and provides plug-in `Use MPE` / `Force MPE Mode` controls. Its plug-in Inspector exposes a configurable pitch-bend range. | The official guide documents the host controls and a configurable semitone range, but does not specify in the reviewed pages whether every plug-in MPE workflow arrives as VST3 Note Expression events, MIDI 1.0 channelized messages, or both. | **Partial** — VST3 per-note-expression support is documented, but the exact `noteId` lifecycle and event types delivered to this plug-in in each mode require a host trace. | **D for configurable range**, not a default guarantee. Configure ±48 and verify min/center/max values. | **D/P** — strongest documented VST3 per-note-expression evidence in this host set; exact event path and product mapping remain unverified. |
| **REAPER / AU and VST3** | **U** — official REAPER material reviewed documents AU/VST3 formats and extensive MIDI/multichannel routing, but no explicit third-party MPE plug-in handshake or VST3 Note Expression input contract. | Up to 128 track channels and 128 MIDI buses are documented. That makes channel routing plausible but does not prove MPE zone negotiation, event preservation or Note Expression delivery. | **U** — neither AU cable/member-channel handling nor VST3 `noteId` delivery is documented as a host MPE contract. | No official plug-in MPE range behavior was found in the reviewed REAPER documentation. | **U** — retain as a target candidate, but require an explicit event-level prototype before any compatibility claim. |
| **Logic Pro / VST3** | **N/A** | Logic's documented third-party plug-in path is AU/AUv3. | N/A | N/A | Not a VST3 target. |

## 4. Host-specific implications

### 4.1 Logic Pro

Apple documents MPE input as channel-per-note behavior for Logic instruments that support MIDI Mono Mode. Each channel is monophonic, with up to 15 notes, and the instrument's pitch range must match the controller. The guide names Logic instruments and also describes choosing a plug-in that supports MIDI Mono Mode; it does not prove automatic discovery of every third-party AU through the AU `supportsMPE` property.

**Prototype observations required:** third-party AU scan/instantiation; whether Logic exposes MPE input for the component; actual channel messages and base-channel messages; zone/channel count; pitch-range endpoints; same-pitch retrigger and sustain behavior. Do not assume a VST3 `noteId` exists in the AU path.

Source: [Apple Logic Pro User Guide — Use MPE with software instruments](https://support.apple.com/guide/logicpro/use-mpe-with-software-instruments/lgcp8f599497/10.7/mac/11.0).

### 4.2 Ableton Live

Ableton's developer guide is unusually precise and changes the adapter assumptions:

- For AU, Live queries `kAudioUnitProperty_SupportsMPE`; AUv3 can report `supportsMPE`.
- For VST3, the format has no API flag that Live uses to identify MPE support. Automatic MPE mode therefore requires Ableton's recognized-plug-in list; a user can instead enable MPE manually or save it in a default plug-in configuration.
- In MPE mode, Live sends channelized MIDI 1.0 messages: one note per member channel where possible, pitch bend and CC74 on that note channel, and channel pressure on that note channel.
- RPN 6 communicates lower/upper zone configuration and member-channel count. Zone deactivation is also communicated.

This is **not** evidence that Live emits VST3 `NoteExpressionValueEvent` with a VST3 `noteId` for this path. The VST3 adapter must be able to process the same channelized MPE messages as the AU adapter. A future Ableton whitelist request is a product-release dependency, not an API implementation detail.

Sources: [Ableton — Supporting MPE with plugins in Live](https://ableton.github.io/plugins/mpe-support-plugins) · [Ableton — MPE in Live FAQ](https://help.ableton.com/hc/en-us/articles/360019144999-MPE-in-Live-FAQ).

### 4.3 Bitwig Studio

Bitwig documents two relevant facilities: VST3 per-note expressions and plug-in MPE mode with a configurable pitch-bend range. The reviewed documentation does not define the precise processor event representation for every plug-in mode. The host prototype must separately log:

- VST3 `NoteExpressionValueEvent` presence, `noteId`, event bus and type IDs;
- MIDI 1.0 channelized Note On/Off, Pitch Bend, Channel Pressure, CC74 and RPN zone messages;
- whether the chosen `Use MPE` mode changes the event representation or only the channelized input;
- the configured pitch-bend range and actual min/center/max conversion.

Do not assume that Bitwig's support for the VST3 standard proves support for every custom Pressure/Timbre type proposed under AR-04.

Sources: [Bitwig — The Modern Foundations of Bitwig Studio](https://www.bitwig.com/de/modern-foundations/) · [Bitwig User Guide — Plug-in Handling and Options](https://www.bitwig.com/userguide/latest/vst_plug-in_handling_and_options/) · [Bitwig User Guide — Unified Modulation System](https://www.bitwig.com/userguide/latest/the_unified_modulation_system/).

### 4.4 REAPER

The official REAPER product documentation lists AU on macOS, VST3, up to 128 track channels and up to 128 MIDI buses. These are useful routing capabilities but are not an MPE-specific contract. No reviewed official REAPER source documents automatic MPE recognition, MPE zone signaling, VST3 Note Expression delivery or the plug-in's pitch-bend range.

The prototype must determine whether raw channelized MPE messages arrive unchanged on AU and VST3, whether RPN zone messages survive, and whether VST3 Note Expression events are supplied. Until then, mark MPE behavior **U**, not supported or unsupported.

Source: [REAPER official product page](https://cockos.com/reaper/about.php).

## 5. Shared-core and adapter consequences

1. Keep two distinct host identity keys:
   - `MpeChannelKey = cable + zone + memberChannel + leaseGeneration` for MIDI 1.0 MPE.
   - `Vst3HostNoteKey = eventBus + valid noteId` for VST3 Note Expression.
2. Both keys may resolve to the same semantic `SourceNoteIdentity`; do not synthesize a VST3 noteId from an MPE channel or pitch.
3. The VST3 adapter must support both VST3 Note Expression and channelized MIDI 1.0 input. Host capability evidence decides which path is expected for a pinned host/mode.
4. `INoteExpressionController` describes the plug-in's supported VST3 expression types. It is not an MPE zone negotiation mechanism and does not replace RPN 6 or MIDI member-channel state.
5. Preserve the distinction between Pressure (MPE channel pressure) and Timbre/Slide (MPE CC74) on channelized input. The product's final VST3 Pressure/Timbre type declarations remain AR-04.
6. Treat the ±48-semitone range as the approved product semantic (DEC-038), not as an assumption about a host default. Each host/format test must record the configured range and verify signed minimum, center and maximum conversion.
7. A host that supplies only channelized MPE cannot promise unique per-note control when multiple notes share a member channel. Preserve the existing `SharedChannel` identity quality and do not claim otherwise.

## 6. Phase 5 host test matrix

These are test-design obligations only. No AC IDs, capacities, pass thresholds or test results are added here.

| Host / format | Required MPE fixture | Evidence to capture |
|---|---|---|
| Logic / AU | MPE controller; MIDI Mono Mode-compatible third-party AU; overlapping notes on separate channels; base-channel message; range match/mismatch | Component discovery, channel event trace, zone/member-channel interpretation, pitch range setting and min/center/max output |
| Live / AU | AU property query true/false; MPE enabled/disabled; lower/upper zone RPN 6; overlapping notes; CC74/pressure/pitch | Property query, actual AUMIDIEvent cable/channel bytes, RPN sequence, note-channel messages and state after mode/zone change |
| Live / VST3 | Plug-in absent/present in recognized list; manual MPE on/off; saved default; lower/upper RPN 6 | Whether the plug-in enters MPE mode; raw MIDI event types/channels; CC74, channel pressure, pitch bend; confirm no assumed VST3 noteId |
| Bitwig / VST3 | Use MPE off/on; Force MPE; PB range values including ±48; overlapping notes and per-note expression lanes | Event trace separating Note Expression `noteId` events from channelized MIDI 1.0 events; type IDs, channel, configured range and conversion |
| REAPER / AU and VST3 | Raw multi-channel MIDI track input; zone RPN; overlapping notes; per-note pitch/pressure/CC74; Note Expression probe | Actual AU/VST3 input events, channel preservation, RPN handling, Note Expression presence/absence and range conversion |

Cross-host parity fixture:
- Send the same musical phrase and expression curve through channelized MIDI 1.0 MPE and VST3 Note Expression where the host supports each path.
- Compare normalized signed pitch semitones, pressure and timbre values at the shared-core boundary.
- Include overlapping same-pitch notes, channel exhaustion/shared-channel behavior, sustain, Note Off, release tails, zone changes, missing/duplicate VST3 note IDs and stale events.
- Verify DEC-034 fan-out to every matching layer exactly once.
- Verify no identity-table growth, allocation, blocking or unbounded lookup occurs in render.

## 7. Product consistency and open gates

Unchanged product decisions:
- GS-MID-002: per-note pitch, pressure and timbre remain R1.
- DEC-026: AU and VST3 remain the R1 plug-in formats.
- DEC-034: same-channel events reach every matching layer.
- DEC-038: the approved per-note pitch range remains ±48 semitones.
- DEC-021: no named-controller templates or MIDI feedback are introduced.

Open technical/evidence gates:
- AR-03: AU MPE matching, same-pitch Note Off, sustain and member-channel reuse.
- AR-04: VST3 Pressure/Timbre Note Expression type mapping.
- AR-05: exact public metadata and ±48 conversion.
- Q-P06/Q-P09: AU component strategy, target hosts/versions and certification scope.
- Ableton VST3 recognized-plugin-list inclusion or documented user MPE-mode setup.
- Per-host event-level proof for channelized MPE versus VST3 Note Expression.

No product decision, requirement tier, acceptance ID, host version, numeric capacity or performance budget is changed. No host is certified. No code, host prototype, benchmark or merge is authorized by this audit.

## 8. Official source register

Checked 2026-11-28:

- [Apple Logic Pro User Guide — Use MPE with software instruments](https://support.apple.com/guide/logicpro/use-mpe-with-software-instruments/lgcp8f599497/10.7/mac/11.0)
- [Apple AUAudioUnit.supportsMPE](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/supportsmpe)
- [Apple AUAudioUnit and MIDI event APIs](https://developer.apple.com/documentation/audiotoolbox/auaudiounit)
- [Ableton — Supporting MPE with plugins in Live](https://ableton.github.io/plugins/mpe-support-plugins)
- [Ableton — MPE in Live FAQ](https://help.ableton.com/hc/en-us/articles/360019144999-MPE-in-Live-FAQ)
- [Bitwig — The Modern Foundations of Bitwig Studio](https://www.bitwig.com/de/modern-foundations/)
- [Bitwig User Guide — Plug-in Handling and Options](https://www.bitwig.com/userguide/latest/vst_plug-in_handling_and_options/)
- [Bitwig User Guide — Unified Modulation System](https://www.bitwig.com/userguide/latest/the_unified_modulation_system/)
- [REAPER — About and supported plug-in formats/routing](https://cockos.com/reaper/about.php)
- [Steinberg — VST3 Note Expression 3.5.0](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.5.0/INoteExpressionController.html)
- [Steinberg — INoteExpressionController](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1INoteExpressionController.html)
- [Steinberg — NoteExpressionValueEvent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1NoteExpressionValueEvent.html)

## 9. Exit condition

MPE host support may be claimed only after an owner-authorized prototype on a pinned host build records the actual event representation, identity semantics, zone configuration, Pressure/Timbre delivery, ±48 pitch conversion and DEC-034 layer fan-out. Documentation of a host feature or plug-in API alone is insufficient.

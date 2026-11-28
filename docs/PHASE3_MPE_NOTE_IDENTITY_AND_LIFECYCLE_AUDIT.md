# Phase 3 MPE Note Identity & Lifecycle Audit

**Date:** 2026-11-28  
**Version:** 0.1  
**Status:** Evidence-backed architecture proposal; owner/host validation gates remain open. No public ABI, component strategy, MIDI note-matching policy, implementation, host certification or benchmark is approved.  
**Baseline:** DEC-015…DEC-042; GS-MID-001…008; GS-MOD-004; IF-01/02/03/09; Phase 3 cross-format parity audit.

## 1. Purpose and decision boundary

R1 requires per-note pitch, pressure and timbre (GS-MID-002), with the approved 48-semitone per-note pitch-bend range (DEC-038). AU and VST3 expose different native identities:

- AU receives MIDI 1.0 messages with a virtual-cable field and channel data. It declares MPE support, but its MIDI event structure does not contain a VST3-style note ID.
- VST3 Note On/Off events carry an optional note ID, and Note Expression events address that ID.
- MPE member-channel messages are channel-scoped. If a sender shares one member channel among multiple notes because the zone is out of member channels, those notes necessarily share that channel's expression.

The purpose of this audit is to define a shared semantic model and expose the remaining lifecycle decisions. It does not choose AU/VST3 component topology, MIDI zone defaults, ambiguous Note Off matching, a VST3 pressure/timbre type mapping, queue capacities or host support.

## 2. Evidence-backed API facts

### 2.1 Apple Audio Unit

- AUAudioUnit.supportsMPE is a read-only Boolean indicating that the Audio Unit supports multidimensional polyphonic expression. It is a capability declaration; it does not itself provide a note ID or prove that a host/controller has negotiated a particular MPE zone.
- AUMIDIEvent carries eventSampleTime, cable, byte length and MIDI data. The cable value must be retained with the event; MIDI channel and message type are decoded from the MIDI bytes.
- The AU event stream therefore requires an adapter-owned note lifecycle model. Do not fabricate a stable per-note identity from pitch alone.
- MPE zone configuration and member-channel interpretation are MIDI protocol state. Keep that state per virtual cable and only treat a channel as an MPE member channel when its zone role is known from valid configuration or an explicitly validated host contract.

### 2.2 Steinberg VST3

- NoteOnEvent and NoteOffEvent each carry a noteId. Steinberg documents -1 as “not available”; that value is not a valid per-note key.
- NoteExpressionValueEvent carries a noteId, a type ID and a normalized value in [0, 1]. The host must provide a sorted event list, and a note-expression event for a note follows its Note On.
- Steinberg's Note Expression example explicitly allows an expression event after Note Off while the voice remains in its release state. The processor must therefore not discard the note-ID mapping merely because Note Off arrived; retain it while the associated voice can still be addressed.
- INoteExpressionController is an optional IEditController extension through which a plug-in describes supported expression types per event bus and channel.
- VST3 PolyPressureEvent is keyed by pitch, not by the Note Expression noteId. It cannot provide unambiguous per-note pressure for overlapping notes of the same pitch; it is not a substitute for Note Expression.
- The standard VST3 note-expression types include Tuning, Expression and Brightness. The API also reserves a custom type-ID range. Product pressure and timbre semantics must not be silently aliased to a superficially similar standard type without an explicit mapping contract and host validation.

### 2.3 MIDI Polyphonic Expression

The MIDI Association's MPE specification describes member channels as the means for addressing individual notes. Where the sender has more active notes than available member channels, notes may share a channel; those notes continue sounding but are no longer uniquely controllable. Zone master-channel messages are distinct from member-channel per-note messages.

The current MPE specification is version 1.1 (M1-100-UM), replacing the earlier RP-053 version. Its default per-note pitch-bend range is commonly described as ±48 semitones; DEC-038 remains the project authority and is not rewritten by this audit.

## 3. Proposed shared identity model

The shared core should distinguish a source note from the voices created when that source note is routed to layers.

| Concept | Meaning | Lifetime |
|---|---|---|
| SourceNoteIdentity | One logical incoming note from a host/device source, independent of layer fan-out | From accepted Note On until the source note is no longer expression-addressable |
| NoteInstanceId | One layer-local voice instance created from a SourceNoteIdentity | From layer voice allocation until voice retirement |
| MpeChannelKey | Cable + negotiated zone + member channel + channel-lease generation | While that member-channel assignment is active |
| Vst3HostNoteKey | Event-bus index + valid host noteId | While the host note ID is valid for the note lifecycle |
| NoteIdentityQuality | Exact, SharedChannel, Missing, Ambiguous or Stale | Attached to the normalized event/identity record |

This distinction preserves DEC-034. One incoming note can fan out to every layer assigned to the incoming MIDI channel. Each receiving layer creates its own NoteInstanceId, while all those voices retain the same SourceNoteIdentity. A per-note expression update for that source note is delivered to all of its receiving layer voices.

A member channel is an expression scope, not necessarily a one-to-one note identity. If multiple active source notes share one MPE member channel, channel expression applies to all active notes in that channel group, and the identity quality is SharedChannel. Do not invent unique per-note control where the wire protocol does not provide it.

Candidate normalized expression dimensions are:
- Pitch: signed per-note pitch displacement, converted through the DEC-038 range.
- Pressure: normalized per-note pressure.
- Timbre: normalized per-note timbre/CC74 source.

These are semantic values, not host SDK types. Host adapters perform the format-specific conversion.

## 4. VST3 note-ID lifecycle

### 4.1 Admission and lookup

1. On Note On with a valid noteId, resolve the key (event bus index, noteId) and allocate a generation-safe SourceNoteIdentity.
2. If the same key is already associated with a note that has not finished its voice lifecycle, do not overwrite or alias the existing mapping. Mark the new identity as ambiguous, account for the malformed/colliding event and do not claim per-note expression for it.
3. If noteId is -1, the note may still be handled as an ordinary note, but no exact Note Expression identity is available. A fallback channel/pitch Note Off policy is a separate open decision; it must not be presented as MPE-equivalent.
4. NoteExpressionValueEvent is accepted only when its event bus and noteId resolve to a live identity and the event follows the corresponding Note On in the host's ordered event stream.
5. Note Off transitions the source note to released state. Keep its identity while the associated layer voice remains in its release lifecycle, because VST3 may deliver Note Expression after Note Off.
6. Once every associated voice has finished and no further note expression can be applied, retire the identity. A later reuse of the same host key creates a new internal generation; stale events must never bind to the new generation.

### 4.2 Expression type mapping gate

- VST3 Tuning is the candidate standard type for per-note pitch. Its normalized value must be converted using the documented Tuning mapping; it is not a direct semitone value.
- Pressure has no dedicated predefined VST3 Note Expression type. A custom Pressure type is a candidate. Do not silently map physical pressure to Expression/Volume semantics.
- Brightness is a candidate for a timbre-like control, but it is not automatically identical to the product's MPE Timbre source. A custom Timbre type is the alternative.
- The exact advertised type list, normalized range and host interoperability remain open. INoteExpressionController must describe only the types actually supported by the processor.

## 5. AU MPE member-channel lifecycle

### 5.1 Zone and channel state

Maintain zone state independently per AU virtual cable. The adapter must distinguish:
- A zone master channel.
- A member channel in a known zone.
- A channel whose zone role is unknown or invalid.
- A non-MPE MIDI channel.

AUAudioUnit.supportsMPE advertises the unit's capability to the host; it does not replace protocol-level zone state. If zone configuration is unavailable or invalid, ordinary MIDI notes may still be processed, but the adapter must not claim per-note MPE identity for the unknown channel.

### 5.2 Note and expression routing

1. A Note On on a known member channel creates a new SourceNoteIdentity and joins it to that cable/zone/member-channel expression group.
2. Member-channel Pitch Bend, Channel Pressure and CC74 are normalized as per-note expression for all currently active source notes in that channel group.
3. If multiple notes share the member channel, all active notes in the group receive the same member-channel expression. This is the protocol's shared-channel behavior, not a unique per-note mapping.
4. A Note Off must be matched using the cable, zone, member channel and pitch context. If more than one active note is a valid match, the adapter must report ambiguity rather than silently choosing FIFO, LIFO or “oldest”.
5. On a matched Note Off, the candidate behavior is to snapshot the last expression values onto the released voice and remove that source note from the active member-channel expression targets. A later channel reuse must not change the expression of an older release tail.
6. A monotonically increasing channel-lease generation distinguishes a newly reused member channel from its previous assignment. Old voice tails retain their snapshot, not a live reference to the new channel lease.
7. Zone reconfiguration invalidates the affected channel map at a safe event boundary. It must not rebind existing voices to a different zone or member channel.

The matching policy for overlapping same-pitch notes on one member channel, sustain interaction, and the exact zone-reconfiguration boundary remain open under AR-03. The candidate snapshot/detach behavior is a safety direction, not an approved MIDI product change.

### 5.3 Master-channel semantics

Master-channel expression is zone-scoped, not per-note. It must remain separate from member-channel per-note values. Do not route master-channel pitch bend or controllers into one arbitrary NoteInstanceId.

## 6. Cross-format lifecycle differences

| Lifecycle point | VST3 | AU MIDI 1.0 MPE | Shared-core treatment |
|---|---|---|---|
| Note On | Host noteId may identify the note | Cable + zone/member channel + pitch/lifecycle | Allocate SourceNoteIdentity |
| Per-note pitch | Note Expression Tuning candidate | Member-channel Pitch Bend | Convert to signed semitone displacement |
| Per-note pressure | Note Expression type mapping required | Member-channel Channel Pressure | Preserve pressure semantic; do not equate it with volume automatically |
| Per-note timbre | Note Expression type mapping required | Member-channel CC74 | Preserve timbre semantic; host type mapping remains open |
| Note Off | Carries noteId when available | MIDI channel + pitch; no native noteId | Release matching layer voices |
| Expression after Note Off | May still arrive while voice is released | Member channel may be reassigned after Note Off | VST3 may retain identity through release; AU snapshots/detaches when identity is no longer unambiguous |
| Channel sharing | Not inherent to a valid noteId | Possible when notes share a member channel | Explicit SharedChannel quality; expression fans out to all active notes in that group |
| Identity unavailable | noteId = -1 or unknown key | Unknown zone, ambiguous Note Off or invalid channel lifecycle | No silent channel-wide fallback |

Cross-format equivalence is required for the product semantics when both adapters can identify the same source note. The APIs do not guarantee identical post-Note-Off expression windows or identical host delivery; those differences must be explicit in host evidence and acceptance criteria.

## 7. Real-time, threading and memory contract

- Note identity lookup, channel-group membership and voice fan-out execute with bounded work on the audio-processing path.
- All note/identity tables and per-channel active-note lists are preallocated. No map growth, heap allocation, string lookup, blocking lock, logging or final destruction is allowed in render.
- Host event pointers and SDK objects are callback-borrowed. Copy only the bounded values needed by the adapter before returning from the host callback.
- AU MIDI ingress may have more than one producer context. Identity assignment occurs when the ordered event is consumed by the render domain; do not mutate the audio-owned identity table from an arbitrary scheduling thread.
- Each source identity and layer voice uses a generation-checked handle. A reused table slot must not make a stale expression event resolve to a new note.
- Event/identity capacity is finite. Exhaustion must be counted and handled by a declared bounded recovery path; it must not silently degrade a per-note event into channel-wide pitch bend.
- Note Off, All Notes Off, All Sound Off and Panic must have bounded recovery paths. Panic must clear active identity/channel bindings without waiting for a worker.
- Numeric identity capacity, overflow policy and maximum simultaneous MPE notes remain Phase 5 engineering/measurement gates; this audit sets no numeric limit.

## 8. Open architecture decisions

| ID | Open question | Candidate direction | Required evidence |
|---|---|---|---|
| AR-03 | AU MPE matching and channel-lease lifecycle: same-pitch overlapping Note On/Off, shared member channels, sustain/release tails and zone reconfiguration | Keep SourceNoteIdentity separate from channel expression scope; report ambiguity; snapshot/detach released notes before channel reuse | MPE 1.1 protocol review, exact event fixtures, sustain/retrigger/channel-reuse tests and owner review of any user-visible ambiguity behavior |
| AR-04 | VST3 Note Expression mapping for product Pressure and Timbre, and host delivery parity | Use Tuning for pitch as the standard candidate; explicitly declare Pressure/Timbre standard or custom types; never silently alias semantics | SDK type review and per-host tests for Note Expression discovery, recording, playback and event delivery |
| AR-05 | Exact public mapping of DEC-038's 48-semitone per-note range to VST3 Note Expression metadata and AU pitch-bend input | Retain the existing project interpretation pending final metadata review; convert through explicit signed semitone units | Confirm DEC-038 wording against MPE 1.1 default and test minimum, center and maximum values in every target host |

AR-03…AR-05 are technical gates. They do not change the approved R1 feature scope. If a final choice changes user-visible behavior or the approved pitch range, it requires owner approval.

## 9. Phase 5 validation families

These are test-design obligations, not new AC IDs or passing results.

| Test family | Candidate scenarios | Requirements / decisions |
|---|---|---|
| VST3 note-ID admission | Valid ID, -1, duplicate active ID, ID reuse after voice finish, stale expression after retirement | GS-MID-002; AR-04 |
| VST3 Note Expression lifecycle | Expression before Note On, after Note On, after Note Off during release, after voice finish; event ordering | GS-MID-002; GS-VOI-001…010 |
| AU zone/cable isolation | Multiple virtual cables, lower/upper zones, known/unknown zone, zone reconfiguration, invalid member channel | GS-MID-002; AR-03 |
| AU channel sharing | Multiple active notes on one member channel; expression reaches all active notes and not unrelated channels/cables | GS-MID-002; DEC-034 |
| AU Note Off matching | Same-pitch retrigger, different pitches on one member channel, unmatched Note Off, ambiguous duplicate pitch | GS-MID-002; AR-03 |
| AU channel reuse | Note Off followed by new Note On on same member channel while old voice tail or sustain remains | GS-MID-002; AR-03 |
| Cross-layer fan-out | One source note reaches two or more layers assigned to the same channel; expression reaches each associated layer voice once | GS-MID-001/002; DEC-034 |
| Expression conversion | Pitch-bend minimum/center/maximum, pressure endpoints, CC74 endpoints, VST3 normalized values and range conversion | GS-MID-002; DEC-038; AR-05 |
| VST3 type interoperability | Note Expression type discovery per event bus/channel; standard/custom Pressure and Timbre alternatives across Q-P09 hosts | GS-MID-002; AR-04; Q-P09 |
| Bounded identity overload | Identity-table saturation, event overflow, panic under overload, stale-generation rejection and no render allocation | GS-MID-002; GS-NFR-001; AR-02 |

No numeric event capacity, timing tolerance, host version or benchmark result is assigned. No new AC identifiers are created in Phase 3.

## 10. Product-consistency check

This audit preserves:
- GS-MID-002: per-note pitch, pressure and timbre remain R1.
- DEC-038: the approved 48-semitone per-note pitch-bend range remains unchanged.
- DEC-034: every layer assigned to the matching MIDI channel receives the event.
- GS-MOD-004: MPE timbre and per-note modulation sources remain available to the modulation architecture.
- DEC-017/019: four layers continue to share one central sequencer and transport.
- DEC-021: no named-controller template or MIDI feedback is introduced.

No product requirement, tier, approved decision or acceptance ID was changed.

## 11. References

### Apple
1. [AUAudioUnit.supportsMPE](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/supportsmpe) — capability declaration for multidimensional polyphonic expression.
2. [AUMIDIEvent](https://developer.apple.com/documentation/audiotoolbox/aumidievent) — virtual cable, sample time and MIDI byte payload.

### Steinberg
1. [NoteOnEvent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1NoteOnEvent.html) — optional noteId and -1 when unavailable.
2. [NoteOffEvent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1NoteOffEvent.html) — associated Note On identifier.
3. [NoteExpressionValueEvent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1NoteExpressionValueEvent.html) — noteId and normalized value.
4. [INoteExpressionController](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1INoteExpressionController.html) — optional expression-type declaration interface.
5. [NoteExpressionTypeIDs](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/namespaceSteinberg_1_1Vst.html) — standard and custom note-expression type ranges.
6. [VST3 Note Expression 3.5.0](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.5.0/INoteExpressionController.html) — event ordering and released-voice example.

### MIDI Association
1. [MPE: MIDI Polyphonic Expression, M1-100-UM v1.1](https://midi.org/mpe-midi-polyphonic-expression) — member-channel, zone and per-note expression protocol.
2. [MPE specification adopted](https://midi.org/midi-polyphonic-expression-mpe-specification-adopted) — member-channel sharing and its loss of unique per-note control.

**Review outcome:** the shared identity model and lifecycle boundaries are now explicit enough for architecture review and Phase 5 test design. AU same-pitch matching, VST3 Pressure/Timbre type mapping and final public pitch-range metadata remain open. No code, host prototype, benchmark or product decision was made.


## 12. Host-delivery qualification — 2026-11-28

The shared identity model above must not be interpreted as one identical host event path. See [PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md](PHASE3_MPE_HOST_COMPATIBILITY_EVIDENCE_AUDIT.md) for the full official-documentation matrix.

The most important correction is Ableton Live's documented plug-in behavior:

- Live's AU path can query the AU MPE capability property. In MPE mode, Live sends MIDI 1.0 member-channel messages: one note per channel where possible, per-note Pitch Bend, CC74 slide and Channel Pressure; RPN 6 communicates the active zone/member-channel count.
- Live's VST3 path has no VST3 MPE capability flag for automatic recognition. Automatic mode requires Ableton's recognized-plug-in list, or the user can enable/save MPE mode manually. Once enabled, Live's documented MPE input is still channelized MIDI 1.0. It is not evidence that Live sends VST3 `NoteExpressionValueEvent` with `noteId`.
- Bitwig documents VST3 per-note expressions and plug-in MPE mode/range controls, but the exact event representation in each mode remains a prototype observation.
- Logic documents MIDI Mono Mode as channel-per-note behavior with a configurable pitch range; arbitrary third-party AU discovery and exact event delivery remain unverified.
- REAPER's documented multichannel MIDI routing is not an MPE or Note Expression compatibility guarantee.

Therefore:
1. Keep `MpeChannelKey` and `Vst3HostNoteKey` separate.
2. Allow a VST3 adapter to normalize either channelized MIDI 1.0 MPE or explicit VST3 Note Expression, depending on the host/mode evidence.
3. Do not treat `INoteExpressionController` as MPE zone negotiation; it advertises VST3 expression types.
4. Preserve DEC-038's ±48-semitone product range, but record each host's configured pitch-bend range and verify endpoint conversion.

No host support, component strategy, event-type mapping, matching policy or product decision is approved by this supplement. AR-03…AR-05 and Q-P06/Q-P09 remain open.

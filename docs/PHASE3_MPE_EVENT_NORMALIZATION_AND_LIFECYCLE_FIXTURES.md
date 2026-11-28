# Phase 3 MPE Event Normalization & Lifecycle Fixture Specification

**Date:** 2026-11-28  
**Version:** 0.1  
**Status:** Detailed Phase 5 fixture proposal only. No implementation, host prototype, host certification, new AC ID, formal test ID or final AR-03/AR-04/AR-05 decision is approved.  
**Authority:** GS-MID-002, GS-MOD-004, DEC-034, DEC-038, GS-NFR-001, IF-03, IF-09, AR-02…AR-05 and Q-P09.

## 1. Purpose and boundary

This document turns the MPE identity and host-evidence audits into deterministic event fixtures reusable across AU, VST3 and Standalone. It separates host delivery, adapter normalization, shared-core identity/fan-out and voice retirement.

It does not select AU same-pitch Note Off matching, sustain/channel-reuse boundaries, VST3 Pressure/Timbre type IDs, exact public pitch metadata, numeric capacities, host versions or user-visible fallback behavior.

## 2. Fixture model

### 2.1 Observable stages

| Stage | Evidence captured | Ownership |
|---|---|---|
| Host ingress | Format, event class, cable/event bus, channel, noteId, timestamp/offset and host mode | Host callback; borrowed pointers never escape |
| Adapter normalization | Bounded copied event, ingress namespace, identity quality, expression dimension/value and source-local order | Adapter-owned record |
| Shared-core result | SourceNoteIdentity transition, NoteInstanceId fan-out, expression delivery and retirement/rejection reason | Render-domain state, observed only through test snapshots |

The test harness must not inspect a live mutable identity table concurrently with rendering. Capture immutable snapshots at explicit block boundaries.

### 2.2 Separate ingress identities

| Path | Native key | Ingress namespace |
|---|---|---|
| AU MIDI 1.0 MPE | Cable + negotiated zone/member channel + note lifecycle | AU virtual cable |
| VST3 channelized MIDI 1.0 MPE | Event bus + negotiated zone/member channel + note lifecycle | VST3 event-bus index |
| VST3 Note Expression | Event bus + valid host noteId | Event-bus index + noteId |
| Standalone MIDI 1.0 MPE | Input port/device + cable where exposed + zone/member channel + note lifecycle | Stable input-port/device identity |

For channelized MPE, the candidate MpeChannelKey is scoped by ingress namespace, negotiated zone, member channel and channel-lease generation. A numeric MIDI channel alone is not globally unique across cables, ports, event buses or formats.

VST3 Note Expression remains a separate key: (eventBusIndex, valid noteId). Never synthesize a VST3 noteId from an AU cable, MIDI channel or pitch.

### 2.3 Shared semantic values

- Pitch is signed per-note displacement in semitones; DEC-038 remains ±48 semitones.
- Pressure and Timbre remain separate semantic dimensions.
- Channelized MPE pressure and CC74/timbre are channel-scoped to the member-channel group.
- A candidate normalized domain [0,1] may be used for fixture arithmetic only. It is not final host metadata or a VST3 type mapping. Exact conversion and standard/custom type selection remain AR-04/AR-05.

## 3. Stable fixture invariants

1. A source note is distinct from every layer-local voice created by fan-out.
2. One SourceNoteIdentity creates one NoteInstanceId in each matching layer under DEC-034; each receiving layer is updated exactly once.
3. Channelized MPE expression applies to all active notes in the matching member-channel group. Shared-channel notes are not claimed to have independent expression.
4. VST3 Note Expression is scoped to a valid (eventBusIndex, noteId) pair. Missing, duplicate-live and stale keys cannot alias another note.
5. Identity-unavailable expression never silently falls back to channel-wide pitch bend while claiming per-note compliance.
6. Source-local event order and validated sample offsets are preserved. Cross-source same-offset writer precedence remains AR-01 and is not asserted here.
7. Identity lookup, member-channel broadcast, layer fan-out and retirement are bounded and allocation-free on the render path.
8. A retired or generation-stale identity never resolves to a reused table slot, note ID or channel lease.

## 4. Deterministic scenario catalogue

Names such as source-A, voice-L1 and noteId=41 are fixture labels, not public IDs. A 64-frame block and offsets 0, 8, 16, 32 and 63 are illustrative coordinates only; they do not set a product block-size limit or timing tolerance. Host SDK serialization is tested separately.

### Valid VST3 Note Expression lifecycle

Input: Note On (event bus 0, noteId 41, pitch 60, offset 8); Note Expression for the same key (offset 16); Note Off (offset 32); another expression during release (offset 40); voice completion and identity retirement; then a stale expression.

Expected: one SourceNoteIdentity; expression reaches only its associated layer voices; Note Off does not retire the mapping while a release voice remains addressable; the post-retirement stale event is rejected. Exact host type/value encoding remains AR-04/AR-05.

### Missing VST3 noteId

Input: Note On with noteId -1, Note Expression with noteId -1, then Note Off without a usable identity.

Expected: an ordinary note may still be represented if the adapter supports that path; -1 is never inserted into the exact Note Expression lookup; the expression is identity-unavailable and is not attached to another note or downgraded to channel-wide pitch bend. Fallback Note Off matching remains a separate policy.

### Duplicate live VST3 noteId

Input: admit (eventBus 0, noteId 41), then reuse the same key before the first lifecycle ends, followed by expression and Note Off.

Expected: the first mapping is not overwritten; the second admission is marked collision/ambiguous and cannot inherit the first note's expression target. Audible recovery for malformed duplicate IDs remains an explicit adapter policy.

### VST3 noteId reuse after retirement

Input: admit noteId 41, release and finish all associated layer voices, retire the identity, then admit a new note with noteId 41 and inject a stale old-generation handle in the test harness.

Expected: the new note has a new internal generation; the stale handle cannot resolve to it.

### Channelized MPE shared-member-channel expression

Input: configure a known zone; admit two different pitches on one member channel; admit another note on a different member channel or cable/event bus; send member-channel pitch, pressure and CC74 changes.

Expected: both notes in the shared group receive the same expression; the other channel/namespace is unchanged; identity quality records shared-channel scope; a numeric channel match on another cable or event bus never causes cross-routing.

### Same-pitch overlap and Note Off ambiguity — AR-03 gated

Input: two same-pitch Note Ons on one channelized MPE member channel, followed by one Note Off.

Evidence: record every valid candidate match. If more than one note is a valid match, do not silently claim an exact identity. FIFO, LIFO or another user-visible rule is not selected; final pass behavior is blocked on AR-03 and owner review.

### Member-channel reuse with a release tail — AR-03 gated

Input: Note On on a known member channel; Note Off while the voice remains in release/sustain lifetime; reuse the channel for a new note; change expression on the new lease.

Candidate safety assertion: new-lease expression must not mutate an older release voice that is no longer an active target. Snapshot/detach is a candidate mechanism only; the exact Note Off, sustain and detach boundary remains AR-03.

### Cable, event-bus and zone isolation

Input: two AU cables or VST3 event buses use the same numeric member channel; configure distinct known zones; admit notes in both; send expression on one namespace; reconfigure one zone.

Expected: expression is isolated by ingress namespace and zone; no numeric-channel cross-routing; an existing identity is never rebound to a different zone/member-channel lease. Active-note behavior and the safe reconfiguration boundary remain AR-03.

### DEC-034 layer fan-out

Input: configure two or more layers for the same source MIDI channel; admit one source note through channelized MPE or VST3 Note Expression; send pitch, pressure and timbre changes.

Expected: one SourceNoteIdentity; one distinct NoteInstanceId per matching layer; exactly one expression update per associated layer voice; no update to non-matching layers. Retiring one layer voice does not retire the source while another remains addressable.

### Canonical expression endpoints

| Dimension | Candidate shared-core values |
|---|---|
| Pitch | -48 semitones, 0 semitones, +48 semitones |
| Pressure | 0, 0.5, 1 |
| Timbre | 0, 0.5, 1 |

Expected: pitch remains signed and monotonic with center at zero; pressure and timbre remain distinct. Compare AU/VST3 conversions only after approved host codecs exist. These vectors do not select VST3 Tuning encoding, Pressure/Timbre type IDs, host defaults or public metadata.

### Event order and block boundaries

Input: Note On at offset 0; following same-source expression at the same offset in host-supplied order; another event at the final valid sample; separate invalid, stale and future timestamp cases.

Expected: valid offsets and source-local order are preserved; VST3 Note Expression follows its Note On; AU absolute time uses checked signed conversion against a valid block anchor; invalid events are not clamped to zero or the final sample. Cross-source same-offset precedence is not selected.

### Panic, All Sound Off and identity exhaustion

Input: admit notes with layer fan-out; trigger All Sound Off/Panic while notes and release tails exist. Separately fill test identity/event storage to its declared capacity and attempt one more admission/event.

Expected: bounded invalidation/cleanup without worker waits; stale events cannot resolve after clear; exhaustion is counted and cannot allocate, block, alias another note or silently change expression scope. Exact capacity and recovery remain AR-02/Phase 5 gates. All Notes Off remains distinct from All Sound Off/Panic.

## 5. Host/format execution matrix

| Host / format | Path to inspect | Required distinction |
|---|---|---|
| Logic Pro / AU | Channelized MIDI 1.0 MPE | AU cable, zone/member channel and actual MIDI bytes; third-party delivery remains partially evidenced |
| Ableton Live / AU | Channelized MIDI 1.0 MPE | supportsMPE, zone signaling and member-channel messages |
| Ableton Live / VST3 | Channelized MIDI 1.0 MPE in documented MPE mode | Do not assume VST3 Note Expression noteId; record recognized-list/manual-mode state |
| Bitwig Studio / VST3 | Channelized MPE and/or Note Expression as actually observed | Record event class, event bus, noteId, type ID and mode |
| REAPER / AU and VST3 | Probe actual event delivery | Multichannel MIDI routing is not proof of MPE/Note Expression delivery |

A host run must record version/build, OS, format, plugin configuration, MPE mode, configured pitch range, cable/event bus, zone/RPN state, raw event trace and normalized trace. Without an authorized prototype, the host cell remains unverified.

## 6. Evidence and result classification

Retain the deterministic event sequence, ingress metadata, raw values required for conversion, normalized event trace, identity/generation transitions, layer fan-out, expression delivery counts and rejection/ambiguity/overflow reasons. Test-only allocation/lock observations must not log from the real-time callback.

Classify outcomes as:

- **Invariant verified:** stable semantic/ownership invariant observed.
- **Invariant failed:** identity alias, stale binding, wrong-layer delivery, lost order, silent fallback, render allocation/blocking or unbounded work observed.
- **Decision-gated:** behavior depends on unresolved AR-03/AR-04/AR-05.
- **Host-unverified:** actual host ingress representation/delivery has not been observed.

A fixture definition is not a passing result. No host is certified.

## 7. Traceability

| Requirement / decision | Coverage |
|---|---|
| GS-MID-002 | Note identity, pitch, pressure, timbre, missing/ambiguous/stale identity and lifecycle |
| GS-MOD-004 | Timbre and per-note modulation semantic separation |
| DEC-034 | Fan-out to every matching layer exactly once |
| DEC-038 | Signed pitch semantic endpoints at -48 / 0 / +48 semitones |
| GS-MID-001/003/005/006 | Event ingress, channel routing, order and bounded recovery where applicable |
| GS-VOI-001…010 | Note admission, release, retrigger and voice lifecycle |
| GS-NFR-001 | No render-time allocation/blocking and bounded work |
| AR-02 | Capacity exhaustion and recovery remain gated |
| AR-03 | AU matching, sustain, zone reconfiguration and channel reuse |
| AR-04 | VST3 Pressure/Timbre type mapping |
| AR-05 | Public metadata and exact ±48 conversion |
| Q-P09 | Host/version/mode execution matrix |

The 124-row requirement inventory and 53/124 acceptance-link baseline are unchanged. No AC IDs, formal test IDs, numeric capacity, performance threshold or test result is created.

## 8. Exit condition

The fixture design is ready for review when stable invariants are accepted, AR-03…AR-05 remain explicitly gated where necessary, shared-core fixtures do not depend on raw host SDK types, and later Phase 5 work assigns formal AC/test IDs and evidence retention. No test is reported as passed until executed and recorded.

**Boundary:** documentation and test design only. No DSP, Plugin Shell, host prototype, benchmark, framework selection, parameter-ID freeze or PR merge is authorized.

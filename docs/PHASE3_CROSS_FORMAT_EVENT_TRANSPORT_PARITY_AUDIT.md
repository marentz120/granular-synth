# Phase 3 Cross-Format Event & Transport Parity Audit

**Date:** 2026-11-28  
**Version:** 0.1  
**Status:** Evidence-backed architecture proposal; requires technical review. No public ABI, framework, component topology, host certification, product change, implementation or benchmark is approved.  
**Baseline:** PR #6 Phase 3 architecture branch; DEC-015…DEC-042; IF-01…IF-12; AU and VST3 public API documentation reviewed 2026-11-28.

## 1. Purpose and decision boundary

This audit checks whether Standalone, Audio Unit and VST3 can deliver equivalent musical timing and parameter behavior to the shared engine without pretending their host APIs expose identical data.

The required outcome is a common semantic model with format-specific adapters. It is not a lowest-common-denominator model: adapters must preserve information that exists in one format (for example VST3 note IDs or AU parameter-ramp duration) and explicitly report information that a host does not provide.

This audit does not resolve AR-01, Q-T12, Q-P06, Q-P09, Q-T13 or MIDI Clock/host-transport fallback policy. It does not assign new AC IDs or numeric queue capacities.

## 2. Executive findings

1. **Event time is represented differently.** AU render events carry sample-time timestamps in the AU timeline; VST3 discrete events carry a block-relative `sampleOffset`. The adapter must normalize both to the current render block using checked arithmetic.
2. **Automation is not just a list of parameter steps.** AU parameter events can specify a ramp duration. VST3 `IParameterChanges` provides per-parameter queues describing piecewise-linear automation curves, including implicit boundary information. Flattening either format into one `TimedEvent::ParameterChange` loses semantics.
3. **MPE note identity differs.** VST3 Note Expression refers to a `noteId`; AU MIDI input is a MIDI 1.0 byte stream with cable/channel context and an AU-level MPE capability declaration, not the same native note-ID field. AU therefore needs an explicit MPE member-channel/note-lifecycle adapter.
4. **Transport fields are independently optional.** AU exposes musical context and transport state through separate callbacks with success results. VST3 exposes a `ProcessContext` with per-field validity flags; accurate context requests are made through `IProcessContextRequirements` in SDK 3.7+.
5. **Loop/cycle discontinuity is a real parity risk.** Steinberg documents that hosts may split a block at a cycle jump to provide correct project time inside each part, but this behavior is not mandatory. A single block-start PPQ value must not be assumed to describe an in-block loop wrap.
6. **A VST3 parameter flush is not an audio block.** A no-audio-bus flush may apply bounded parameter updates, but it must not render samples, access audio scratch, advance transport or trigger sequencer time.

## 3. Format-specific evidence and adapter obligations

### 3.1 Audio Unit

**Discrete MIDI events**
- `AUMIDIEvent` exposes `eventSampleTime`, `cable`, byte length and MIDI data. The adapter must retain cable, MIDI channel, message order and note lifecycle information.
- AU event sample time is a signed sample-count domain. Do not cast it to an unsigned block offset before validating the difference from the render block's sample-time anchor.
- `AUEventSampleTimeImmediate` is a scheduling convenience for the next cycle, optionally plus a small sample offset. Apple documents that the base AUAudioUnit implementation translates this constant before subclasses see the event; do not treat the constant as a literal negative/large timestamp in the shared core.
- The render-event representation includes MIDI and parameter event forms. The adapter walks only the callback-borrowed event data and copies the required values into preallocated bounded storage.

**Parameter automation**
- `AUParameterEvent` provides an event sample time, parameter address, target value and `rampDurationSampleFrames`. AU values use the AU parameter's declared value domain; `AUParameter` exposes min/max and unit metadata, so an `AUValue` is not guaranteed to be normalized to [0,1].
- A nonzero ramp duration is meaningful only for a parameter whose flags permit ramping. Apple's scheduling documentation says a non-rampable parameter with a nonzero ramp duration remains unchanged; the adapter must not silently convert that request to an immediate step.
- Normalize a supported AU ramp as a time-bounded automation segment with its exact start offset, duration and target. A zero-duration event is a step.
- `scheduleParameterBlock` is documented as safe from any thread context, including realtime render threads. The AU ingress must not assume one producer and must not feed an SPSC queue unless a single-producer guarantee is independently established.

**Transport**
- `musicalContextBlock` supplies tempo, time signature, precise beat position at the beginning of the current buffer, samples to next beat and measure downbeat. Its callback returns success/failure; individual output pointers may be null.
- `transportStateBlock` separately supplies transport flags, current sample position and cycle start/end beat positions. It also returns success/failure.
- Cache the host callbacks before realtime rendering as required by the API. Build one normalized snapshot from the fields that were actually returned successfully. A successful musical-context callback does not imply that transport-state fields are valid, or vice versa.

### 3.2 VST3

**Discrete MIDI and per-note expression**
- `Vst::Event::sampleOffset` is expressed in sample frames relative to the current block start. The event also carries `busIndex`, `ppqPosition`, event type and the typed event payload.
- Note On/Off and Note Expression are separate event types. `NoteExpressionValueEvent` refers to a specific `noteId`, uses a normalized value, and Steinberg requires Note Expression events for a note to follow its Note On; the host must provide a properly sorted event list.
- Preserve the event bus and note ID. A missing note ID (for example `-1` in a Note On payload) is not a valid per-note identity and must not be replaced by channel-wide pitch bend while claiming MPE support.

**Parameter automation**
- `IParameterChanges` supplies queues only for parameters that changed in the current block. VST3 `ParamValue` is normalized to [0,1], unlike AU's declared parameter value domain.
- `IParamValueQueue` is an automation-curve representation: points define linear sections; hosts may omit an implicit point at block position zero; a sustained slope may be represented using the previous-block boundary value; and a discontinuous jump is represented by an old-value point followed by the new value at the next sample.
- Steinberg's documented interpolation example uses relative position `-1` as the previous-block last-sample anchor. This is not a discrete event inside the current block. Keep it in a signed automation-point representation; never cast it to an unsigned `SampleOffset`, clamp it to zero or discard it.
- Convert the queue into piecewise-linear curve segments using the prior parameter value and the ordered points. Do not flatten the queue to one target value and do not add a second host-style ramp on top of the reconstructed curve.

**Transport**
- `ProcessContext` includes project sample time and optional musical position, bar position, cycle range, tempo, time signature and transport flags.
- Optional fields are valid only when their corresponding state flags are set. A null process-context pointer means no context is available; a zero-valued field is not a substitute for a validity flag.
- In VST3 SDK 3.7+, implement `IProcessContextRequirements` and request the fields needed by the product before activation. The host asks once during setup; requirements cannot be changed after activation.
- Steinberg notes that a host may split a block around a cycle jump to provide correct project time inside each part, but is not required to do so. Host-loop behavior inside a block therefore remains a host-validation item, not a universal guarantee.

## 4. Candidate normalized shared-core contract

The following types are illustrative. They refine, rather than approve, the existing IF-08/IF-09 and API catalogue.

### 4.1 Process kind

- `AudioRender`: a real audio block with a valid frame range and audio bus views.
- `ParameterFlush`: a bounded control/parameter update call with no audio render. It has no fabricated frame count, no audio buffers and no transport advancement.
- Offline rendering remains an audio-render mode with an explicit process mode; it is not equivalent to a parameter flush.

### 4.2 Discrete event time

Normalize all discrete MIDI, note-expression, sequencer and step-parameter events to a validated block-relative sample offset in `[0, frames)`.

- AU: compute the signed difference between the event's AU sample time and the render block's valid sample-time anchor; handle immediate scheduling at the AU adapter boundary.
- VST3: use `Event::sampleOffset` directly after validating the range and preserving the host's event-list order.
- Preserve source-local order. For events from one source at the same offset, keep the order supplied by that source.
- Events from different sources at the same offset form one timestamp batch. Do not create a hidden producer-arrival or source-priority rule. Conflicting writes to the same parameter remain governed by AR-01.
- Future, stale, invalid and in-block timestamps must be distinguished. Never silently clamp an event to sample zero or the final sample. Carry a future event only through a bounded adapter-owned mechanism; report/reject stale or invalid events according to the eventual adapter policy.

### 4.3 Parameter automation curve

Use a separate curve type rather than representing every automation point as a discrete `TimedEvent`:

- A curve identifies one canonical parameter and contains a bounded ordered set of signed block-relative points. Both AU and VST3 values are converted through the canonical parameter descriptor so that the shared core does not confuse AU's declared value range/unit with VST3's normalized [0,1] domain.
- The previous-block anchor at relative position `-1` is allowed only as a continuity anchor, not as a current-block event.
- In-block points use `[0, frames)`.
- The curve representation preserves step changes and piecewise-linear sections, the previous value needed to continue a ramp, and the exact endpoint/duration semantics from the source format.
- AU parameter ramps and VST3 parameter queues are translated into the same curve semantics. Parameter-class smoothing remains a separate policy; do not double-ramp or silently downgrade a ramp to a step.
- A plain `TimedEvent::ParameterChange` denotes an instantaneous step only. It is not a substitute for an AU ramp or VST3 automation curve.

### 4.4 Note identity

Introduce an internal `NoteInstanceId` independent of either host's native representation:

- VST3 maps a valid `(eventBusIndex, noteId)` to the internal note instance and retains the host ID for the lifetime of that note.
- AU maps MPE member-channel expression using cable, negotiated MPE zone/member channel and the active note lifecycle. It allocates an internal note instance when a note begins and associates subsequent per-note channel expression with that active instance.
- The AU mapping must detect ambiguous/retriggered or malformed note lifecycles. Its exact same-pitch overlap and note-off matching policy remains an implementation design gate; no arbitrary FIFO/LIFO behavior is approved here.
- If the host does not provide sufficient identity/zone information, report per-note expression as unavailable for that event. Do not substitute channel-wide pitch bend and claim DEC-038 is met.
- Ordinary non-MPE MIDI channel pitch bend remains channel-scoped.

### 4.5 Transport snapshot and validity

The normalized transport snapshot must carry values and explicit validity independently:

- host timeline sample position;
- PPQ position at block start;
- bar-start PPQ;
- cycle start/end PPQ and cycle-active state;
- tempo and time-signature values;
- playing/recording state;
- process sample rate and process kind;
- source/authority and a discontinuity generation or equivalent seek/loop detection state.

An unavailable field remains unavailable. Do not initialize missing tempo to 120 BPM, missing PPQ to zero, or missing playing state to stopped and then present that value as host truth.

**Authority rule remains unchanged:** in AU/VST3, valid host transport is authoritative while supplied. MIDI Clock must not become a second active master in plugin contexts. Standalone has exactly one selected authority (internal clock, MIDI Clock or Ableton Link). The policy when host transport becomes invalid mid-playback remains open; no silent clock-source switch is permitted.

### 4.6 Loop, seek and tempo discontinuities

- Detect block-to-block sample/PPQ discontinuities and publish them explicitly to the shared transport/sequencer.
- Preserve cycle range and validity flags. Do not assume that every host splits a render block at a loop boundary.
- If a host does not provide a reliable intra-block loop transition, the adapter must mark the intra-block musical timeline as insufficient for sample-accurate loop-wrap scheduling. A host-specific workaround requires evidence and must not be inferred from a block-start PPQ value alone.
- Tempo/time-signature values are block-context snapshots unless the host provides a more granular timed representation. Do not invent an intra-block tempo ramp from one block-start value.

## 5. Cross-format parity matrix

| Capability | AU source representation | VST3 source representation | Shared-core obligation | Evidence status |
|---|---|---|---|---|
| MIDI Note On/Off | `AUMIDIEvent`, cable + MIDI bytes + AU sample time | `Event`, block-relative offset + bus/channel + note payload | Same note semantics and validated sample offset | API representation documented; host parity untested |
| Per-note expression | MIDI 1.0 MPE member-channel messages; AU MPE capability flag | Note Expression event with explicit `noteId` | Stable internal note instance; no channel-wide fallback | Format mapping differs; AU lifecycle policy open |
| Parameter step | AU parameter render event, zero ramp duration | Parameter queue point/jump | Exact step offset and canonical parameter mapping | API documented; Q-T12 and AR-01 open |
| Parameter ramp | AU target + ramp duration, subject to rampable flag | Per-parameter piecewise-linear queue | Preserve curve shape, anchor and endpoints | API documented; normalized conversion needs implementation tests |
| Transport | Separate musical-context and transport-state callbacks with independent success/validity | Optional ProcessContext plus per-field flags | One shared snapshot with field-level validity | API documented; host delivery and loop behavior untested |
| Cycle/loop | Cycle start/end beat positions plus current beat/sample position | Cycle range and PPQ/sample position; host may or may not split blocks | Explicit discontinuity; no assumed intra-block wrap | Host-dependent and unverified |
| Parameter flush | No direct equivalent assumed by this audit | May be a process call with no audio buses | Control-only path; no audio/transport advancement | Existing VST3 adapter contract retained |
| Same-offset writer collision | Parameter and MIDI render events may coexist in AU event stream | Parameter queues and event list are separate | Timestamp batch; conflict delegated to AR-01 | Open; no precedence selected |

## 6. Real-time and threading consequences

1. AU `scheduleParameterBlock` may be called from any thread, including a realtime render thread. Its callback ingress must be safe for the documented producer topology. Do not wire it to a single-producer queue by assumption.
2. AU render-event data, VST3 `IEventList`, `IParameterChanges` and `ProcessContext` are callback-scoped host data. Copy only bounded values needed by the engine; never retain host pointers or SDK interface objects after the call.
3. VST3 event list and parameter queues are separate inputs. Merge them with sequencer/device events in preallocated bounded storage; preserve per-source order and group cross-source equal-offset events without inventing precedence.
4. Use bounded validation/merge work. No allocation, unbounded sorting, logging or blocking in the process callback. The exact maximum event/automation point count and overflow recovery remain Phase 5 engineering gates.
5. Transport callbacks/context are read once per process block into one immutable snapshot shared by all four layers. Layers and sequencer do not independently query host transport.
6. A VST3 no-audio-bus parameter flush may update permitted bounded control state but must not touch audio scratch, increment rendered-frame counters, consume sequencer time or advance the shared transport.
7. Host process-context availability and field validity are not concurrency guarantees. Thread ownership and event ordering remain separate concerns.

## 7. Required Phase 5 validation families

These are test-design obligations, not new AC IDs or passing results.

| Test family | Candidate scenarios | Requirements / decisions |
|---|---|---|
| Absolute-to-block event conversion | AU signed sample times; valid timestamp flags; immediate scheduling; negative sample time; block start/end; stale/future events; no clamping | GS-MID-001…008; GS-PLT-003 |
| VST3 event ordering | Event sample offsets, event bus, source-local order, Note On before Note Expression, invalid/missing note ID, block boundaries | GS-MID-001…008; DEC-038 |
| Automation curve equivalence | AU zero-duration step and ramp; non-rampable parameter; VST3 implicit start point, previous-block anchor, linear points, two-point jump, empty queue across constant blocks | GS-MOD-005; GS-SEQ-009; Q-T12/Q-T13 |
| MPE identity | Overlapping notes, same-pitch retrigger, member-channel reuse, Note Off, missing ID/zone, per-note pitch/pressure/timbre and 48-semitone range | GS-MID-002; GS-MOD-004; DEC-038; AR-03…AR-05 |
| Transport validity | Missing/null context, callback failure, independent field validity, tempo/time-signature changes, stop/start, seek, cycle start/end | GS-SEQ-001…012; DEC-039 |
| Intra-block cycle behavior | Host that splits at cycle boundary versus host that does not; compare sequencer event timing and discontinuity reporting | GS-SEQ-001…012; Q-P09 |
| Parameter flush | VST3 zero-bus flush with automation updates; no audio buffer access, rendered-frame advance or transport advance | GS-PLT-003; GS-NFR-001 |
| Same-offset conflict batch | Automation + MIDI mapping + sequencer lock + UI write at same offset; verify deterministic batch capture without asserting unresolved winner | AR-01; GS-MOD-005; GS-SEQ-009 |
| Variable-block parity | Same event/automation/transport fixture across different legal block partitions; compare event sample positions and deterministic output | GS-PLT-003/004; GS-NFR-006 |

Do not create new `AC-*` identifiers in Phase 3. Phase 5 must decide formal AC grouping and pass/fail wording. Event capacities, overflow limits, host versions and numeric timing tolerances remain unapproved.

## 8. Product-consistency check

This audit preserves the approved product baseline:

- DEC-017/019: four layers share one transport and one central sequencer.
- DEC-020/026: live audio remains an R1 requirement in Standalone and plugin contexts; AU and VST3 remain R1 formats.
- DEC-031: shared per-layer macro base values remain distinct from per-note modulation contributions.
- DEC-034: all layers assigned to the same MIDI channel receive matching events.
- DEC-038: MPE per-note pitch range remains 48 semitones.
- DEC-039: MIDI Clock Start/Continue/Stop/timeout semantics apply when MIDI Clock is the selected authority; valid host transport remains authoritative in plugin contexts.
- No product requirement, layer count, engine scope, effect inventory or performance workflow is changed.

## 9. Open gates carried forward

| Gate | Why it remains open | Required next evidence |
|---|---|---|
| AR-01 | No approved precedence when host automation, MIDI mapping, sequencer locks and UI writes collide at the same sample offset | Owner/architecture-approved writer matrix and deterministic conflict tests |
| Q-T12 | AU address, VST3 ParamID and canonical parameter manifest mapping are not finalized | Stable ID mapping and migration/automation round-trip review |
| Q-T13 | Parameter smoothing and update-class interaction with host ramps is not finalized | Parameter-class smoothing specification and curve tests |
| AU MPE note identity | AU's MIDI 1.0 member-channel stream is not a VST3-style noteId | MPE zone/note lifecycle mapping, including retrigger policy |
| Host transport loss | Fallback behavior when host context becomes invalid mid-playback is not approved | Explicit transport-source transition policy and host evidence |
| Intra-block cycle jump | VST3 host may not split a block at cycle boundary; AU/host behavior also requires validation | Exact host/version prototype evidence; do not infer support |
| Q-P09 | Exact host versions/builds remain open | Versioned host certification matrix |
| Event capacity/overflow | No numeric per-block event/automation capacity is approved | Phase 5 workload definition and bounded-overflow tests; no benchmark without authorization |

## 10. Official API evidence register

### Apple
1. [AUScheduleParameterBlock](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/scheduleparameterblock) — safe to call from any thread, including realtime render threads.
2. [AUParameterEvent](https://developer.apple.com/documentation/audiotoolbox/auparameterevent) — sample time, target value and ramp duration.
3. [AUScheduleMIDIEventBlock](https://developer.apple.com/documentation/audiotoolbox/auschedulemidieventblock) — MIDI event sample time, cable and byte payload.
4. [AUMIDIEvent](https://developer.apple.com/documentation/audiotoolbox/aumidievent) — cable, event sample time, length and MIDI bytes.
5. [AUEventSampleTime](https://developer.apple.com/documentation/audiotoolbox/1387633-aueventsampletime) — signed sample-count domain and immediate scheduling semantics.
6. [AUHostMusicalContextBlock](https://developer.apple.com/documentation/audiotoolbox/auhostmusicalcontextblock) — tempo, beat, time signature and sample offset to next beat.
7. [AUHostTransportStateBlock](https://developer.apple.com/documentation/audiotoolbox/auhosttransportstateblock) — transport flags, sample position and cycle range.
8. [AUParameter](https://developer.apple.com/documentation/audiotoolbox/auparameter) — declared min/max, unit and parameter metadata used to normalize AU values.

### Steinberg
1. [IParameterChanges](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IParameterChanges.html) — per-block queues for parameters that changed.
2. [IParamValueQueue](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IParamValueQueue.html) — piecewise-linear automation, implicit points, prior-block anchor and jump encoding.
3. [Event](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1Event.html) — block-relative sample offset, event bus and typed event payload.
4. [NoteExpressionValueEvent](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1NoteExpressionValueEvent.html) — note ID, normalized value and ordering requirement after Note On.
5. [ProcessContext](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1ProcessContext.html) — transport fields and validity flags; cycle-jump block-splitting caveat.
6. [IProcessContextRequirements](https://steinbergmedia.github.io/vst3_doc/vstinterfaces/classSteinberg_1_1Vst_1_1IProcessContextRequirements.html) — request required process-context fields during setup.

**Review outcome:** the API surfaces can be normalized into a shared event/automation/transport model, but host parity is not established by API capability alone. Host prototypes and tests remain separately gated. No code, benchmark or product decision was made.


## 9. MPE note identity and lifecycle refinement — 2026-11-28

The high-level NoteInstanceId contract is expanded in [PHASE3_MPE_NOTE_IDENTITY_AND_LIFECYCLE_AUDIT.md](PHASE3_MPE_NOTE_IDENTITY_AND_LIFECYCLE_AUDIT.md).

Key refinements:
- Separate a source note identity from each layer-local voice NoteInstanceId. One input note may fan out to every layer assigned to the matching MIDI channel (DEC-034).
- A VST3 host key is the event-bus index plus a valid noteId. The value -1 is unavailable, not an identity. Keep the mapping through the voice release because VST3 Note Expression may still address a released voice.
- AU identity is derived from cable, negotiated zone, member channel and note lifecycle. A member channel is an expression scope; if several notes share it, all active notes in that group share its expression.
- A released AU note must not follow a subsequently reused member channel. The candidate is to snapshot its final expression and detach it from the active channel group; same-pitch Note Off matching and sustain interaction remain open.
- VST3 standard/custom Note Expression type mapping for product Pressure and Timbre is not selected. Pitch uses the standard Tuning type as a candidate, with explicit normalized-to-semitone conversion.
- Identity tables, channel groups and layer fan-out must be bounded and preallocated. No render-time map growth or silent fallback to channel-wide expression is allowed.

Open gates are recorded as AR-03 (AU lifecycle/matching), AR-04 (VST3 Pressure/Timbre type mapping) and AR-05 (DEC-038 public range metadata). The audit does not change GS-MID-002, DEC-034 or DEC-038 and adds no AC IDs.

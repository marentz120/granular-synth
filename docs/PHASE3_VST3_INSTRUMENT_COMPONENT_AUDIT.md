# Phase 3 VST3 Instrument Component Audit

**Version:** 0.1 — instrument classification, event/audio buses and host integration  
**Reviewed:** 2026-11-28  
**Status:** Evidence-based architecture proposal only. No VST3 category, bus type, bus activation default, simple-mode flag, framework or host-compatibility claim is approved. No prototype or implementation is authorized.  
**Authority:** DEC-017…DEC-020, DEC-026, DEC-034, DEC-038, GS-ARC-006/007, Q-P06, Q-P09.  
**Related documents:** `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md`, `PHASE3_PLATFORM_AND_PLUGIN_RESEARCH.md`, `PHASE3_HOST_CAPABILITY_EVIDENCE_MATRIX.md`, `PHASE3_API_SYMBOL_TRACEABILITY.md`, `PHASE3_ARCHITECTURE_DECISION_REGISTER.md`.

## 1. Executive finding

VST3 does not define a separate processor interface that makes a plug-in an instrument instead of an effect. Instrument identity is communicated through the plug-in class metadata/category (subcategories), while the processing component implements the VST3 component and audio-processor contracts. The SDK's `AudioEffect` helper class can declare audio input/output buses and event input/output buses; its helper-class name must not be mistaken for the product's host-visible category.

The current VST3 API model can represent the R1 combination of:
- an instrument-classified component;
- MIDI/event input;
- one or more audio input buses;
- one main stereo output and auxiliary stereo outputs.

This is an API-capability finding, **not** proof that Logic Pro, Ableton Live, Bitwig or REAPER will expose and route the exact configuration required by GRANULAR SYNTH.

**Recommendation:** retain one VST3 component classified as an instrument as the first validation candidate. Use a single 16-channel event input bus as the baseline; keep four layers as internal engine layers, not automatically four VST3 Units or four event buses. Declare one full-band live-audio input bus for the first host-validation configuration, but leave its VST3 bus type (`kMain` versus `kAux`) open until host tests prove the raw source-routing behavior. Keep one main stereo master output and four auxiliary stereo layer outputs. Do not claim VST3 simple mode or host certification until validated.

## 2. Instrument identity is class metadata, not a separate processor API

Steinberg's public API separates:
- the processing component: `IComponent` + `IAudioProcessor`;
- the edit controller: `IEditController`.

The host may use the processor without instantiating the edit controller. Therefore audio processing, event handling, state restoration and parameter state must not depend on the editor/controller being present.

The SDK exposes instrument subcategories such as:
- `Instrument`;
- `Instrument|Synth`;
- `Instrument|Sampler`;
- `Instrument|Synth|Sampler`.

The last is a plausible metadata candidate because R1 combines granular synthesis and sample playback, but the exact category string remains a validation choice, not an owner-approved decision. A generic `Instrument` category is a conservative fallback if host categorization is inconsistent.

The SDK's `AudioEffect` helper exposes `addAudioInput`, `addAudioOutput`, `addEventInput` and `addEventOutput`. These declarations do not force the host-visible category to be an insert effect. Conversely, assigning an instrument subcategory does not guarantee that a host will expose every declared audio input bus.

### 2.1 Avoid an unproven dual Fx/Instrument category

Steinberg documents `Fx|Instrument` as a way for sequencers to offer a plug-in as both an effect and an instrument. The same FAQ records a historical caveat: Steinberg sequencers up to version 5.5 did not enable audio input buses when the plug-in was loaded as an instrument. This is not evidence about current target-host behavior, but it demonstrates why a combined category must not be used as a shortcut around host testing.

For R1, prefer a single instrument identity unless a tested host workflow establishes a separate, genuinely required insert-effect identity. A dual category may create two insertion paths and different bus behavior without solving live-input routing.

## 3. Candidate VST3 bus topology

The following is a validation baseline, not a final SDK declaration:

| Direction | Media | Candidate bus | Purpose | Status |
|---|---|---|---|---|
| Input | Event | One main event bus, 16 MIDI channels | Notes, controllers, channel routing and host event timing | Proposed baseline |
| Input | Audio | One stereo full-band input bus | Raw live-audio source for the layer source router | Required behavior; bus type open |
| Output | Audio | One main stereo bus | Master mix | Required |
| Output | Audio | Four stereo auxiliary buses | Layer 1–4 stems | Required semantic outputs; activation host-dependent |
| Output | Event | None | MIDI/sequencer output is not an R1 commitment | Out of R1 |

### 3.1 Event bus and four-layer routing

A single 16-channel event input bus is sufficient as the first candidate for the product's MIDI channel model. The adapter translates host events with their sample offsets and channel/note context; the shared core routes them to the configured layers.

This preserves DEC-034: if multiple layers are assigned to the same MIDI channel, every matching layer receives the event. It does **not** require one event bus per layer. The four product layers are internal sound engines and output stems; they are not automatically four VST3 Units, four MIDI ports or four independently addressable host instruments.

The exact host-visible layer-to-track association remains open. Do not implement `IUnitInfo`, multiple event buses or a one-to-one input/output routing claim merely because the product has four layers. Add those facilities only if an approved host workflow requires them and a truthful mapping can be defined.

### 3.2 Full-band audio input

The host input must deliver raw audio samples to the live-source path, not merely a detector envelope, sidechain level or modulation signal.

Steinberg distinguishes main and auxiliary audio buses. An auxiliary bus is commonly used for sidechain input, but the API does not make every auxiliary input a detector-only signal. The product's required behavior is defined by the samples that reach the source router, not by the bus label.

Keep the bus type (`kMain` or `kAux`) open. Test both viable arrangements where the host allows them and record:
- whether the bus appears in the instrument insertion workflow;
- whether the user can route an arbitrary audio track/device input to it;
- whether unmodified mono/stereo samples reach the engine at the expected sample offsets;
- whether the bus survives project save/reopen and bus deactivation/reactivation;
- whether it coexists with MIDI note input and all five output pairs.

A one-stereo-input configuration is a minimum validation case, not a product-level limit. DEC-020 does not impose a fixed product cap on physical input sources. The same physical input view may be shared read-only by several layer source paths without duplicating the host buffer.

### 3.3 Outputs and activation

Use one main stereo output for the complete master mix and four auxiliary stereo outputs for the layer stems. This matches GS-ARC-006 and the signal semantics in `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md`.

Important host contract:
- `kDefaultActive` is a request, not a guarantee.
- The host activates/deactivates buses through `IComponent::activateBus`.
- A plug-in UI may request activation through `IComponentHandlerBusActivation`, but the host may accept or reject the request.
- ProcessData may omit trailing inactive buses; inactive sample pointers may be null. Resolve each bus from the per-call counts and activation state, not from the declared topology alone.

If one or more layer outputs are unavailable, the layer must still contribute exactly once to the master. Never double-sum the layer stems into the master as a fallback. The product's parallel dry live-input path must also remain summed to the master as required by GS-ARC-007.

## 4. Simple mode and multi-timbral integration

Steinberg's documented VST3 simple mode is intended for a simple instrument track with one input and one output and uses MIDI channel 0. GRANULAR SYNTH's candidate topology has an event input, an audio input, five audio output buses, multiple MIDI channels and MPE. Do not set or claim `kSimpleModeSupported` merely to improve instrument-track discovery.

The target-host matrix currently names Logic Pro, Ableton Live, Bitwig and REAPER, not Cubase. Simple-mode behavior should be recorded as a separate compatibility question if the host matrix later adds Cubase/Nuendo or if a target host requires it. Do not reduce the R1 bus or MIDI model to fit a mode whose product trade-offs have not been approved.

VST3 Units and multi-timbral routing metadata solve host integration problems when a host needs to associate MIDI tracks/units with particular outputs. R1 does not yet require each of the four layers to appear as a separate host-visible instrument. Because DEC-034 permits one incoming channel to trigger multiple layers, an input-to-output relation may be one-to-many rather than unique. Do not publish misleading `getRoutingInfo` or Unit associations. Validate whether the four named auxiliary outputs alone provide the required DAW mixing workflow.

## 5. MPE and per-note expression

The VST3 MIDI documentation distinguishes ordinary MIDI channel messages from MPE/per-note expression:
- MIDI 1.0 channel pitch bend is channel-scoped and is not, by itself, per-note pitch bend.
- Steinberg's VST3 MIDI mapping table identifies MPE through Note Expression.
- A per-note expression event references a note ID and is delivered in the process event list.
- `INoteExpressionController` describes the supported per-note expression types; `INoteExpressionPhysicalUIMapping` exposes physical mappings to the host.
- Steinberg's wrapper MPE interface is a separate optional mechanism for wrappers that translate MPE input to VST3 Note Expression; it must not be confused with the direct-host plug-in contract.

For DEC-038, the VST3 adapter must preserve per-note identity and translate pitch expression to the product's ±48-semitone range. The exact normalized-value mapping and supported event sequence must be specified and tested. Do not claim the 48-semitone product range is satisfied merely because ordinary channel pitch bend is received.

Host validation must include overlapping notes on the same channel, per-note pitch changes, note-off/retrigger ordering, missing/invalid note IDs, and a host that does not provide the required Note Expression/MPE translation. Unsupported per-note expression must be reported as a host capability limitation, not silently converted to channel-wide pitch bend.

## 6. Processor/controller separation and state

- The processor must initialize and process correctly even when the edit controller is not instantiated.
- The processor owns real-time engine interaction; the controller owns UI-facing parameter metadata, gestures and view lifecycle.
- Processor/controller communication uses the VST3 connection/message mechanism or another explicitly bounded non-RT bridge; no raw mutable DSP object is exposed to the editor.
- State must preserve DEC-025: unsaved recording buffers are embedded; library samples remain external references with relink/collect.
- Offline state callbacks may occur on a processing thread according to the existing VST3 callback audit. State snapshots must therefore be immutable/consistent and must not introduce disk I/O, unbounded waits or unsafe concurrent mutation.

## 7. Required host validation matrix

For each target VST3 host and exact version selected under Q-P09:

| Test | Required evidence |
|---|---|
| Discovery/category | Plug-in appears as an instrument, not only as an insert effect; record category, class ID and insertion workflow |
| Processor without controller | Audio/event processing remains valid without editor/controller instantiation |
| Event input | One 16-channel event bus receives Note On/Off, velocity, CC, channel pressure and pitch bend with correct sample offsets |
| Same-channel layer fan-out | Two or more layers assigned to one MIDI channel all receive the event exactly once |
| MPE | Note Expression pitch reaches the correct note ID with ±48-semitone range; no channel-wide substitution |
| Full-band input | Arbitrary routed audio samples reach the layer source path, not only a detector/sidechain control path |
| Five outputs | Main master plus four independent layer stems; verify active/inactive and save/reopen behavior |
| Master fallback | Inactive/missing layer outputs do not remove or double-add layer audio in the master |
| Bus lifecycle | Default-active request ignored; individual bus activation; middle inactive bus; omitted trailing buses; reactivation |
| Simple mode | Record whether the host invokes simple mode and whether it is intentionally unsupported; do not claim support without a complete mapping |
| State and automation | Session save/reopen, embedded recordings, external sample relink, stable parameter IDs and automation |
| Offline/process flush | Zero-audio-bus parameter flush does not advance transport or dereference audio buffers; offline state behavior is safe |

Bitwig is VST3-only in the current proposed matrix; do not create a Bitwig AU test cell. A host documentation entry is not a pass. Only a reproducible host/format test can certify a cell.

## 8. Product consistency

| Approved decision/requirement | VST3 treatment |
|---|---|
| DEC-017/018 — four layers, Granular + Sampler | Four internal layers; no automatic requirement for four VST3 Units |
| DEC-019 — central sequencer | One shared sequencer; no MIDI output implied |
| DEC-020 — live input per layer in plug-in contexts | Full-band host input must reach per-layer source routing |
| DEC-026 — AU + VST3 R1 | This audit does not change R1 formats |
| DEC-034 — same MIDI channel may feed multiple layers | One event bus plus internal fan-out is the baseline candidate |
| DEC-038 — MPE per-note pitch ±48 semitones | Note identity and per-note expression must be preserved |
| GS-ARC-006 — master + four layer outputs | One main stereo output plus four auxiliary stereo outputs |
| GS-ARC-007 — parallel dry live input to master | Preserve the dry path and prevent duplicate summing |
| DEC-025 — state persistence | Same embedded-recording/external-library-reference policy |

No product decision is changed or weakened by this audit.

## 9. Open decisions and blockers

| Item | Current status |
|---|---|
| Exact VST3 subcategory string | Candidate `Instrument|Synth|Sampler`; validate host discovery before selection |
| Live-input audio bus type | `kMain` versus `kAux` remains open; validate raw full-band routing |
| Default activation of four layer outputs | `kDefaultActive` is only a request; host behavior unverified |
| VST3 simple mode | Not claimed; target-host impact not tested |
| VST3 Units / host MIDI-track association | Not required by current product baseline; reconsider only with a concrete host workflow |
| MPE Note Expression host coverage | API path documented; exact target-host delivery and ±48 mapping unverified |
| Exact host versions and release-blocking cells | Q-P09 remains open |

No owner decision is required to continue documentation. If host evidence later reveals that the required live-input or MPE workflow cannot be delivered, present the exact host/format conflict to the owner; do not change DEC-020 or DEC-038.

## 10. Official evidence register

Checked 2026-11-28:

1. VST 3 API architecture — processor and edit-controller separation:  
   https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html
2. AudioEffect helper — audio and event bus declaration methods:  
   https://steinbergmedia.github.io/vst3_doc/vstsdk/classSteinberg_1_1Vst_1_1AudioEffect.html
3. VST3 plug-in subcategories — Instrument, Synth, Sampler, Synth|Sampler:  
   https://steinbergmedia.github.io/vst3_doc/vstinterfaces/group__plugType.html
4. Multiple dynamic I/O and bus activation:  
   https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.0.0/Multiple%2BDynamic%2BIO.html
5. Complex/multi-timbral structures, simple mode, Units and routing information:  
   https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Complex%2BStructures/Index.html
6. Steinberg FAQ — Fx|Instrument category and historical instrument-mode input-bus caveat:  
   https://steinbergmedia.github.io/vst3_dev_portal/pages/FAQ/Miscellaneous.html
7. VST3 Note Expression:  
   https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.5.0/INoteExpressionController.html
8. VST3 MIDI mapping and MPE:  
   https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/About%2BMIDI/Index.html
9. Note Expression physical UI mapping:  
   https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.6.11/INoteExpressionPhysicalUIMapping.html
10. Wrapper MPE support (wrapper-specific; not a substitute for direct-host tests):  
    https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/Change%2BHistory/3.6.12/IVst3WrapperMPESupport.html
11. VST3 NoteExpressionValueEvent and noteId semantics:  
    https://steinbergmedia.github.io/vst3_doc/vstinterfaces/structSteinberg_1_1Vst_1_1NoteExpressionValueEvent.html

**Conclusion:** VST3's API model is capable of expressing the candidate instrument + live audio input + five-output topology. Host routing, bus activation, MPE delivery and the exact insertion workflow remain unproven. This audit recommends one instrument-classified component as the first validation candidate; it does not approve a category string, bus type, simple-mode flag, host compatibility claim or implementation.

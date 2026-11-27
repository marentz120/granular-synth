# Phase 2 Release Scope Matrix

**Status:** Phase 2 working matrix — existing approved scope distinguished from proposed interpretation.  
**Authority:** R1_PRODUCT_SPECIFICATION.md, DECISIONS_LOG.md DEC-015…021, GRANULAR_SYNTH_REQUIREMENTS.md.  
**Rule:** This matrix does not promote any later/optional requirement to R1. “Approved” means explicitly established by owner decisions/product specification; “Requirement baseline” means present in the current requirements inventory but may still need owner confirmation where the tier is labelled proposed.

## 1. Approved product scope

| Domain | R1 approved scope | Decision/source |
|---|---|---|
| Layer model | Exactly four independent layers | DEC-017 |
| Engines | Each layer selects Granular or Sampler; Slice deferred | DEC-017/018 |
| Live audio | Live input path per layer | DEC-020; R1 Product Spec §3 |
| Freeze | Per-layer Freeze and global Freeze; semantics deferred | DEC-020; GS-SMG-021 |
| Recording | Per-layer recording and stereo master-mix recording; retain/export captured audio | DEC-020 |
| Sequencer | One central sequencer controlling all four layers; up to eight events per step | DEC-019 |
| Performance sets | Patterns, chaining and ordered Performance Sets | DEC-019; R1 Product Spec §4 |
| Sync | Internal BPM, MIDI Clock, Ableton Link and DAW/host transport | DEC-019 |
| MIDI live operation | Generic class-compliant controller; essential operations and sequencer editing controller-reachable | DEC-016/019 |
| MIDI feedback | Not required in R1 | DEC-021 |
| Named controller templates | Not required in R1 | DEC-021 |

## 2. Domain scope

| Domain | R1 requirement baseline | Later / optional / unresolved |
|---|---|---|
| Modulation | Four LFOs/layer; matrix ≥50 rows; approved sources and R1 destinations; 10 macros; envelope follower; random sources; modulation visualization | Draw XY L3; CV optional/later; patch randomizer L2; Poly/Mono LFO scope Q-M11; macro 11–20 unresolved and not committed |
| Effects | Four reorderable slots/layer; Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator, Compressor; dry/wet; live-input and synth sends; automation/modulation; tail linger/cut | Large Reverb, Resonator, Flanger, Vibrato, Ring Mod, Reducer, Distortion L2; chain/single-FX presets L2; duplicate effect types OPT; master FX L3 |
| Sampler | One playback head/voice; pitch-as-speed; spray/pan spray/direction/scan; one-shot/loop/ping-pong; looped release; high-quality interpolation; env-pile-up option | 60-minute samples L2; loop crossfade/zero-crossing snap L2; SFZ velocity-layer multisample L3; looped-release semantics unresolved |
| Presets/Library | Browser with banks/tags/search/favourites; default template; templates; versioned state; sample portability/relink/collect; plugin state restore; Undo/Redo; copy/paste; patch comments as current proposed requirement | GR import optional/legal gate; sample embedding/reference Q-P08 Phase 3; hierarchy Q-P07 open; serialization architecture Phase 3 |
| MIDI | Per-layer channels; MPE; MIDI Learn/manual mapping; 14-bit CC/NRPN input; soft takeover; Program Change recall; channel-mode; MIDI Clock in Standalone; controller-only live operation | Default CC map compatibility L2; sequencer MIDI out/clock out L3; MIDI feedback and named templates excluded by DEC-021; MPE zone/channel details open |
| Audio | Standalone + plugin; live input; per-layer/global Freeze; per-layer + stereo master recording; sample import/export; layer outputs; real-time safety | Threshold/transport arming L2; clocked record L2; resampling L2; synth-to-record feedback L2; exact buses/devices/resource ceilings Phase 3/5 |

## 3. Tier discipline

- A requirement row marked R1 in GRANULAR_SYNTH_REQUIREMENTS.md is the current R1 requirements baseline, but any row explicitly described as proposed or unresolved remains subject to review.
- Owner decisions DEC-015…021 and R1_PRODUCT_SPECIFICATION.md govern where older source documents conflict.
- No product-level maximum is introduced for patterns, chains or performance sets where the approved spec says no arbitrary cap; technical limits must be measured and safe.
- No fixed max voice/grain/audio buffer resource budget is set by this matrix.
- This matrix does not settle Freeze semantics, parameter IDs, host automation exposure, framework, plugin buses or persistence serialization.

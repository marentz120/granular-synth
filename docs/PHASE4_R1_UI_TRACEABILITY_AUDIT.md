# GRANULAR SYNTH — R1 UI Traceability & Coverage Audit

**Version:** 0.2 — reconciliation with Phase 4 controller catalogue, consolidated layouts and accessibility audit, 2026-11-28  
**Status:** Phase 4 discovery audit — proposed UI mapping, owner review required  
**Inputs:** `GRANULAR_SYNTH_REQUIREMENTS.md`, `R1_PRODUCT_SPECIFICATION.md`, `LIVE_PERFORMANCE_SPECIFICATION.md`, `UI_UX_SPECIFICATION.md`, `MIDI_MAPPING_UX_SPECIFICATION.md`, `SCREEN_STATE_AND_NAVIGATION_SPECIFICATION.md`, and `PHASE3_API_SYMBOL_TRACEABILITY.md`.

## 1. Purpose and limits

This audit maps every R1 requirement ID in the Phase 3 API traceability inventory to a likely user-facing surface or to an internal/indirect category. It does not change requirement wording, release tier, candidate API symbols, IF contracts, acceptance criteria or technical decisions.

**Important distinction:** a UI surface mapping is not proof that a requirement is implemented, accepted, or fully specified. Requirements classified as internal/non-UI still need engineering and test coverage. The audit intentionally does not invent acceptance IDs.

## 2. Coverage inventory

The source Phase 3 traceability inventory contains **124 unique R1 requirement IDs**. Each ID is listed below exactly once.

| Requirement | Proposed UI surface / handling | Classification |
|---|---|---|
| GS-PLT-001 | Indirect UI / release diagnostics | Platform/host configuration or package metadata; not a primary instrument control surface. |
| GS-PLT-002 | Indirect UI / release diagnostics | Platform/host configuration or package metadata; not a primary instrument control surface. |
| GS-PLT-003 | Indirect UI / release diagnostics | Platform/host configuration or package metadata; not a primary instrument control surface. |
| GS-PLT-004 | Indirect UI / release diagnostics | Platform/host configuration or package metadata; not a primary instrument control surface. |
| GS-ARC-001 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-ARC-002 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-ARC-003 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-ARC-004 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-ARC-005 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-ARC-006 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-ARC-007 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-ARC-008 | Layer header / Mix; internal constraints not exposed as arbitrary controls | Layer configuration, engine selection and routing; internal voice/grain budgets remain implementation-managed. |
| GS-GRN-001 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-002 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-003 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-004 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-005 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-006 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-007 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-008 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-009 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-010 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-011 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-012 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-013 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-014 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-015 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-016 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-017 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-018 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-019 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-GRN-020 | Granular workspace / contextual parameter pages | Granular engine parameters, source position, pitch, density, scan, window and grain behavior. |
| GS-CHD-001 | Granular/Sampler performance and pitch subpanels | Chord, arpeggiator, scale and chord-recording controls. |
| GS-CHD-002 | Granular/Sampler performance and pitch subpanels | Chord, arpeggiator, scale and chord-recording controls. |
| GS-CHD-003 | Granular/Sampler performance and pitch subpanels | Chord, arpeggiator, scale and chord-recording controls. |
| GS-CHD-004 | Granular/Sampler performance and pitch subpanels | Chord, arpeggiator, scale and chord-recording controls. |
| GS-CHD-005 | Granular/Sampler performance and pitch subpanels | Chord, arpeggiator, scale and chord-recording controls. |
| GS-SMP-001 | Sampler workspace | Sampler playback head, loop, direction, interpolation and Env Pile-up. |
| GS-SMP-002 | Sampler workspace | Sampler playback head, loop, direction, interpolation and Env Pile-up. |
| GS-SMP-003 | Sampler workspace | Sampler playback head, loop, direction, interpolation and Env Pile-up. |
| GS-SMP-004 | Sampler workspace | Sampler playback head, loop, direction, interpolation and Env Pile-up. |
| GS-VOI-001 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-002 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-003 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-004 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-005 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-006 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-008 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-009 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-VOI-010 | Voice/Envelope/Filter contextual panels; MIDI performance controls | Voice, sustain, glide, velocity, pitch bend, envelopes, filters and sub-oscillator. |
| GS-SMG-001 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-002 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-003 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-004 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-005 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-006 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-007 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-008 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-009 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-010 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-011 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-012 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-017 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-019 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-020 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-SMG-021 | Granular/Sampler workspace; Library/Recording/Freeze controls | Sample import/browser, waveform editing, flags, recording, live input, export, Freeze and buffer status. |
| GS-MOD-001 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-002 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-003 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-004 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-005 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-006 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-007 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-008 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-MOD-009 | Modulation workspace | LFOs, modulation sources/destinations, assignment, matrix rows, macros and modulation monitor. |
| GS-SEQ-001 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-002 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-003 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-004 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-005 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-008 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-009 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-011 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-SEQ-012 | Fixed 16-step strip + expanded Sequencer + Performance controls | Central sequencer, event/parameter lanes, timing, patterns, transport, Fill, Mute, drone and Undo/Redo. |
| GS-MID-001 | MIDI Mapping / Settings + performance-accessible controls | MIDI channel, MPE, MIDI Learn, Program Change, channel-mode, clock and controller-only operation. |
| GS-MID-002 | MIDI Mapping / Settings + performance-accessible controls | MIDI channel, MPE, MIDI Learn, Program Change, channel-mode, clock and controller-only operation. |
| GS-MID-003 | MIDI Mapping / Settings + performance-accessible controls | MIDI channel, MPE, MIDI Learn, Program Change, channel-mode, clock and controller-only operation. |
| GS-MID-005 | MIDI Mapping / Settings + performance-accessible controls | MIDI channel, MPE, MIDI Learn, Program Change, channel-mode, clock and controller-only operation. |
| GS-MID-006 | MIDI Mapping / Settings + performance-accessible controls | MIDI channel, MPE, MIDI Learn, Program Change, channel-mode, clock and controller-only operation. |
| GS-MID-007 | MIDI Mapping / Settings + performance-accessible controls | MIDI channel, MPE, MIDI Learn, Program Change, channel-mode, clock and controller-only operation. |
| GS-MID-008 | MIDI Mapping / Settings + performance-accessible controls | MIDI channel, MPE, MIDI Learn, Program Change, channel-mode, clock and controller-only operation. |
| GS-FX-001 | FX workspace | FX slots/order/sends/mix, effect catalog, parameter automation and tail behavior. |
| GS-FX-002 | FX workspace | FX slots/order/sends/mix, effect catalog, parameter automation and tail behavior. |
| GS-FX-004 | FX workspace | FX slots/order/sends/mix, effect catalog, parameter automation and tail behavior. |
| GS-FX-006 | FX workspace | FX slots/order/sends/mix, effect catalog, parameter automation and tail behavior. |
| GS-PRS-001 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-002 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-003 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-004 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-005 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-006 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-007 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-008 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-PRS-009 | Preset/Library/State management | Preset/state, library, templates, collect/relink, undo/redo, clipboard and comments. |
| GS-UI-001 | Global shell / Granular workspace / MIDI Mapping | Waveform display, engine control bank, window shape, editor scaling, metrics and MIDI Learn conflict UI. |
| GS-UI-002 | Global shell / Granular workspace / MIDI Mapping | Waveform display, engine control bank, window shape, editor scaling, metrics and MIDI Learn conflict UI. |
| GS-UI-003 | Global shell / Granular workspace / MIDI Mapping | Waveform display, engine control bank, window shape, editor scaling, metrics and MIDI Learn conflict UI. |
| GS-UI-004 | Global shell / Granular workspace / MIDI Mapping | Waveform display, engine control bank, window shape, editor scaling, metrics and MIDI Learn conflict UI. |
| GS-UI-005 | Global shell / Granular workspace / MIDI Mapping | Waveform display, engine control bank, window shape, editor scaling, metrics and MIDI Learn conflict UI. |
| GS-UI-006 | Global shell / Granular workspace / MIDI Mapping | Waveform display, engine control bank, window shape, editor scaling, metrics and MIDI Learn conflict UI. |
| GS-UI-007 | Global shell / Granular workspace / MIDI Mapping | Waveform display, engine control bank, window shape, editor scaling, metrics and MIDI Learn conflict UI. |
| GS-NFR-001 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-002 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-003 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-004 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-005 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-006 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-007 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-008 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-009 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-010 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-011 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-012 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |
| GS-NFR-013 | Mostly internal/non-UI; expose status/diagnostics only where useful | Real-time safety, CPU/load, smoothing, latency, determinism, memory, progressive loading and validation. |

## 3. Coverage interpretation by area

| Area | UI treatment | Main review risk |
|---|---|---|
| Platform (PLT) | Configuration and release diagnostics; most constraints are not live instrument controls. | Do not expose unresolved host/build decisions as user-selectable options. |
| Architecture (ARC) | Four-layer selection, per-layer engine choice and mix/routing presentation. Voice/grain resource policy remains internal. | Avoid exposing unapproved numeric budgets or unsafe resource controls. |
| Granular (GRN) | Main Granular workspace and contextual parameter panels. | Full parameter inventory must remain reachable without overcrowding the main view. |
| Chord (CHD) | Chord/arp/scale subpanels and performance mappings. | Ensure chord recording and quantization are reachable during performance when essential. |
| Sampler (SMP) | Dedicated Sampler workspace. | Preserve exact Loop Release behavior and default Env Pile-up OFF. |
| Voice (VOI) | Contextual voice, envelope, filter and sub-osc panels; MIDI controls. | Some functions are per-voice and need clear scope indication. |
| Sample management (SMG) | Library, waveform editor, recording and Freeze controls. | Keep import/editing distinct from live performance; communicate pending/failure states. |
| Modulation (MOD) | Modulation workspace with matrix, LFO, macro and monitor views. | Show shared macro base values separately from per-voice modulation. |
| Sequencer (SEQ) | Fixed 16-step strip plus expanded editor and performance surface. | 16 visible steps must not imply a 16-step Pattern cap; all editing must be MIDI-controller reachable. |
| MIDI (MID) | MIDI Mapping workspace plus performance-accessible actions. | No R1 MIDI feedback or named-controller template; do not hide essential operations behind touch-only UI. |
| Effects (FX) | Four ordered slots per layer in FX workspace. | R1 catalog only; do not imply deferred effects are included. |
| Presets/state (PRS) | Preset browser, library, state and recovery UI. | Preset recall must not replace the active Pattern; unsaved recordings and external sample references need clear status. |
| UI (UI) | Global shell, waveform/editor and mapping UX. | Screen scaling and shared editor behavior still require design and platform validation. |
| NFR | Mostly internal; expose actionable status and diagnostics, not raw engineering controls. | Avoid implying that a visual indicator proves real-time safety or benchmark compliance. |

## 4. Cross-cutting UI requirements to verify

- **Layer focus:** selecting L1–L4 changes editing focus only; it must not mute, solo, stop or redirect another layer.
- **Engine scope:** Granular/Sampler selection is per layer; changing one layer must not overwrite another layer's state.
- **Sequencer:** one central sequencer; up to eight events per step; variable Pattern length/resolution; bar-end switching and edit application; explicit Save, Draft, Undo and Redo.
- **Performance:** transport, Fill, Mute, Freeze, Panic, Pattern navigation and all other essential live operations must remain controller-reachable.
- **MIDI:** mappings need visible scope, conflict handling, soft-takeover state and recovery; R1 feedback is not required.
- **Freeze:** show pending, active and failure states consistent with DEC-042; recording continues and Panic does not thaw.
- **Recording:** distinguish per-layer recording from stereo master recording and show source/record-head state.
- **Preset/Pattern separation:** changing Preset must not replace or stop the active Pattern.
- **State safety:** destructive actions need undo or confirmation; stale references and missing samples must be visible and safe.
- **Accessibility:** no critical state communicated by color alone; no essential operation hidden behind a gesture or long press.

## 5. Gaps and decisions carried forward

The original Phase 4 discovery gaps have been reconciled against the current v0.2 controller catalogue, encoder proposal, consolidated layouts and accessibility audit. The following are now **documented at proposal level**, but remain subject to owner review and/or technical validation:

1. **Encoder matrix and A–H bank:** proposed Granular and Sampler assignments are documented in `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md` §2.5. The controller catalogue deliberately retains an approval boundary: assignments must be checked against the authoritative R1 parameter inventories and approved by the owner. Stable parameter IDs, scaling and taper remain Phase 3/Q-T12 gates.
2. **16-step viewport:** paging/window behavior and playhead-follow proposal are documented in `PHASE4_ENCODER_SEQUENCER_PERFORMANCE_UX.md` and `PHASE4_CONSOLIDATED_SCREEN_LAYOUTS.md`; owner review remains open.
3. **Persistent performance strip:** hierarchy and placement are proposed in the consolidated layouts; owner review remains open.
4. **Full parameter reachability:** six workspace layouts and the controller target catalogue provide the current proposal. Final parameter exposure must be reconciled with the authoritative domain specifications; no deferred/L2 control may be promoted into R1.
5. **MIDI action mapping:** named action classes, explicit scopes and controller-only workflows are documented in `PHASE4_CONTROLLER_ACTION_AND_TARGET_CATALOG.md`; protocol, stable IDs, capacity, normalization, hot-plug and recovery implementation remain technical gates.
6. **MPE UI:** detailed per-note expression visualization remains a design/validation gap. Host-specific ingress and identity behavior remain Phase 3/5 technical work.
7. **Sample and recording lifecycle:** Library, waveform, recording and Freeze surfaces plus explicit Arm/Start/Stop proposal are documented. Concurrency, resource policy and final lifecycle acceptance remain open.
8. **Host/plugin layout:** resizability, display constraints and host-specific behavior remain dependent on unresolved Phase 3 format and hardware decisions.

These items must not be interpreted as owner-approved, implemented or tested. The action-by-action status and owner review list in `PHASE4_PERFORMANCE_ACCESSIBILITY_AUDIT.md` is the current operational summary.
 
## 6. Acceptance traceability boundary

The Phase 3 API traceability file records 124 R1 IDs, of which 49 have a direct acceptance-criteria reference and four more have a cross-domain release-gate link; 71 have neither. This UI audit does not close that Phase 5 gap. Each UI-facing requirement must eventually receive a testable interaction/acceptance path, while non-UI requirements need explicit internal test coverage or a documented rationale.

## 7. Review checklist

- [ ] Confirm each requirement's UI surface or non-UI classification.
- [ ] Check every essential live operation against GS-MID-008 and LIVE_PERFORMANCE_SPECIFICATION.md.
- [ ] Complete encoder mapping and sequencer viewport decisions.
- [ ] Reconcile detailed control inventory against the domain specifications.
- [ ] Add acceptance links in Phase 5; do not fabricate them in Phase 4.

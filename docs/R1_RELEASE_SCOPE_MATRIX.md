# R1 Release Scope Matrix — Phase 2

**Status:** Phase 2 proposed matrix; owner-approved scope is distinguished from pending proposals.
**Baseline:** DEC-015…DEC-021 and R1_PRODUCT_SPECIFICATION.md.
**Rule:** This matrix classifies requirements; it does not approve a new tier or authorize implementation.

| Domain | R1 — required by current requirements/product decisions | Later / optional / not R1 | Pending approval or gate |
|---|---|---|---|
| Modulation | Four LFOs per layer; modulation matrix ≥50 rows; R1 source set for existing features; eligible R1 parameter destinations; quick assignment; row copy/paste/delete; 10 macros; summed modulation display/recent-destination monitor; envelope follower; Random 1–4; XY pad source; sequencer modulation lanes | Drawn trajectories L3; CV input optional/deferred; patch randomizer L2; macros 11–20 not R1 | Q-M11 LFO Poly/Mono behavior; exact parameter domains and host exposure Phase 3 |
| Effects | Four reorderable serial slots per layer; per-slot dry/wet; synth and audio-input sends; modulation/automation of eligible effect parameters; linger/cut tail behavior | Large Reverb, Resonator, Flanger, Vibrato, Ring Mod, Reducer, Distortion L2; chain/single-FX presets L2; duplicate effect type OPT; master bus FX L3 | Q-P11 seven-effect R1 set is PROPOSED, not owner-approved |
| Sampler | Sampler engine in each layer; one playhead per voice; pitch = playback speed; spray/pan spray/direction/scan; one-shot/loop/ping-pong; looped release; high-quality interpolation option; env pile-up option | 60-minute long samples L2; loop crossfade/zero-crossing snap L2; SFZ velocity multisampling L3 | Q-M05 looped-release semantics OPEN; exact interpolation/default and edge behavior need confirmation |
| Presets / Library | Sample/Patch/Multi/Project logical hierarchy; browser with banks/tags/search/favourites; init template; sample-free templates; versioned state; relative sample references/hash/relink/collect; full DAW state restore including recorded buffers; undo/redo; copy/paste; comments | GR-1/GR-MEGA import OPT and blocked by Q-L01; fixed benchmark slot limits explicitly excluded | Q-P07 searchable library + performance-bank view PROPOSED; Q-P08 unsaved-recording embed/reference policy PROPOSED; Q-T15 serialization Phase 3 |
| MIDI | Per-layer channels; MPE; MIDI Learn for eligible parameters; 14-bit CC/NRPN; soft takeover; Program Change recall; channel-mode messages; Standalone MIDI Clock input; generic controller-only operation for all essential live performance actions | Benchmark CC map compatibility L2; sequencer MIDI note/clock output L3; named controller templates and MIDI feedback explicitly not R1 | MPE zone details, PC mapping, clock loss and host MIDI input Phase 3/4 |
| Audio / I/O | Standalone device I/O; per-layer live input; per-layer/global Freeze controls; per-layer recording and stereo master recording; one-shot/loop; AUTO/fixed buffers; scrolling record; play/process while recording; WAV/AIFF/FLAC load; WAV export; main stereo + four stereo layer outputs at product level | Input threshold/transport arm L2; clocked record L2; resampling L2; synth-to-record feedback L2; master FX L3 | Freeze semantics, AU/VST3 buses, signal tap points, sample-rate and resource budgets Phase 3/5 |

## Tier discipline

- **Approved product decisions:** DEC-015…DEC-021 are binding and are not reopened by this matrix.
- **Existing R1 rows:** Requirements labelled R1 in GRANULAR_SYNTH_REQUIREMENTS.md remain the current scope baseline unless explicitly marked proposed/unapproved in the source documents.
- **Pending product approvals:** Q-P07 and Q-P11 remain PROPOSED. Q-P08 is a proposed persistence policy with Phase 3 technical consequences.
- **Technical proposals:** Q-T01…Q-T17 are not approved architecture decisions. Numeric performance limits, formats, component/bus layout and serialization are not settled by this matrix.
- **Benchmark parity is not scope authority:** a feature in GR_MEGA_FEATURE_MAP.md is not R1 unless it is explicitly represented in the GRANULAR SYNTH requirements or approved decisions.

## R1 exclusions preserved

- Granular Slice, Tape and Spectral engines are not R1 (DEC-018).
- No R1 MIDI feedback or named-controller templates (DEC-021).
- No arbitrary fixed product-level caps on pattern count, chain length or performance sets; technical safety limits must be measured and tested.
- No proprietary firmware, source, graphics, factory content or undocumented file-format replication.
- Freeze semantics remain a Phase 3 architecture decision.
- No DSP implementation begins in Phase 2.

## Exit review checklist

- [ ] Every GS requirement row has one explicit tier and traceable source.
- [ ] All proposed product decisions are visibly separated from approved scope.
- [ ] Requirement totals reconcile with the source inventory.
- [ ] No row contradicts DEC-015…DEC-021 or R1_PRODUCT_SPECIFICATION.md.
- [ ] Owner approves pending product decisions and the final matrix.

# R1 Release Scope Matrix — Phase 2

**Status:** Phase 2 release-tier matrix approved under DEC-041; technical gates and later tiers are distinguished.
**Baseline:** DEC-015…DEC-042 and R1_PRODUCT_SPECIFICATION.md.
**Rule:** This matrix classifies requirements; it does not approve a new tier or authorize implementation.

| Domain | R1 — required by current requirements/product decisions | Later / optional / not R1 | Pending approval or gate |
|---|---|---|---|
| Modulation | Four LFOs per layer; per-LFO Poly/Mono (Mono default); matrix ≥50 rows; R1 sources/destinations; quick assignment; row copy/paste/delete; 10 macros; summed modulation display; envelope follower; Random 1–4; normalized XY pad; sequencer modulation lanes | Drawn trajectories L3; CV input optional/deferred; patch randomizer L2; macros 11–20 not R1 | Parameter domains, modulation evaluation and host exposure Phase 3 |
| Effects | Four reorderable serial slots per layer; per-slot dry/wet; synth and audio-input sends; modulation/automation of eligible effect parameters; linger/cut tail behavior; seven approved effects per DEC-023 | Large Reverb, Resonator, Flanger, Vibrato, Ring Mod, Reducer, Distortion L2; chain/single-FX presets L2; duplicate effect type OPT; master bus FX L3 | Exact parameter ranges/tapers and effect algorithms Phase 3 |
| Sampler | Sampler engine per layer; one playhead per voice; pitch = playback speed; spray/pan spray/direction/scan; one-shot/loop/ping-pong; Loop Release ON/OFF per DEC-030; high-quality interpolation option; Env Pile-up toggle default OFF per DEC-037 | 60-minute long samples L2; loop crossfade/zero-crossing snap L2; SFZ velocity multisampling L3 | Interpolation choices/default and detailed scan math Phase 3 |
| Presets / Library | Searchable desktop library; banks/tags/search/favourites; init and sample-free templates; versioned state; relative sample references/hash/relink/collect; embed unsaved recording buffers in plugin state; undo/redo; copy/paste; comments; independent Pattern Library | GR-1/GR-MEGA import OPT and blocked by Q-L01; fixed benchmark slot limits explicitly excluded | Serialization, migration and state-size implementation Phase 3 |
| MIDI | Per-layer channels; shared-channel events trigger all matching layers; MPE; MIDI Learn; 14-bit CC/NRPN; soft takeover; Program Change explicit performance-bank mapping; channel-mode messages; Standalone MIDI Clock input; controller-only operation | Benchmark CC map compatibility L2; sequencer MIDI note/clock output L3; named controller templates and MIDI feedback explicitly not R1 | MPE zone negotiation, NRPN parser details and plugin MIDI input Phase 3; mapping UI Phase 4 |
| Audio / I/O | Standalone device I/O; per-layer live input; per-layer/global Source Snapshot Freeze; per-layer recording and stereo master recording; one-shot/loop; AUTO/fixed buffers; scrolling record; play/process while recording; WAV/AIFF/FLAC load; WAV export; main stereo + four stereo layer outputs | Input threshold/transport arm L2; clocked record L2; resampling L2; synth-to-record feedback L2; master FX L3 | Freeze product semantics approved by DEC-042; AU/VST3 buses, signal tap points, sample-rate and resource budgets Phase 3/5 |

## Tier discipline

- **Approved product decisions:** DEC-015…DEC-040 define the product behavior, DEC-041 approves all release-tier placements, and DEC-042 approves Source Snapshot Freeze behavior. These decisions are binding and are not reopened by this matrix.
- **Approved tiers versus technical proposals:** DEC-041 approves each requirement's release tier. Any numeric value or implementation detail explicitly tied to Q-Txx remains a proposal until its Phase 3/5 technical gate.
- **Approved Phase 2 product decisions:** Q-P02, Q-P07, Q-P08, Q-P11, Q-P15, Q-P16, Q-M01, Q-M05, Q-M06, Q-M11 and Q-M12 are resolved by DEC-022…DEC-037; MIDI and mono-input behavior are further settled by DEC-038…DEC-040. Technical serialization, host-state implementation, parameter models and measured resource limits remain later-phase gates.
- **Technical proposals:** Q-T01…Q-T17 are not approved architecture decisions. Numeric performance limits, formats, component/bus layout and serialization are not settled by this matrix.
- **Benchmark parity is not scope authority:** a feature in GR_MEGA_FEATURE_MAP.md is not R1 unless it is explicitly represented in the GRANULAR SYNTH requirements or approved decisions.

## R1 exclusions preserved

- Granular Slice, Tape and Spectral engines are not R1 (DEC-018).
- No R1 MIDI feedback or named-controller templates (DEC-021).
- No arbitrary fixed product-level caps on pattern count, chain length or performance sets; technical safety limits must be measured and tested.
- No proprietary firmware, source, graphics, factory content or undocumented file-format replication.
- Freeze product semantics are approved by DEC-042; implementation, memory admission, crossfade and host validation remain Phase 3/5 engineering gates.
- No DSP implementation begins in Phase 2.

## Exit review checklist

- [x] Every GS requirement row has one explicit primary tier; no "Pending" tier remains. Optional rows without a benchmark source are explicitly marked OPT.
- [x] Approved product decisions and remaining technical/evidence questions are visibly separated.
- [x] Requirement totals reconcile with the 168-row source inventory (124 R1 / 27 L2 / 9 L3 / 8 OPT).
- [x] The R1 scope matrix is reconciled with DEC-015…DEC-042 and R1_PRODUCT_SPECIFICATION.md.
- [ ] Owner approval/merge of the Phase 2 PR remains an owner gate.

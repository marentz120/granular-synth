# Open Questions

**Version:** 0.7 (Phase 3 state I/O scheduling addendum, 2026-11-28). Product decisions delegated by the owner to ChatGPT as product manager are recorded as Approved in DECISIONS_LOG.md; technical, legal and evidence questions remain open until their relevant gate.

**Change summary (v0.5 → v0.6):** Q-P02, Q-P07, Q-P08, Q-P11, Q-P15, Q-P16, Q-M01, Q-M05, Q-M06, Q-M11 and Q-M12 were resolved or tiered under the owner's explicit product-management delegation and recorded in DEC-022…DEC-037. Q-P18 is answered by DEC-018; MIDI and audio product behaviors were further settled by DEC-038…DEC-040; DEC-041 approves all 168 release-tier assignments. Six domain specifications, R1 Release Scope Matrix and R1 Acceptance Criteria are included. Architecture, numeric feasibility, remaining evidence and legal questions stay open for their proper gates.
**Change summary (v0.6 → v0.7):** Added Q-T15 state I/O scheduling, capture-watermark, retention-lease, callback-context, cancellation and aggregate peak-admission evidence gates. No product decision or requirement tier changed.
**Owners of answers:** **Owner** = product owner (human, final authority) · **CGPT** = ChatGPT (technical decision authority) · **Research** = Claude (investigation task)

Status values: `OPEN` · `PROPOSED` (recommendation given, awaiting approval) · `RESOLVED-EVIDENCE` (settled by documentary evidence, no owner decision needed) · `ANSWERED` (move to DECISIONS_LOG.md).

## v0.1 checklist → v0.2 tracking

Every item from the v0.1 list is kept and mapped:

| v0.1 item | v0.2 ID | State |
|---|---|---|
| Owner: Relationship to GR-MEGA (parity / inspired-by / baseline + differentiation) | Q-P15 | **ANSWERED — DEC-027** |
| Owner: Confirm formats | Q-P02 | **ANSWERED — DEC-026** |
| Owner: Layer model (4 fixed, more, configurable) | Q-P04 | **ANSWERED — DEC-017** |
| Owner: First-release engines | Q-P20 | **ANSWERED — DEC-018** |
| Owner: Approve framework after comparison | Q-T01 (comparison in DSP_ARCHITECTURE.md §2) | OPEN |
| Owner: Minimum macOS / Apple Silicon–Intel | Q-T03 | OPEN |
| Owner: Main use case (studio / live / both) | Q-P19 | **ANSWERED** — both (DEC-015) |
| Research: Inspect manual 1.8.d with page/section refs | — | **Done** (GR_MEGA_FEATURE_MAP.md) |
| Research: Manual vs firmware version differences | Q-I15 | **Partly done** — release notes 1.8.0–1.8.4 reviewed (feature map §16); roadmap 1.9–1.12 not reviewed |
| Research: Verify numeric limits against manual | Q-M01…M19 | **Done**; 15 manual conflicts logged |
| Research: Map performance, MIDI and automation workflows | — | **Done** (feature map §11) |
| Research: Legal and product-differentiation boundaries | §F | OPEN — legal advice recommended |
| Technical: Sample rates, buffer sizes, latency | Q-T17, Q-T10 | OPEN — proposal given |
| Technical: CPU/memory budgets and baseline hardware | Q-P10, Q-T07 | OPEN |
| Technical: Sample formats, max asset size, streaming | Q-T05, Q-P16, GS-SMG-001 | OPEN — proposal given |
| Technical: Preset/project portability; embedded vs linked samples | Q-P07, Q-P08 | OPEN — proposal given |
| Technical: Host compatibility, signing/distribution | Q-P09, Q-P03 | OPEN |

---

## 0. Product-scope decisions resolved for R1

The owner explicitly delegated remaining open product decisions to ChatGPT as product manager on 2026-11-27, with the constraint that no decision may contradict an already-approved owner decision. The following product questions are now answered and recorded in DECISIONS_LOG.md:

| ID | Resolution | Decision |
|---|---|---|
| Q-P04 | **ANSWERED** | Exactly four independent layers in R1, each selectable as Granular or Sampler (DEC-017/018). |
| Q-P05 | **ANSWERED** | Full central performance-oriented step sequencer in R1 (DEC-019), not L2. |
| Q-P06 | **ANSWERED** | Live input to each layer in Standalone and plug-in contexts; format-specific AU/VST3 implementation remains Phase 3 work (DEC-020). |
| Q-P12 | **ANSWERED** | Generic class-compliant MIDI controller support, MIDI Learn + manual CC/channel/Note mapping; no named-controller template or MIDI feedback in R1 (DEC-021). |
| Q-P20 | **ANSWERED** | Granular + Sampler in R1; Granular Slice deferred; Tape later (L2), Spectral later (L3) (DEC-018). |

These answers establish the product baseline; they do **not** close Phase 2, approve a framework, set measured performance budgets, or authorize implementation.

### Remaining gates

- **Phase 2:** complete the domain specifications, R1 requirement/tier matrix, traceability and product-level acceptance criteria. Q-P07, Q-P08 and Q-P11 are now answered; technical format/host decisions remain Phase 3.
- **Phase 3:** resolve Q-T01…Q-T17 (framework, macOS targets, DSP/resource budgets, parameter model, sample handling and plug-in integration), Q-P09/Q-P10 host/reference-machine details, and the format-specific implementation of live input. Freeze product semantics are resolved by DEC-042; technical resource and RT implementation remain open.
- **Evidence/research:** Q-P13 (physical GR-MEGA access), Q-I15 (firmware roadmap 1.9–1.12), remaining manual ambiguities and undocumented behaviors.
- **Legal/release:** Q-L01…Q-L07, product naming and distribution questions before release.

## A. Product decisions (Owner)

| ID | Question | Context / options | Recommendation | Blocks | Status |
|---|---|---|---|---|---|
| Q-P01 | Final product name and visual identity. Is "GRANULAR SYNTH" a working title? | Must avoid confusion with Tasty Chips trademarks/trade dress (see Q-L03) | Keep as working title; brand decision before Phase 8 | Phase 8 | OPEN |
| Q-P02 | Plugin formats | AU + VST3 (stated); CLAP (cheap with modern frameworks); AAX (Pro Tools; extra SDK, signing, cost); AUv3 (sandbox, iPad path) | AU + VST3 in R1; CLAP optional, not R1 | Phase 3 | **ANSWERED — DEC-026** (component/bus implementation Phase 3) |
| Q-P03 | Commercial product or personal/internal tool? Distribution channel? Copy protection? | Affects licence tier (JUCE), notarization, installer, licensing system | Decide before Phase 6 | Phase 6 | OPEN |
| Q-P04 | Layer model and R1 layer count | 4 fixed layers · more layers · configurable count | Exactly four independent Granular layers in R1 | Phase 2–3 | **ANSWERED — DEC-017** |
| Q-P05 | Step sequencer tier | R1 vs L2; standalone live performance must not depend on a DAW | Full central performance-oriented step sequencer in R1 | Phase 2 | **ANSWERED — DEC-019** |
| Q-P06 | Live-input granulation in plugins | Live input per layer is a core product workflow; AU/VST3 component and bus implementation is host-specific | Yes: live input per layer in Standalone and plug-in contexts; exact AU/VST3 implementation is Phase 3 | Phase 3 | **ANSWERED — DEC-020** |
| Q-P07 | Library model | (a) Benchmark-style fixed slots: 50 projects × 128 presets × 128 multis; (b) desktop file library with browser, tags, favourites, plus an optional performance-bank view for MIDI Program Change | Searchable desktop library with tags/favourites; performance banks index library items and are not a fixed storage cap | Phase 2/4 | **ANSWERED — DEC-022** |
| Q-P08 | Recorded/edited samples in plugin state | (a) always embed audio in DAW project; (b) reference files + "collect" on save; (c) embed only unsaved recordings | Embed unsaved recording buffers in plugin state; reference library files and support relink/collect | Phase 3 | **ANSWERED — DEC-025** (serialization/size policy remains Phase 3) |
| Q-P09 | Host certification list | Logic Pro, Ableton Live, Bitwig, Reaper, Cubase, Studio One, MainStage? | Logic, Live, Bitwig, Reaper for R1 | Phase 7 | OPEN |
| Q-P10 | Reference machine and minimum supported Mac | Defines CPU budget and acceptance tests | Reference: Apple Silicon M1 (base) as minimum performance target; also test latest M-series | Phase 5 | OPEN |
| Q-P11 | R1 effect set | Proposal: Delay, PP Delay, Reverb, Chorus, EQ3, Saturator, Compressor | Seven-effect set approved for R1; advanced set remains L2 | Phase 2 | **ANSWERED — DEC-023** |
| Q-P12 | Dedicated hardware-controller support (a *specific* controller template/target)? | Generic controller operation is required by DEC-016/021; a named template is optional | Generic class-compliant MIDI controller, MIDI Learn + manual CC/channel/Note mapping; no named template or feedback in R1 | Phase 4 | **ANSWERED — DEC-021** |
| Q-P13 | GR-MEGA unit access for A/B | Needed only to verify residual benchmark ambiguities; product scope does not depend on parity | If physical access becomes available, use it for optional follow-up verification; not a Phase 2/R1 blocker | Post-Phase 2 research | DEFERRED — access not confirmed |
| Q-P14 | Factory content | Who produces samples/presets; licensing of samples | Decide before Phase 8 | Phase 8 | OPEN |
| Q-P15 | Fidelity policy | Where the benchmark has a *limitation* (linear interpolation in granular, window crackle, 128-grain bunching, crackling auto-slice, 8192-FFT mono), do we replicate or improve? | Improve by default; preserve artifacts only when deliberately selectable and musically useful | Phase 2 | **ANSWERED — DEC-027** |
| Q-P16 | Granular sample-length ceiling | Benchmark: 5m49s efficient limit | R1 product target: at least 6 minutes stereo per layer at 48 kHz; feasibility and memory profile validated in Phase 3/5 | Phase 2 | **ANSWERED — DEC-028** |
| Q-P17 | Document naming | Earlier chat drafts used `GR_MEGA_FEATURE_MAP.md` / `DSP_ARCHITECTURE.md`; the repository uses `GR_MEGA_FEATURE_MAP.md` / `DSP_ARCHITECTURE.md` | Repository names are canonical; drafts merged into them | Admin | RESOLVED-EVIDENCE |
| Q-P18 | Engine scope after R1 | Order Tape (L2) before Spectral (L3)? | Tape L2 before Spectral L3 | Phase 2 | **ANSWERED — DEC-018** |
| Q-P19 | Main use case: studio sound design, live performance, or both? | Drives priority of sequencer, MIDI-controller templates, standalone features, CPU headroom targets | **ANSWERED (DEC-015, 2026-11-27):** both, and the instrument must be a genuine performance tool, not merely a plugin wrapper. Raises priority of Q-P05 (sequencer reassessment, docs/LIVE_PERFORMANCE_SPECIFICATION.md) and establishes DEC-016 (controller-only operability); does not itself decide Q-P12's specific-controller question | Phase 2 | **ANSWERED** |
| Q-P20 | First-release engines | Granular-only · Granular + Sampler · broader engine set | Granular + Sampler in R1; Granular Slice deferred; Tape L2, Spectral L3 | Phase 2 | **ANSWERED — DEC-018** |
| Q-P21 | R1 Freeze behavior | Source snapshot · grain/voice hold · post-FX latch · full engine pause | Source Snapshot: immutable current source region; voices/notes/transport/modulation/FX continue; independent recording continues; thaw crossfades back; global applies to all four layers; static samples report already static; state persists under DEC-025 | Phase 3 | **ANSWERED — DEC-042** |

---

## B. Technical decisions (CGPT review → Owner approval)

| ID | Decision | Options | Recommendation | Trade-off summary | Status |
|---|---|---|---|---|---|
| Q-T01 | Language/framework | C++/JUCE 8 · C++/iPlug2 · raw SDKs · Rust/nih-plug | C++20 + JUCE 8 | Mature AU/VST3/standalone support and community vs. licence cost above revenue cap | PROPOSED |
| Q-T02 | Core independence | DSP core on JUCE types vs framework-free | Framework-free | Slightly more glue code; big gain in testability and portability | PROPOSED |
| Q-T03 | macOS minimum / architectures | 11, 12, 13+ ; Universal 2 vs arm64-only | macOS 12+, Universal 2 | Workgroup APIs need 11+; Intel support costs test time | OPEN |
| Q-T04 | Sample-rate handling | A: source rate + compensated increment · B: SRC to host rate on load · C: fixed 48 kHz engine | A | Memory/latency-friendly; needs good interpolation | PROPOSED |
| Q-T05 | Long samples | RAM only · RAM + streaming (sampler) | RAM for granular; streaming for 60-min sampler in L2 | Streaming adds a disk thread and complexity | PROPOSED |
| Q-T06 | Grain window implementation | Analytic per sample · table bank + per-grain snapshot | Table bank + snapshot | Memory for tables vs CPU for pow/cos | PROPOSED |
| Q-T07 | Global grain budget value | Fixed number vs adaptive (CPU-load feedback) | Measure on reference machine; start fixed, consider adaptive later | Adaptive is non-deterministic → harder to test | OPEN |
| Q-T08 | Multithreaded rendering | Single RT thread · RT workers joined to host audio workgroup | Architect for it, enable post-R1 profiling | Complexity, host variance | PROPOSED |
| Q-T09 | RT-safety tooling | Allocation interposer only · + RTSan (Clang) | Both in CI | Toolchain constraints | PROPOSED |
| Q-T10 | Spectral latency reporting | Always report · report only when live input is processed · never | Only when processing live input | Reporting latency on an instrument shifts MIDI-driven output in some hosts | OPEN |
| Q-T11 | PV analysis window & overlap | Hann 4× · Hann 8× · per-FFT-size | Hann 4× baseline; 8× for quality mode | CPU vs smearing | OPEN |
| Q-T12 | Parameter ID scheme and host exposure | Expose all (~1,000+) · curated subset | Expose all, stable string IDs, grouped per layer, macros first | Long automation lists vs. no "missing parameter" complaints. **Irreversible after first release.** | PROPOSED |
| Q-T13 | Control-rate size for modulation | 8 / 16 / 32 / 64 samples | 16 (≤ 0.36 ms @ 44.1 kHz) | CPU vs modulation resolution; audio-class targets are interpolated anyway | PROPOSED |
| Q-T14 | Interpolation default for granular | Linear (benchmark) · cubic Hermite · sinc | Cubic Hermite default, selectable | ~2× cost vs benchmark | PROPOSED |
| Q-T15 | Preset/state container and migration | Binary · JSON manifest + binary chunks · host-native opaque state | Versioned JSON manifest + binary audio chunks; external library references; embed unsaved recording/Freeze payloads in DAW document state per DEC-025/042. Exact payload encoding, compression, host mapping, user-preset scope and migration policy remain open. | Transparency vs. payload size, host limits and compatibility | PROPOSED |
| Q-T16 | Audio file decoding | AudioToolbox only · libsndfile/dr_libs | AudioToolbox primary, portable fallback | Platform lock-in vs. dependency | PROPOSED |
| Q-T17 | Supported sample rates, buffer sizes, latency (REQ-AUD-005) | 44.1–48 kHz only · 44.1–96 · 44.1–192 kHz; buffers 32–2048 · 16–4096 | 44.1–192 kHz, 16–4096 frames, variable block size; 0 added latency except spectral | Wider range = more test matrix | PROPOSED |

---

## C. Manual ambiguities & inconsistencies (resolve by A/B test, Tasty Chips release notes, or owner ruling)

| ID | Item | Manual evidence | Proposed resolution | Status |
|---|---|---|---|---|
| Q-M01 | Grain size minimum | 5 ms (p.39) vs 0.1 ms (p.142) | R1 product range 0.1–5000 ms; parameter taper/scaling Phase 3 | **ANSWERED — DEC-035** |
| Q-M02 | Granular sample length | 5m49s (p.74, p.171) vs 3 min (p.162) | Product target ≥6 min stereo at 48 kHz; benchmark discrepancy does not change our requirement | **ANSWERED — DEC-028** |
| Q-M03 | Ratio modes: TimeStretch existence & ScanRate at scan = 0 | 6 modes on p.40 vs 5 enums p.143/147; division by scan undefined | Implement 6; at |scan| < ε hold last valid rate/size | PROPOSED |
| Q-M04 | Scan modes count / "kill" | "four modes" lists 3 (p.58); spec "Bounce, wrap, kill" (p.162) | One-shot = kill/stop; implement 3 modes; verify on unit | OPEN |
| Q-M05 | Looped release ON/OFF semantics | p.58–59 vs p.102–103 contradict | GRANULAR SYNTH behavior: ON continues looping through voice release; OFF exits at the next valid loop boundary and continues toward Stop. This is our product behavior, not a verified benchmark claim. | **ANSWERED — DEC-030** |
| Q-M06 | Number of macros | 10 (p.23) vs 20 NRPNs (p.150) | Ten macros are R1; 20 are not required in R1 | **ANSWERED — DEC-036** |
| Q-M07 | Mod source count (24/25/26) | p.78, p.157–158, p.163 | Use enumerated source table (25 + None) as reference | **RESOLVED-EVIDENCE** |
| Q-M08 | Mod destination count (140/149/150) | p.23, p.76, p.78, p.158–161 | Use enumerated destination table (149) as benchmark reference; GRANULAR SYNTH implements only destinations for R1 parameters | **RESOLVED-EVIDENCE** |
| Q-M09 | Grain-count normalization law | Only "max volume at density 17" (p.33) | Energy-preserving 1/√density beyond d0; match level at density 17 by measurement | OPEN |
| Q-M10 | Tape chord: tracking head = first or lowest note | p.48 "TBV" | Lowest note (consistent with p.53 rule "lowest chord note matches the played note") | PROPOSED |
| Q-M11 | LFO scope: per voice or per layer | p.79 "each layer has its own LFOs"; key sync Legato | Per-LFO Poly/Mono mode; default Mono (shared per-layer phase), Poly uses per-voice phase | **ANSWERED — DEC-024** |
| Q-M12 | Patch spray / patch bias semantics | p.19 "grain counts boosted based on which part of the screen you focus on" | Deferred to L2; benchmark semantics remain unverified and are not an R1 dependency | **ANSWERED — DEC-029** (tier decision only; behavior research may continue for L2) |
| Q-M13 | Spectral C+Shift: Amp smear vs FFT reducer | p.37 vs p.49, p.148 | Firmware 1.8.0 release notes: FFT reducer merged into amp threshold → C+Shift = Amp smear; no separate reducer needed | RESOLVED-EVIDENCE |
| Q-M14 | Grain key trigger values (On/Off vs Off/On/Legato) | p.54 vs p.135 | Implement Off/On/Legato | PROPOSED |
| Q-M15 | Sequencer "Rate sets time signature" | p.22 vs p.134 | Treat as rate division; time signature not modelled per layer | PROPOSED |
| Q-M16 | "Voice sync" vs "Clock retrigger" | p.55 text vs p.54 screenshot | Release notes 1.8.0-a19: "voice retrigger" renamed "voice sync" (screenshot is from 1.8.0-a14). Very likely the same control; confirm on unit | PROPOSED |
| Q-M17 | Spawn modes 3 vs 4 | p.55 vs p.162 | Implement 4 | PROPOSED |
| Q-M18 | Envelope minimum 0 vs 0.1 ms | p.143 vs p.163 | 0.1 ms minimum (click-safe) | PROPOSED |
| Q-M19 | "Sampler slice mode possible" | p.162 only | Treat as Slice engine with sampler-style (single head) playback option — L2 | OPEN |

---

## D. Undocumented or under-documented items (Research tasks)

| ID | Item | Evidence | Investigation plan | Status |
|---|---|---|---|---|
| Q-I01 | Master EQ (5 bands) | NRPN 300–370; RN 1.8.0 (Q ≤ 18, gain ≤ 24 dB) | Signal-path position still unknown; videos / unit | OPEN |
| Q-I02 | Envelope loop modes | NRPN 160/165/170/175 | Same | OPEN |
| Q-I03 | Grain probability | NRPN 1125; RN 1.8.0 (Shift + knob A) | Control location known; exact behavior to verify | OPEN |
| Q-I04 | Drawing modulator (Draw X/Y) | p.80; NRPN 1100–1104, 1122–1124; RN 1.8.0 | Confirmed as touchscreen "Draw XY" trajectory modulator with waypoints; detailed semantics still to verify. Natural fit for mouse/trackpad in software | PROPOSED |
| Q-I05 | Front-panel "mod pages" 1–4 | NRPN 340–347, 397–400, 1135–1144 | Same | OPEN |
| Q-I06 | Window parameter 3 | NRPN 217 | Same | OPEN |
| Q-I07 | Filter mode enum (SVF modes), routing, bypass | NRPN 289, 293, 391; RN 1.8.0 (classic SVF notch) | Full mode list still unknown | OPEN |
| Q-I08 | Pan-spray distribution shapes | p.37; NRPN 505 | Same | OPEN |
| Q-I09 | Auto-slice detection algorithm | p.46 | Our own design (spectral-flux onset detector) — no investigation needed beyond A/B feel | PROPOSED |
| Q-I10 | Gran filter analysis (centroid map) | p.56 | Our own design: per-window spectral centroid map computed on load | PROPOSED |
| Q-I11 | Full scale list incl. microtonal | p.54 | Use standard scale set + Scala import | PROPOSED |
| Q-I12 | Sub-oscillator waveform | — | Unit / videos; default sine | OPEN |
| Q-I13 | Phase-vocoder hop/overlap/window | — | Our own design (Q-T11) | PROPOSED |
| Q-I14 | Resonator scale list ("inharmonic e.g. metal rods") | p.86 | Physical-model mode tables (bars, plates) — our design | PROPOSED |
| Q-I15 | Firmware changes and roadmap | Release notes 1.8.0–1.8.4 reviewed → feature map §16. Roadmap 1.9–1.12 (Dec 2026–Jul 2027) not yet reviewed | Review roadmap at Phase 2 start | OPEN |
| Q-I16 | Voice-stealing keyboard-position heuristic | p.120 | Our own design (protect lowest/highest held notes) | PROPOSED |
| Q-I17 | "Pos Y" modulation source | RN 1.8.4 mentions POS X and POS Y; manual table lists only Pos X | Verify; our XY pad covers both | OPEN |
| Q-I18 | Sequencer preset switching ("Elektron-style", System → preset change) | RN 1.8.0 only | Capture semantics before Phase 4/5 acceptance design for the R1 sequencer | OPEN |

---

## E. Technical risks (summary — detail in DSP_ARCHITECTURE.md §15)

| ID | Risk | Severity | Mitigation owner |
|---|---|---|---|
| R-01 | Worst-case grain load > single core | High | CGPT / Claude (Phase 3 benchmark) |
| R-02 | Parameter-ID scheme instability | High (irreversible) | CGPT (Q-T12) |
| R-03 | Plugin state size with audio | Medium | Owner (Q-P08) |
| R-04 | AU live-input host behavior | Medium | Claude research (Q-P06) |
| R-05 | Spectral engine CPU/latency | Medium (deferred to L3) | CGPT |
| R-06 | Click artefacts under modulation | High (quality) | Claude (architecture), tests |
| R-07 | Memory with 4 long samples | Medium | Q-T05 |
| R-08 | Undocumented benchmark behavior → user expectation mismatch | Medium | Owner (Q-P13, Q-P15) |
| R-09 | Framework licence | Low–Medium | Owner (Q-L04) |

---

## F. Legal / IP (Owner; legal advice recommended)

| ID | Question | Notes | Status |
|---|---|---|---|
| Q-L01 | May we import GR-1 / GR-MEGA patch files? | Formats are undocumented; implementing an importer requires analysing their files. Interoperability may be lawful in many jurisdictions but needs advice. Not in R1. | OPEN |
| Q-L02 | May we adopt the GR-MEGA MIDI CC numbering for compatibility? | A CC map is a functional interface; still confirm. | OPEN |
| Q-L03 | Naming / marketing references to "GR-MEGA", "Tasty Chips" | Avoid implying affiliation; comparative statements only with legal review. | OPEN |
| Q-L04 | JUCE 8 EULA tier and obligations | Starter tier has a revenue/funding cap (currently quoted at $20k/yr); paid tiers above. Review full EULA text. | OPEN |
| Q-L05 | Third-party library licences (SRC, decoders, FFT fallback, test frameworks) | Prefer BSD/MIT/zlib; avoid GPL in shipped binary unless the product is GPL. | OPEN |
| Q-L06 | Reference implementations of published algorithms (e.g. RTPGHI in GPL toolboxes) | Implement from papers, not from GPL code, unless licence-compatible. | OPEN |
| Q-L07 | Manual-derived documentation | Our docs paraphrase the manual with page references for internal use; do not redistribute manual text or screenshots. | PROPOSED |

## G. Phase 3 MPE reset and state-restore gate

| ID | Question | Dependency / evidence | Status |
|---|---|---|---|
| AR-06 | When state restore commits while notes/voices are active, should old voices terminate or finish as non-addressable tails under the old immutable runtime generation? | Owner decision; then host tests for held notes, release tails, VST3 noteId/member-channel reuse and controller synchronization | OPEN — owner decision required |

See PHASE3_MPE_RESET_STATE_RESTORE_AND_HOST_LIFECYCLE_AUDIT.md. CC 121/123/120 remain distinct; no new product requirement or default controller value is approved.

*End of OPEN_QUESTIONS.md*

## H. Q-T15 state I/O scheduling and memory-admission evidence — 2026-11-28

The State I/O Scheduling Audit adds engineering evidence gates without changing DEC-025, DEC-042 or product scope:

- Define and verify the per-source committed-frame watermark used by a save snapshot; a stale prepared descriptor must not be reported as a successful DAW document save.
- Verify retention leases/ring-wrap behavior while recording continues into reserved writable capacity; lease exhaustion must fail capture without stopping or corrupting the recording.
- Record actual AU/VST3 callback thread/context in normal and offline modes; no render-thread acknowledgement wait, disk I/O or unbounded capture is permitted.
- Observe AU dictionary Data materialization and VST3 IBStream short-transfer behavior only in an owner-authorized host prototype.
- Define bounded in-flight save/restore jobs and aggregate physical-memory/retention admission; exact capacities and numeric budgets remain unapproved.
- Verify cancellation releases leases only after all serializer/copy readers are quiescent.
- Preserve the distinction between required DAW document payloads and unresolved user-preset payload scope.

See `PHASE3_STATE_IO_SCHEDULING_AND_PEAK_MEMORY_ADMISSION_AUDIT.md`. These are Q-T15/Q-P09 engineering gates, not new owner-approved product behavior. AR-06 and the mixed-layer Global Freeze preparation-failure policy remain open; no decision is inferred here.

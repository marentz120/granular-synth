# Phase 1 Reference Summary — GR-MEGA Manual 1.8.d

**Issue:** #1 · **Status:** For ChatGPT review and owner approval · **Evidence base:** GR-MEGA User Manual 1.8.d (179 pp., firmware 1.8.x) and the official firmware 1.8.0–1.8.4 release notes. Details and page references: GR_MEGA_FEATURE_MAP.md.

## 1. Instrument model

The GR-MEGA is a **sample-based, 4-layer multitimbral workstation**. Every sound comes from a sample (p.111). Its data model is:

**Sample → Template** (every sound parameter) → **Patch/Preset** (sample + template) → **Mtpreset** (up to 4 layers + layer volumes) → **Project** (128 presets, 128 mtpresets, tempo). A project is one of 50 slots (p.111–113).

| Dimension | Value | Ref |
|---|---|---|
| Layers | 4, each on its own MIDI channel, or all under MPE | p.13, p.92, p.162 |
| Voices | 1–20 per layer; 3 in Spectral | p.52, p.120 |
| Grains | ≤128 per voice; ≈10,000 total (5,000 on pre-Oct-2025 hardware) | p.39, p.162 |
| Internal format | 48 kHz, 32-bit float mix | p.162 |
| Signal flow | Voice: engine + sub-osc → LPF → HPF. Layer: 4 FX → M-S → patch vol. Then layer mix → wet + dry input → master | p.32 |
| Modulation | 4 LFOs per layer; 4 ADSR envelopes (pitch, filter, amp, aux); 50-row matrix (25 sources, 149 destinations); 10 macros; 4 per-note random sources; envelope followers | p.18–24, p.78–80, p.157–161 |
| Effects | 4 slots per layer from 14 types; sends for synth and audio input | p.81–86 |
| Sequencer | One per layer: 64 steps, up to 7 notes per step, 4 mod lanes, 16 parameter-lock lanes, ties | p.75–77 |

## 2. Engines

| Engine | Essence | Distinguishing behavior |
|---|---|---|
| **Granular** (primary) | Many overlapping windowed grains. Rate 0.1–1000/s; size 5–5000 ms (MIDI table says 0.1 ms–5 s); density = rate × size | 6 ratio modes (Free, DensitySize, DensityRate, ScanRate, ScanOverlap, TimeStretch); 2 window families (PowAR, raised cosine with AM); spawn modes; click-free grain stealing; chord notes distributed across grains with a 10-mode arp; spectral-content-guided spawning ("Gran filter") |
| **Granular Slice** | Granular playback confined to slices | Slices mapped chromatically from C-2; manual/auto slicing; per-slice filter; position slider and Scan inactive |
| **Sampler** | One play head per voice (≤12 with chord); pitch = speed | Loop flags confine playback (one-shot / wrap / bounce); looped release; polyphase interpolation; samples up to ~1 h |
| **Tape** | Play head chases the target position with slew | Speed (and pitch) derived from head motion → scratching; chord heads are pitch-bent copies |
| **Spectral** | STFT phase vocoder, FFT 256–8192 | Time-stretch/freeze, threshold, phase/amp smear, bend (2 algorithms), fold, pitch blend, formant lock/shift; ≤3 voices; chord ×12 at almost no cost; optional high-quality phase-gradient (RTPGHI) mode |

All engines share one 8-knob control layout. Scan and Pitch mean the same everywhere; the other knobs remap per engine (p.37–38). See feature map §7 for the full engine comparison.

## 3. Core workflows

1. **Sample → patch:** load or record → find a position with the slider → shape grains → save (p.123–126).
2. **Live granular effect:** loop- or scrolling-record the input while granulating it, using dedicated input/wet/dry levels (p.73, p.127–129).
3. **Slicing and drum sequencing:** auto- or manual-slice, then trigger slices from the sequencer (p.44–46, p.75).
4. **Performance:** layer mix faders, drone (play) mode per layer, preset/bar switching while the sequencer runs, macros on faders, hold-and-turn modulation assignment, chord recording (p.16–24, p.135–137).
5. **Multitimbral set-up:** 4 layers in an mtpreset, each with its own FX chain, MIDI channel and sequencer (p.92, p.111).

## 4. Technical implications for GRANULAR SYNTH

| # | Implication | Where addressed |
|---|---|---|
| T1 | **CPU is the dominant risk.** The benchmark worst case is ≈490 M grain-samples/s, more than one core can handle. We need a global grain budget with deterministic, click-free stealing, quality tiers, and possibly workgroup-joined worker threads | DSP_ARCHITECTURE.md §6.10; Q-T07, Q-T08 |
| T2 | Granular reads sample memory at random → **RAM-resident samples** (≈134 MB for 5m49s stereo per layer); streaming only for long Sampler files | Arch §5.3; Q-T05 |
| T3 | **Click-freedom by design:** per-grain window snapshots (the benchmark crackles on window changes), spawn-sampled parameters, smoothing classes, faded voice and FX swaps | Arch §6.4, §6.8, §9.2 |
| T4 | The benchmark uses linear interpolation for granular; we can afford cubic by default plus band-limited reads | Arch §6.5; Q-T14 |
| T5 | **Plugin live input** needs a VST3 input bus and an AU music-effect variant | Arch §5.6; Q-P06 |
| T6 | ~1,000–1,400 automatable parameters across 4 layers → the **stable parameter-ID scheme** is the main irreversible decision | Arch §9.4; Q-T12 |
| T7 | Spectral is a separate subsystem with fixed latency and the highest CPU cost → schedule last | Q-P18, Q-T10 |
| T8 | Firmware notes show the benchmark changed parameter scaling between versions → **version our parameter scaling** in presets | RN-10; GS-PRS-004 |

## 5. Evidence gaps

- **15 manual self-contradictions** (feature map §15). Two are resolved by the firmware release notes (C-09; C-13 partly). The rest need A/B tests on a unit or an owner ruling.
- **Features present only in the MIDI table:** master EQ, envelope loop modes, mod pages, window parameter 3, filter mode list. The release notes clarify grain probability and Draw XY.
- **Algorithms Tasty Chips does not document:** auto-slice detection, gran-filter analysis, the stealing law, the density-normalization law, the tape slew law, phase-vocoder overlap and window, sub-osc waveform. We treat these as our own design.
- **Unknown file formats** (presets are JSON plus files, per release notes). Importing them is a legal question (Q-L01), not an R1 need.
- **Firmware roadmap 1.9–1.12** not reviewed yet.

## 6. Decisions required (top of OPEN_QUESTIONS.md)

Q-P04 layers · Q-P05 sequencer tier · Q-P06 plugin live input · Q-P02 formats · Q-T01 framework · Q-P13 access to a GR-MEGA unit, plus Q-P19 (studio vs live) and Q-P20 (first-release engines).

## 7. Recommended next steps

1. ChatGPT technical review of the Issue #1 PR.
2. The owner answers OPEN_QUESTIONS §0 together with Q-P19/Q-P20 → Phase 2.
3. If a GR-MEGA unit is available, run a short A/B session on §C/§D items (looped release, normalization law, stealing, pan-spray distributions).
4. Review the firmware roadmap 1.9–1.12 at the start of Phase 2, in case planned features change the R1 parity target.

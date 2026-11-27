# GR-MEGA Feature Map

**Project:** GRANULAR SYNTH
**Tracking:** GitHub Issue #1 — Phase 1 reference research
**Source:** Tasty Chips GR-MEGA User Manual, document version 1.8.d (applies to firmware 1.8.x), 179 pages, downloaded from the official support page on 2026-11-27.
**Status:** Phase 1 research draft — awaiting ChatGPT technical review and owner approval. Nothing here is a GRANULAR SYNTH requirement; see GRANULAR_SYNTH_REQUIREMENTS.md.

---

## 0. How to read this document

Every row carries a **manual reference** (`p.NN — Section`) and a **classification**:

| Tag | CLAUDE.md evidence class | Meaning |
|---|---|---|
| **[MC]** | Manual-confirmed | Stated in manual 1.8.d (paraphrased); page and section given |
| **[INT]** | Interpretation | Our reading of how documented behavior is likely realized. Not stated by Tasty Chips; uncertainty explained inline |
| **[PROP]** | Proposal | GRANULAR SYNTH design recommendation beyond the documented behavior |
| **[UNV]** | Unverified | Manual silent, ambiguous or self-contradictory; evidence still required |

**[RN]** is not a fifth evidence class. Per owner review of PR #2 (2026-11-27, item 1), the four classes above remain canonical and exhaustive. **[RN]** is a *supplemental source marker* — it is always attached to one of the four canonical tags, never used alone — noting that the attached classification rests on Tasty Chips' official firmware release notes (fw 1.8.0–1.8.4, public text files on the support page) rather than the manual itself. In this document a fact "confirmed via [RN]" is written **[MC via RN]** (manufacturer-confirmed, sourced from release notes, same confidence tier as manual text) or, where the release notes are themselves terse or still leave behavior ambiguous, **[UNV] (partially confirmed via [RN])** / **[INT] via [RN]**. See §16.

All page numbers refer to the PDF page numbering of **GR-MEGA User Manual 1.8.d** (printed page number = PDF page number in this revision).

Hardware-only features (power, HDMI, LAN, physical file manager) are inventoried in §14 for completeness but are marked **N/A (hardware)** for our software.

The manual's MIDI/NRPN table (p.142–156) states it was *generated from the firmware's parameter enum (fw 1.8.0)*. It is therefore the most complete parameter list in the manual and in several places documents features that the prose chapters do not. Where the two disagree, both are cited and the conflict is logged in §15.

---

## 1. Product architecture

### 1.1 Top-level structure

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| ARCH-01 | Five sound engines | Granular, Granular Slice, Sampler, Tape, Spectral (phase vocoder). Granular is the primary/default engine. | p.37 Sound Engines; p.162 Specs | [MC] |
| ARCH-02 | Shared control surface across engines | Eight engine knobs A–H plus position slider; functions remap per engine but Scan and Pitch keep their meaning everywhere. | p.17 Sound engine controls; p.37–38 table | [MC] |
| ARCH-03 | Engine selection per patch | [Mode] button cycles granular → slice → sampler → … | p.134 Button combinations | [MC] |
| ARCH-04 | Multitimbral layers | 4 layers ("parts"). Each layer is focused / toggled with layer buttons. | p.13 Display; p.162 Specs | [MC] |
| ARCH-05 | Voice polyphony per layer | 1–20 voices per layer (patch setting); Spectral engine limited to 3. | p.52 Patch menu (General); p.120 Polyphony; p.162 | [MC] |
| ARCH-06 | Grain polyphony per voice | Max 128 simultaneous grains per voice. | p.39 Rate; p.120; p.162 | [MC] |
| ARCH-07 | Global grain budget | ~10,000 grains across all layers combined (5,000 on older hardware). 4 layers × 20 voices × 128 grains = 10,240, consistent with this. | p.162 Specs | [MC] + arithmetic [INT] |
| ARCH-08 | Spectral "polyphony" | Up to 12 chord notes per spectral voice at nearly no extra CPU. | p.50 Chord mode (Spectral); p.120; p.163 | [MC] |
| ARCH-09 | Internal format | 48 kHz sample rate, 32-bit float mix. | p.162 Specs | [MC] |
| ARCH-10 | Mono or stereo samples | Sample buffers are mono or stereo. | p.162 Specs; p.74 Init mono/stereo | [MC] |
| ARCH-11 | Multitimbral MIDI | Each layer listens on its own MIDI channel (layers may share a channel); MPE mode overrides channel mapping and drives all layers. | p.92 MIDI menu; p.171 FAQ | [MC] |
| ARCH-12 | "Control all layers" | UI option: knob tweaks affect all active layers instead of the focused one. | p.90 User Interface menu | [MC] |

### 1.2 Signal flow (from the p.32 diagram)

```
Audio In (stereo/mono) ──► SPL (sample buffer / recorder) ─────────────────────────────┐
     │                         │                                                       │ (Audition path)
     │                         ▼                                                       │
     │            ┌──── per VOICE (×20) ─────────────┐                                 │
     │            │ SND ENG ──► LPF ──► HPF ──┐      │                                 │
     │            │ Sub osc ──┘(summed before │      │                                 │
     │            │            filter)        │      │                                 │
     │            └───────────────────────────┼──────┘                                 │
     │                                        ▼                                        │
     ├──(audio-in FX send)──────────────► FX ×4 (chain) ──► M-S ──► Patch vol          │
     │                                    per LAYER (×4)                 │             │
     │                                                                   ▼             │
     │                                                   Layer mix (4 → stereo)        │
     │                                                                   ▼             │
     │                                                                  Wet            │
     └──────────────── Dry ──────────────────────────────────────────►  (+)            │
                                                                         ▼             │
                                                       Tuning sine / Metronome         │
                                                                         ▼             │
                                                          Master vol ◄─────────────────┘
                                                             │
                                        USB-C (per-layer + master taps) / Analog out
```

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| SIG-01 | Per-voice chain | Sound engine output + sub-oscillator → LPF → HPF, per voice. | p.32 diagram; p.20 Filter controls | [MC] |
| SIG-02 | Per-layer chain | 4-slot FX chain → Mid-Side → Patch volume. | p.32 diagram; p.81 Effects menu | [MC] |
| SIG-03 | Live input into layer FX | Audio input can be sent into each layer's FX chain (−100…+24 dB). | p.32; p.81 Sends column | [MC] |
| SIG-04 | Layer mix → Wet → sum with Dry input | Wet/Dry are system-wide; Dry is the unprocessed input. | p.18 Amp section; p.32 | [MC] |
| SIG-05 | Utility signals | Tuning sine (≈261 Hz C) and metronome injected before master. | p.32; p.89 Audio prefs; p.104 Sample Tuning | [MC] |
| SIG-06 | Audition path | Sample audition bypasses voices and goes to master. | p.32; p.16 Play | [MC] |
| SIG-07 | Multichannel output taps | USB-C streams 4 stereo layer outputs + master stereo (10 ch); 8 ch input (only first stereo pair used). | p.27–28 USB C; p.95 IO menu | [MC] |
| SIG-08 | Default voice gain −12 dB | Headroom for 4 droning layers. | p.33 note on voice volume | [MC] |
| SIG-09 | Grain-count gain normalization ("grain compression") | Prevents volume blow-up at high density; max volume reached at density ≈17. Switchable per patch. | p.33; p.56 Compression | [MC] |
| SIG-10 | No master limiter | Summed output can hard-clip at high polyphony. | p.121 Master volume and patch volume | [MC] |
| SIG-11 | Resample tap | Resampling records the signal after layer mix, before Wet and Master. | p.71 Audio source | [MC] |
| SIG-12 | Master EQ (5 bands) | Freq/gain/resonance/type/bypass per band + global bypass exist as NRPNs; no prose description. Prose mentions "future master effects". Release notes 1.8.0: Q up to 18, max band gain 24 dB. | p.148–150 NRPN 300–370; p.111; RN 1.8.0 | [MC] params / [RN] ranges / [UNV] placement in signal path |
| SIG-13 | Synth → recording feedback | Synth output can be fed back into the recording buffer at an adjustable level (Shift + env Amount fader). | p.19; p.135; NRPN 396 p.150 | [MC] |

---

## 2. Granular engine

### 2.1 Core grain parameters

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| GRN-01 | Rate | 0.1–1000 grains/s per voice. | p.39 Rate | [MC] |
| GRN-02 | Grain size | 5–5000 ms in prose; MIDI table says 0.1 ms–5 s. | p.39 Grain Size; p.142 CC 12 | [MC] — **conflict, see §15** |
| GRN-03 | Density | Density = Rate × Size (average number of overlapping grains); displayed live as e.g. "3.0 = 30.0 Hz × 100 ms". | p.35 Window; p.40 Rate, size, and density | [MC] |
| GRN-04 | Position | Grain spawn centre in the sample, set by the 100 mm position slider; Shift+slider scrubs when zoomed. 14-bit via NRPN. | p.14; p.37; p.141 NRPN example | [MC] |
| GRN-05 | Spray | Randomizes grain start position around Position; range 0 → whole sample (or one FFT block in spectral). | p.42 Spray; p.142 CC 13 | [MC] |
| GRN-06 | Spray polarity | Bipolar / positive-unipolar / negative-unipolar (Shift+Spray). NegUnipolar recommended for live scrolling-record use. | p.37 table; p.73 Scrolling rec; NRPN 395 p.150 | [MC] |
| GRN-07 | Pan | Per-grain static pan; in Sampler and Tape the pan range is doubled. | p.42 Panning | [MC] |
| GRN-08 | Pan spray | Per-grain random pan, 0 = centred → full stereo field. | p.42 Pan Spray | [MC] |
| GRN-09 | Pan spray distribution | Shape of the pan-spray random distribution (Shift+C). Distribution shapes not documented. | p.37 table; NRPN 505 p.153 | [MC] / [UNV] |
| GRN-10 | Mid-Side | Per-layer M-S width (Shift+G). | p.37 table; p.32 | [MC] |
| GRN-11 | Direction | Probability of reverse vs forward grains: full left = all reverse, centre = 50/50, full right = all forward. | p.43 Direction; p.142 CC 14 | [MC] |
| GRN-12 | Scan | Movement speed of the spawn position, −2× … +2× of original speed; follows stop/loop flags, loop and ping-pong; modulatable; clock-syncable. Fine mode (Shift) and ±0.25 snap mode (View). | p.37; p.43 Scan; p.142 CC 15 | [MC] |
| GRN-13 | Pitch | ±1 octave (coarse) / ±1 semitone (Shift); snapping with [Chord] held. | p.37–38 table | [MC] |
| GRN-14 | Pitch spray | Per-grain random pitch (View+H in granular/slice), with polarity setting. | p.38 table; p.54 Voicing right column; NRPN 1121, 1126 | [MC] |
| GRN-15 | Grain probability | NRPN exists ("Grain probability", 14-bit); not described in manual prose. Release notes 1.8.0 place it on Shift + knob A. Behavior (likely per-grain spawn-skip probability) undocumented. | p.155 NRPN 1125; RN 1.8.0 | [MC] param / [RN] control / [INT] behavior |
| GRN-16 | Engine knob table | Full A–H mapping (incl. Shift/View/Chord alternates) per engine. | p.37–38 | [MC] |

### 2.2 Grain ratio (scheduling) modes

| ID | Mode | Knob A | Knob B | Purpose | Ref | Class |
|---|---|---|---|---|---|---|
| GRN-20 | Free (default) | Rate | Size | Independent control; density is the result | p.40 Table 1 | [MC] |
| GRN-21 | DensitySize | Density | Size | Hold density constant while sweeping size (rate follows) | p.40 | [MC] |
| GRN-22 | DensityRate | Rate | Density | Hold density constant while sweeping rate (size follows) | p.40 | [MC] |
| GRN-23 | ScanRate | Rate × Scan | Size ÷ Scan | Rhythmic speed-up/slow-down tied to scan speed | p.40 | [MC] |
| GRN-24 | ScanOverlap | Rate × Scan | Size | Adds grains as scan speeds up to keep constant coverage of the source | p.40 | [MC] |
| GRN-25 | TimeStretch | — | Size | Two grains with constant *output* overlap (Akai/90s-jungle style), vs ScanRate/ScanOverlap which keep constant *input* overlap | p.40–41 | [MC] |
| GRN-26 | Mode list mismatch | NRPN 49/291 enumerate only 5 modes (no TimeStretch); spec table lists "Free, Sync-size, Scan & rate". | p.143; p.147; p.162 | **[UNV]** |

[INT] For ScanRate, multiplying rate and dividing size by |scan| keeps density constant while making grain timing proportional to source traversal; behavior at scan = 0 (division by zero) must be defined by us.

### 2.3 Grain window (envelope)

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| WIN-01 | Two window families | **PowAR** (GR-MEGA type: sides, tilt, curve) and **Raised Cosine** (tilt, curve, amplitude modulation). Toggled with Shift+[Grain Ratio]. p.18 calls this "GR-MEGA window type vs GR-1 type". | p.18 Amp section; p.135; NRPN 48 p.143 | [MC]; naming link [INT] |
| WIN-02 | Tilt | Skews window left/right (attack vs release emphasis). | p.135; NRPN 45 | [MC] |
| WIN-03 | Curve | Hollow ↔ linear ↔ bulging/square. | p.135; NRPN 46 | [MC] |
| WIN-04 | Sides (PowAR only) | 0 = square … 127 = triangular. | p.135; NRPN 44 | [MC] |
| WIN-05 | Window AM (Raised Cosine only) | Amplitude-modulation frequency relative to grain size (makes "multi-hump" windows). | p.135; NRPN 47 | [MC] |
| WIN-06 | Window parameter 3 | NRPN 217 "WIN PAR3" listed without description. | p.147 | [UNV] |
| WIN-07 | Window display | Shape plus density equation shown on main screen; becomes amplitude spectrum in spectral mode. | p.35 Window | [MC] |
| WIN-08 | Single global window per part | One shared window table per layer; changing it while grains play can crackle (minimal if gradual and convex). | p.121–122 Grain window modification | [MC] |
| WIN-09 | Window as filter | Window shape audibly filters the result at short grain sizes (microsound theory). | p.39 Granular Sound Engine | [MC] |
| WIN-P1 | Crackle-free window changes | Snapshot (or analytically evaluate) the window per grain at spawn so live window modulation never discontinues running grains. | — | [PROP] |

### 2.4 Grain scheduling, triggering and sync

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| SCH-01 | Grain key trigger | ON: a grain fires immediately on note-on. OFF: grain clock independent of notes. Panel cycles Off/On/Legato. | p.54 Grain Key trigger; p.135 [Key Trig] | [MC] |
| SCH-02 | Grain clock sync | Grain spawning locked to clock division instead of free rate. | p.55 Grain Sync; p.135 [Sync] | [MC] |
| SCH-03 | Synced rate divisions | 1/64 … 64/1 including triplets and dotted (33 entries); system toggles which families appear. 14-bit mul/div 1–128 also exposed. | p.157 Table 3; p.88 Straight/Triplet/Dotted rates; NRPN 1006–1015, 1109 | [MC] |
| SCH-04 | Voice sync | ON: grain clocks synced across voices; OFF: each voice has its own clock phase. The p.54 screenshot (fw 1.8.0-a14) shows "Clock retrigger"; release notes 1.8.0-a19 record a rename "voice retrigger" → "voice sync". | p.55 Voice sync; p.54 screenshot; NRPN 504; RN 1.8.0 | [MC]; rename [RN]; whether "Clock retrigger" is the same control [UNV] |
| SCH-05 | Spawn mode | **Gradual** (build up from the clock), **Direct** (full cloud as if running all along), **Direct reset** (full cloud all within spray area — more bass and level), **Recycle** (reuse old grains). Granular & slice only. | p.55 Grain spawn | [MC] |
| SCH-06 | Idle scheduler shutdown | The scheduler is disabled when a voice is idle for a while (CPU saving) — the reason spawn modes exist. | p.55 technical note | [MC] |
| SCH-07 | Local pitch ("granular glide") | ON: each grain keeps the pitch it was born with. OFF: running grains retune to the current note immediately. | p.55 Local pitch | [MC] |
| SCH-08 | Grain stealing | When 128 grains are in use the scheduler stalls, creating bunched "oscillation". Stealing threshold slider (0 = off) steals progressively; stealing is click-free and shortens grains. | p.55 Grain stealing threshold; p.120 Grain stealing | [MC] |
| SCH-09 | Anti-aliasing | Granular/slice: linear interpolation (residual aliasing possible). Sampler/tape: optional polyphase. Polyphase deemed too CPU-heavy for granular. | p.56 Anti-aliasing; NRPN 210 | [MC] |
| SCH-10 | Granular filter ("Gran filter") | Turns LPF/HPF cutoffs into a *spectral-content selector*: grains preferentially spawn in sample regions whose spectral centroid falls within the LPF/HPF window; matching regions highlighted on screen. Granular engine only. | p.56 Gran filter | [MC]; analysis method [INT] |
| SCH-P1 | Sub-sample accurate grain onsets | Fractional spawn offsets to eliminate jitter at high rates. | — | [PROP] |
| SCH-P2 | Better interpolation in granular | Cubic/Hermite as default, band-limited (mip-mapped) source for upward transposition; selectable quality. | — | [PROP] |

### 2.5 Chord mode & granular arpeggiator

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| CHD-01 | Chord mode in all engines | Toggle with [Chord]. | p.37; p.135 | [MC] |
| CHD-02 | 12 chord notes + fine tune | Voicing menu; duplicates allowed (raise weighting in arp/random). Disable a slot with "−". | p.53 Patch menu (Voicing) | [MC] |
| CHD-03 | Reference convention | C4 is the centre note (Roland convention); lowest chord note matches the played note, others transposed relative. | p.53 | [MC] |
| CHD-04 | Granular distribution | Granular engines split the cloud: chord notes are distributed across grains, total volume unchanged. Other engines stack notes (volume adds). | p.54 | [MC] |
| CHD-05 | Granular ARP modes | Up, down, down-up, up-down, random, shuffle, forward, backward, forward-backward, backward-forward. Up/down sort notes; forward/backward keep entry order. | p.55–56 ARP mode; NRPN 27 | [MC] |
| CHD-06 | Chord recording | Shift+[Chord] starts/stops recording chord notes from played keys. | p.135 | [MC] |
| CHD-07 | Pitch snapping | [Chord]+Pitch: snaps to semitones (no voicing) or to chord notes (voicing present). | p.38 | [MC] |
| CHD-08 | Sampler chord | Up to 12 grains (play heads) per voice; with periodic properties usable as a "pitched Euclidean sequencer". | p.47 Sampler | [MC] |

### 2.6 Pitch quantization

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| PQ-01 | Scale quantization | Select scale; user scale via toggling keys. | p.54 Voicing middle column | [MC] |
| PQ-02 | Microtonal / non-western scales | e.g. 24-TET, 31-TET (more than 12 keys per octave). | p.54 | [MC] |
| PQ-03 | Quantize per voice or per grain | Per-grain quantization makes pitch spray land on scale notes. | p.54 right column | [MC]; effect [INT] |
| PQ-04 | Sub-osc quantize | Sub-oscillator snaps to scale or stays unquantized. | p.54 | [MC] |
| PQ-05 | Full scale list | Not enumerated in the manual. | — | [UNV] |
| PQ-06 | MTS SysEx | Not implemented; considered for future. | p.141 | [MC] (absent) |

---

## 3. Granular Slice engine

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| SLC-01 | Granular engine operating on slices | All granular knobs apply except position slider and Scan, which have no function. | p.44 | [MC] |
| SLC-02 | Chromatic key mapping | Slice 1 on C-2 (lowest), each next slice one semitone up. | p.44 Slices | [MC] |
| SLC-03 | Manual slice add | Position slider + Operation encoder "AddSlice" + Exec; playable immediately. | p.45 | [MC] |
| SLC-04 | Auto-slice | Shift+push encoder; sensitivity on horizontal fader; confirm/cancel. Offline operation — may crackle while sounding. | p.46 Auto slice | [MC]; detection algorithm [UNV] |
| SLC-05 | Delete slice / delete all | DEL SLICE (+Shift = all). | p.46 | [MC] |
| SLC-06 | Drag slice | DRAG SLICE picks up and repositions a slice. | p.46 | [MC] |
| SLC-07 | Filter per slice | Independent LPF/HPF cutoff pair per slice (default OFF). | p.59 Filter per slice | [MC] |
| SLC-08 | Slice import | WAV cue points import as slices; SFZ load switches to slice mode with key groups as resampled slices. | p.108 Supported sample formats | [MC] |
| SLC-09 | Drum-sequencer use | Step sequencer triggers slices like a drum machine. | p.75 Step sequencer | [MC] |
| SLC-10 | "Sampler slice mode possible" | Listed in specs; not explained. | p.162 | [UNV] |
| SLC-11 | Slice capacity | Max number of slices not documented. | — | [UNV] |

---

## 4. Sampler engine

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| SMP-01 | Traditional sampler | Akai S-series / Simpler-like; no time-stretch; pitch = playback speed. | p.47 | [MC] |
| SMP-02 | One grain per voice | Extendable to 12 via chord mode. | p.47 | [MC] |
| SMP-03 | Rate & Size unused | Knobs A and B have no function. | p.47; p.37 | [MC] |
| SMP-04 | Spray / Pan spray / Direction active | Randomize start position and stereo placement per note; direction as in granular. | p.47 | [MC] |
| SMP-05 | Scan offsets spawn centre | Scan moves the central spawn position for new notes, with loop/stop behavior. | p.47 | [MC] |
| SMP-06 | Loop flags confine playback | One-shot / loop / ping-pong; looped release lets note-off exit the loop. Zero-crossing loop points required for click-free loops. | p.47; p.100 Sample Loop Confinement | [MC] |
| SMP-07 | Very long samples | Up to ~1 hour. | p.47; p.162 (60 min) | [MC] |
| SMP-08 | All voices displayed | Every voice's play head shown on the waveform. | p.47 | [MC] |
| SMP-09 | Polyphase anti-aliasing | Available for sampler and tape. | p.56 | [MC] |
| SMP-10 | Env pile-up | Same-note retrigger reuses the voice (default ON); recommended OFF for sampler. | p.52 | [MC] |
| SMP-11 | Multisamples | SFZ load (loudest velocity layer only; no velocity layers yet). | p.108; p.113 Multisamples | [MC] |

---

## 5. Tape engine

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| TAP-01 | Analog tape head simulation | Play head chases the target position; head velocity determines playback speed and pitch → vinyl-style scratching. | p.48 | [MC] |
| TAP-02 | Tape slew | Pitch knob becomes slew: how fast the head catches up to the (modulated) target. 14-bit. | p.48 Tape slew; p.38 table; NRPN 211 | [MC] |
| TAP-03 | Automated scratching | All position and scan modulation applies. | p.48 | [MC] |
| TAP-04 | Chord heads | Multiple heads with chord pitches; one tracks position, others are pitch-bent copies. Manual itself is unsure whether the tracking head is the first or lowest note ("TBV"). | p.48 Chord mode (Tape) | [MC] / **[UNV]** |
| TAP-05 | Direction knob unused | E/E+Shift have no function in tape. | p.37 table | [MC] |
| TAP-06 | Slew law | Shape of the follower (1-pole, 2-pole, inertia) undocumented. | — | [UNV] |

[INT] A plausible model: target = position + scan + mod; head obeys a critically damped follower; instantaneous read increment = d(head)/dt, which naturally yields pitch from motion.

---

## 6. Spectral (phase-vocoder) engine

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| SPC-01 | Purpose | Smooth time-stretch incl. extreme ("Paul stretch"-style ×1000), pitch/auto-tune with preserved timing, spectral mangling. Not intended as a transparent DJ/vocal tool (ringing). | p.49 | [MC] |
| SPC-02 | Block processing, no grains | Frequency-domain blocks; position moves bump-free. | p.49 | [MC] |
| SPC-03 | Polyphony 3 per layer | Due to FFT latency/CPU; 1 layer fine, 2 may drop out. | p.49; p.52; p.120–121 | [MC] |
| SPC-04 | FFT size | 256 / 512 / 1024 / 2048 / 4096 / 8192 (stepped, not modulatable). 8192 is mono. Larger = more latency. | p.49 FFT size; p.120 | [MC] |
| SPC-05 | Amplitude threshold | Removes weaker partials → "whistly" fundamental-only. Modulatable in newer firmware. | p.49 | [MC] |
| SPC-06 | Phase smear | Randomizes phases progressively; transients → slush. | p.49 | [MC] |
| SPC-07 | Amp smear | Smears amplitude spectrum; turning left sharpens it. | p.49 | [MC] |
| SPC-08 | Scan | Normal scanning; at scan 0 with no position change, loops one block (phase smear still randomizes). | p.49 | [MC] |
| SPC-09 | Bend | Extreme pitch bend; "Spectral bend" ON = log-map warp (natural); OFF = exponential frequency shift (ringier). Prose on p.57 describes the toggle as "legacy ringy" vs "newer non-ringy". | p.50 Bend; p.57; NRPN 392 | [MC] |
| SPC-10 | Pitch | Regular pitch shift. | p.50 | [MC] |
| SPC-11 | Pitch blend | Superimposes a pitch-shifted copy. | p.50 | [MC] |
| SPC-12 | Freeze | Progressive spectral freeze that yields to new input. | p.50; NRPN 1127 | [MC] |
| SPC-13 | Formant shift + formant lock | Lock ON: shift relative to original formant; OFF: shift relative to pitched note ("chipmunk"). Range ±24 st. | p.50; p.57; NRPN 393 | [MC] |
| SPC-14 | Fold spectrum | Folds bins back → metallic timbres. | p.50; NRPN 1130 | [MC] |
| SPC-15 | Spectral shaping by window knobs | Tilt/Curve/AM shape the spectrum like a grain window; AM as comb filter; Shift+Curve = spectral contrast; "Spec depth" applies the window to the spectrum (0 = bit-exact bypass). | p.50 Spectral shaping; NRPN 1133–1134 | [MC] |
| SPC-16 | FFT reducer | Mentioned in p.49 and NRPN 331, but the p.37 table assigns C+Shift to Amp smear. Release notes 1.8.0: FFT reducer was **merged into amp threshold** (overlapping function) — so the p.49/NRPN mentions are leftovers. | p.37; p.49; p.148; RN 1.8.0 | [MC via RN] (ambiguity resolved) |
| SPC-17 | RTPGHI mode | Higher-quality, transient-preserving spectral engine (more CPU; newer hardware only). RTPGHI = published *Real-Time Phase Gradient Heap Integration* method. | p.57 | [MC]; algorithm identity [INT] |
| SPC-18 | Chord mode ×12 | Pitch-shifted copies with timing preserved, almost no CPU. | p.50 | [MC] |
| SPC-19 | Sequencer as chord source | 3 voices × 7-note steps for large polyphony. | p.75 | [MC] |
| SPC-20 | Spray in spectral | Spray range = one FFT block. | p.142 CC 13 | [MC] |
| SPC-21 | Hop size / overlap / analysis window | Not documented. | — | [UNV] |

---

## 7. Engine comparison

| Aspect | Granular | Granular Slice | Sampler | Tape | Spectral |
|---|---|---|---|---|---|
| Core unit | Many short grains (≤128/voice) | Grains confined to a slice | 1 play head (≤12 with chord) | 1 chasing head (+chord copies) | STFT blocks, no grains |
| Pitch vs time | Independent | Independent | Coupled (speed = pitch) | Coupled, driven by head velocity | Independent (PV) |
| Position control | Slider + scan | Key → slice (slider/scan unused) | Slider + scan sets spawn centre | Slider = target position | Slider + scan |
| Rate/Size knobs | Yes (ratio modes) | Yes | No | No | Repurposed (Bend / FFT len / Phase smear / contrast) |
| Direction | Probabilistic per grain | Same | Same | No | No |
| Chord mode | Distributes notes across grains (constant level) + arp | Same | Stacks heads (adds level) | Stacks heads (pitch-bent copies) | Stacks pitch-shifted copies, ~free |
| Voices per layer | 20 | 20 | 20 | 20 | 3 |
| Interpolation | Linear | Linear | Linear / polyphase | Linear / polyphase | n/a (FFT) |
| Loop flags | Confine **scan** | n/a | Confine **playback** | Confine playback | Confine scan |
| Long samples | ≤5m49s effective | ≤5m49s | ≤~60 min | ≤~60 min (assumed same as sampler) | ≤5m49s (assumed) |
| CPU driver | grains × voices, spray scatter | same | low | low | FFT size × voices × layers (highest) |
| Typical use | Pads, clouds, freezes, live FX | Beat slicing/drums | Classic sampling, loops | Scratching, tape-stop | Extreme stretch, spectral FX |

Refs: p.37–50, p.56, p.74, p.99–100, p.120–121, p.162. Long-sample limits for Tape/Spectral are [INT].

---

## 8. Voices, polyphony, envelopes, filters, sub-oscillator

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| VOI-01 | Polyphony slider | 1 (mono) … 20; "PV LIMIT" when spectral clamps to 3. | p.52 | [MC] |
| VOI-02 | Voice stealing | Removes oldest note, preferring those in release and not held by sustain; also weighs keyboard position. Abrupt. | p.120 Voice stealing | [MC]; exact heuristic [UNV] |
| VOI-03 | Note hold | MIDI sustain/hold CC64 holds notes. | p.120 Note hold; CC 64 p.143 | [MC] |
| VOI-04 | Glide | 0–2000 ms; "Always" glides even on non-overlapping notes. | p.52; CC 24/25/84 | [MC] |
| VOI-05 | Velocity sensitivity | 0–100 % on amp envelope; global velocity curve in MIDI menu. | p.53 Velosens; p.92 | [MC] |
| VOI-06 | Pitch bend range | 0–48 semitones; MPE forces 48. | p.52; CC 2 | [MC] |
| VOI-07 | Transpose | Semitones + cents per patch (sample tuning workflow). | p.52; p.104 | [MC] |
| VOI-08 | Env pile-up | See SMP-10. | p.52 | [MC] |
| ENV-01 | Four ADSR envelopes | Pitch, Filter, Amp, Aux; each with bipolar amount. | p.18; p.162–163 | [MC] |
| ENV-02 | Stage time range | 0.1 ms – 45 s per stage. | p.163; CC 55–75 (0–45 s) | [MC] |
| ENV-03 | Curved stages | Per-stage curve (A, D, R) from log through linear to exponential. | p.18; p.135–136; NRPN 156–173 | [MC] |
| ENV-04 | Invert | Per envelope. | p.18; NRPN 159/164/169/174 | [MC] |
| ENV-05 | Envelope loop mode | NRPN for each envelope's loop mode exists; not described. | p.146 NRPN 160/165/170/175 | [MC] (param) / [UNV] |
| ENV-06 | Aux envelope assignable | Hold [Aux] + turn knob to assign; available as mod source. | p.136; p.80 | [MC] |
| FLT-01 | LPF + HPF per voice | Cutoff 0 Hz–20 kHz, resonance. | p.20; CC 50–53 | [MC] |
| FLT-02 | Slopes | 6/12/18/24 dB/oct per filter (Shift+cutoff). | p.137–138; p.163; NRPN 1153–1154 | [MC] |
| FLT-03 | Filter topology | "trap-SVF mode enum" → trapezoidal (TPT) state-variable filter. | p.150 NRPN 391 | [MC] (name) / [INT] (topology) |
| FLT-04 | Filter routing / bypass | LPF only / LPF+HPF / HPF only; bypass. | p.147–148 NRPN 289, 293 | [MC] (param) |
| FLT-05 | Keytrack → LPF cutoff | Bipolar shortcut that writes a matrix route. | p.154 NRPN 1105 | [MC] |
| SUB-01 | Sub-oscillator per voice | Toggle [Sub]; frequency −1/−2 oct relative; amplitude (Shift+FX1/FX2 with Sub lit). | p.32; p.135; p.138; CC 4/5; NRPN 503 | [MC] |
| SUB-02 | Sub-osc balance | Polyphonic emphasis on low vs high notes. | p.53 | [MC] |
| SUB-03 | Sub-osc waveform | Not documented. | — | [UNV] |

---

## 9. Sample management

### 9.1 Loading, formats, storage

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| SMG-01 | WAV | Load & save; 8/16/24-bit int, 32-bit float; cue points → slices. | p.108 | [MC] |
| SMG-02 | AIFF | Load only (no proprietary Ableton variant; no preview). | p.108 | [MC] |
| SMG-03 | FLAC | Load only. | p.108 | [MC] |
| SMG-04 | SFZ | Load only; switches to slice mode; loudest velocity layer only; key groups become resampled slices. | p.108; p.113 | [MC] |
| SMG-05 | HLAC | Load only; speed-optimized lossless format (Tasty Chips). | p.108 | [MC] |
| SMG-06 | Import options | Auto-convert to 48 kHz (slow); auto-normalize on import. | p.94 IO menu | [MC] |
| SMG-07 | Previews | Audition in file chooser and file manager; scrub preview with fader; Shift+Play manual preview. | p.67; p.94; p.107 | [MC] |
| SMG-08 | Max efficient length | 5 min 49 s for granular ("efficiently handle"); FAQ advises cropping 1 h samples. | p.74 Sample size; p.171 FAQ; p.162 ("3 minutes max in granular") | [MC] — **conflict, see §15** |
| SMG-09 | RAM/disk usage display | Warning thresholds 95 % RAM / 99 % disk. | p.95 | [MC] |
| SMG-10 | Storage layout | user/samples, user/recording, user/resampling, user/templates, user/fx, factory (write-protected). | p.65; p.72; p.84; p.109; p.111 | [MC] |

### 9.2 Recording / sampling

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| REC-01 | Sources | Analog in, Resampling (post layer-mix), USB-C (first stereo pair), USB-A class-compliant interface. | p.71–72 Audio source | [MC] |
| REC-02 | Destination | Into current preset's buffer, or background WAV file (auto-numbered prefix). | p.72 Destination | [MC] |
| REC-03 | Channel duplication | LL / RR for mono sources. | p.73 Channels | [MC] |
| REC-04 | Input gain | ±12 dB. | p.73 | [MC] |
| REC-05 | Arm / trigger | Arm on MIDI beat start; input threshold trigger −80…0 dB or OFF. | p.73 Arm; CC 40 | [MC] |
| REC-06 | Record start mode | Current position (resume) / Wiper (slider position) / Start (0 s) / Off (record protection). | p.73 | [MC] |
| REC-07 | Loop recording | One-shot vs continuous circular recording for live processing. | p.73; p.96 | [MC] |
| REC-08 | Scrolling record | Play head kept at an adjustable distance behind the record head (fader sets latency); zoom 1–10 s. Supersedes "Play follows rec". | p.73 Scrolling rec | [MC] |
| REC-09 | Clocked record | Length in bars (4 quarter notes); stops at bar end; based on current BPM. | p.74 Clocked record | [MC] |
| REC-10 | Sample size | AUTO (buffer grows while recording) or fixed (seconds/bars, max 5m49s); requires Init mono/stereo. | p.74 | [MC] |
| REC-11 | Init mono / stereo | Creates an empty buffer; undo by pressing current preset. | p.74 | [MC] |
| REC-12 | Resampling workflow | Records to background WAV; on stop: discard / keep / keep & load. | p.97 | [MC] |
| REC-13 | Granulate while recording | All engines (incl. spectral) can play the buffer during recording; red record wiper on display. | p.96; p.127–129 | [MC] |
| REC-14 | Live granular FX workflow | Dedicated input level / wet / dry / master knobs; loop recording + scan 1.0×, or scrolling rec. | p.127–129 | [MC] |
| REC-15 | Synth → rec feedback | See SIG-13. | p.19 | [MC] |

### 9.3 Editing, markers and looping

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| EDT-01 | Flags | Start (crop start; not the grain position), Stop (crop end + scan end point), Loop start, Loop end. Also set via Shift + A/D/S/R faders. | p.99 Start End flags; p.135–136 | [MC] |
| EDT-02 | Loop confinement semantics | Granular & spectral: flags confine **scan**. Sampler & tape: confine **play heads**. Loop changes take effect on next note. | p.100 | [MC] |
| EDT-03 | Scan modes | One-shot (stop at flag), Loop (wrap), Bounce/ping-pong. Spec also lists "kill". | p.58; p.101–102; p.162 | [MC]; "kill" [UNV] |
| EDT-04 | Scan key trigger | OFF (shared, never reset) / ON (reset each note-on) / LEGATO (first note only) / POLY (independent per voice — "auto fugue"). | p.58 Key trigger | [MC] |
| EDT-05 | Scan clock sync | Scan speed from clock division. | p.58 Sync | [MC] |
| EDT-06 | Loop clock sync | Loop traversal locked to beat. | p.58 | [MC] |
| EDT-07 | Looped release | ON: stay looping after release / OFF: exit after an even number of loops/bounces. **Prose on p.58–59 and p.102–103 describe ON/OFF oppositely.** | p.58–59; p.102–103 | **[UNV]** |
| EDT-08 | Crop modes | Zoomed (crop to view), Recorded (crop up to record head), Start–End (between flags). | p.105–106 | [MC] |
| EDT-09 | Soft cut | Crossfade at crop boundaries (shortens sample slightly); global pref. | p.89; p.106 | [MC] |
| EDT-10 | Normalize | Operation; also optional on import. | p.105 | [MC] |
| EDT-11 | Tuning | Patch transpose (semi+cent) against built-in ≈261 Hz tuning sine. | p.104 | [MC] |
| EDT-12 | Zoom & scroll | Shift+encoder zoom; arrows / Shift+slider scroll; OSD shows visible time span. | p.14; p.34; p.105 | [MC] |
| EDT-13 | Waveform / spectrogram view | [View] toggles time series vs spectrogram. | p.34 | [MC] |
| EDT-14 | Sample colour per layer | View+Layer randomizes waveform colour; Shift+View+Layer resets. | p.132 | [MC] |
| EDT-15 | Operation encoder | Central "operation" menu (AddSlice, Crop, Normalize, Set loop…, Zoom…) executed with [Exec]. | p.14; p.45; p.100; p.105 | [MC] |
| EDT-P1 | Zero-crossing snap & loop crossfade | Auto snap loop flags to zero crossings + optional loop crossfade (sampler/tape). | — | [PROP] |
| EDT-P2 | Non-destructive editing | Keep the original file; crop/normalize as metadata until "commit". | — | [PROP] |

---

## 10. Modulation

### 10.1 LFOs

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| LFO-01 | 4 LFOs per layer | Each layer has its own LFOs. | p.79 Mod sources; p.163 | [MC] |
| LFO-02 | Rate | 0 (stopped) – 50 Hz; synced division when sync ON; 14-bit free and synced controls. LFO3/4 have separate clock mul/div NRPNs. | p.21; CC 89 etc.; NRPN 1010–1013, 1110–1117 | [MC] |
| LFO-03 | Waveforms | Sine, triangle, saw, −saw, square, random. | p.21; CC 90 | [MC] |
| LFO-04 | Sync source | Free / MIDI clock / sequencer. | CC 88 p.144 | [MC] |
| LFO-05 | Key sync | OFF / ON / LEGATO. | p.21; p.137; NRPN 184–187 | [MC] |
| LFO-06 | Phase | Start phase (14-bit). | p.21; NRPN 188–191 | [MC] |
| LFO-07 | Amplitude quantization | Continuous … 2 steps (staircase LFO). | p.137; NRPN 192–195 | [MC] |
| LFO-08 | Polarity | +uni / −uni / +bi / −bi. | NRPN 196–199 | [MC] |
| LFO-09 | Direct LFO routing | Each LFO has its own destination + amount independent of matrix rows (LFO page). | p.21; NRPN 1145–1152 | [MC] |
| LFO-10 | Panel assignment | Hold [LFO Select] + turn any knob = assign destination and amount in one gesture. | p.21; p.78; p.137 | [MC] |
| LFO-11 | Per-voice vs per-layer LFO phase | Not stated whether LFOs run per voice (poly) or per layer. Key sync with "Legato" suggests per-layer (shared) LFOs. | — | **[UNV]** / [INT] |
| LFO-12 | Legacy amount scaling flag | "LFOAMT25" switches to corrected LFO-amount math (default ON since 1.7). | p.52 | [MC] |

### 10.2 Modulation matrix

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| MTX-01 | Rows | Up to 50 rows per patch. | p.78; CC 76 | [MC] |
| MTX-02 | Row fields | Enable, Source, Curve, Amount, Polarity (+uni/−uni/+bi/−bi), Destination category, Destination. | p.78 screenshot; CC 77–82 | [MC] |
| MTX-03 | Curve | Bias modulation response: low-flat→hollow ↔ linear ↔ high-flat. | p.79 Curve; CC 79 | [MC] |
| MTX-04 | Sources (25 + None) | LFO1–4, CV1–2, Seq mod 1–4, Mod wheel, Key track, Velocity (MPE Strike), Pitch wheel (MPE Slide), Aftertouch (poly under MPE), Timbre (MPE CC74), Lift (release velocity), Aux env, Random1–4 (new value per note, per layer), Pos X (fader), Draw X, Draw Y, Input envelope follower. | p.79–80; p.157–158 Table 4 | [MC] |
| MTX-05 | Destinations | 149 enumerated (index 0–148): 23 engine, 19 envelope, 4 filter, 16 LFO, 1 sequencer (length), 76 FX, 10 macro. | p.158–161 Table 5 | [MC] |
| MTX-06 | Destination count wording | Manual cites 140, 150 and "and growing". | p.23; p.76; p.78; p.163 | [MC] (inconsistent wording) |
| MTX-07 | Quick assign | Shift + turn any control assigns it as destination of the current row. | p.79 | [MC] |
| MTX-08 | Mod pages | Four front-panel "mod pages" with own source, destination, amount, curve. | NRPN 340–347, 397–400, 1135–1144 | [MC] (params) / [UNV] (UI semantics) |
| MTX-09 | Copy/paste/delete rows | Copy row, paste row, delete row, delete all. | p.133–134 | [MC] |
| MTX-10 | Not modulatable | FFT size (stepped). Window shape changes are modulatable but crackle-prone. | p.49; p.121 | [MC] |
| MTX-11 | Envelope follower | Input follower AR via Shift+[ADSR Curve] + A/R faders; follower input selectable (audio in or layer 1–4); per-layer follower attack/release; follower gain ±24 dB. | p.80; p.89; p.137; NRPN 482–491, 496 | [MC] |
| MTX-12 | Drawing modulation | Draw X/Y trajectories with play, speed, loop mode, tempo sync, snap, phase, rate, key sync (global / per mtpreset). Manual references a drawing menu without documenting it. Release notes 1.8.0: "Draw XY" tile menu, **touchscreen required**, multitouch scale/rotate, waypoints that stop/restart movement, LFO-section controls (rate, phase, sync, key trig Off/On/Legato) apply. | p.80; p.154–155 NRPN 1100–1104, 1122–1124; RN 1.8.0 | [MC] params / [RN] UI & behavior |
| MTX-13 | CV direct routes | CV1/CV2 each have direct destination + amount. | CC 83, 85–87 | [MC] |
| MTX-14 | 10-destination monitor | Sidebar shows recently used destinations with summed modulation values. | p.36 | [MC] |

### 10.3 Macros

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| MAC-01 | 10 macros per patch | Levels on the 5 vertical faders (+Shift for 6–10). | p.19; p.23–24 | [MC] |
| MAC-02 | Multi-destination with ranges | Hold [Macro Assign] + turn a knob: adds destination; continued turning sets start/end range. Any matrix destination incl. FX. | p.23 | [MC] |
| MAC-03 | Curve & invert | Per-assignment curve; [Invert] swaps start/end. | p.23 | [MC] |
| MAC-04 | Polyphonic macro modulation | Macros can be modulated (e.g. by LFO) — "polyphonic modulation" — shown as moving dot on the macro bar. | p.23–24 | [MC]; per-voice semantics [UNV] |
| MAC-05 | Macro menu | Full overview with copy/paste/delete. | p.23 | [MC] |
| MAC-06 | Storage | Per patch; copy via templates. | p.24 | [MC] |
| MAC-07 | Macros 11–20 | NRPN table lists MACRO1–MACRO20 but prose and destination table describe 10. | p.150 NRPN 371–390 | **[UNV]** |

### 10.4 Random & generative

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| GEN-01 | Random1–4 | New random value per note, per layer. | p.80 | [MC] |
| GEN-02 | Per-grain randomization | Spray, pan spray (+distribution), pitch spray, direction probability, grain probability (NRPN). | p.42–43; p.54; NRPN 1125 | [MC] |
| GEN-03 | Sequencer drunkness | Probability and size of missteps. | p.22 | [MC] |
| GEN-04 | Step chance | Per-step probability. | p.75 | [MC] |
| GEN-05 | Sequencer random row | Shift+Exec randomizes a row. | p.76 | [MC] |
| GEN-06 | Patch randomizer | Choose which sections are randomized; matrix up to 20 rows; per-knob list mode; random patch names. | p.60 | [MC] |
| GEN-07 | Arp random/shuffle | Chord notes randomized across grains. | p.55–56 | [MC] |
| GEN-08 | Resonator noise | Randomized band gains with update rate. | p.86 | [MC] |

---

## 11. Performance & sequencing

### 11.1 Keyboard & MIDI behavior

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| MID-01 | DIN MIDI in/out/thru + USB-C MIDI | Hardware ports. | p.26–27 | [MC] (hardware) |
| MID-02 | Per-layer MIDI channel | Part 1–4 channel. | p.92 | [MC] |
| MID-03 | MPE | Strike/Slide/Pressure/Timbre(CC74); channel aftertouch becomes polyphonic; PB range fixed to 48. | p.52; p.80; p.92 | [MC] |
| MID-04 | CC + 14-bit NRPN | Every 0–127 continuous parameter also as 14-bit NRPN; optional 14-bit CC send from panel. | p.141; p.93 | [MC] |
| MID-05 | Full CC/NRPN map | ~400 assigned numbers incl. per-FX params, macros, matrix rows. | p.142–156 | [MC] |
| MID-06 | Program change receive | Toggle. | p.93 | [MC] |
| MID-07 | Receive filters | CC / notes / PGM change toggles. | p.93 | [MC] |
| MID-08 | Clock in | External beat clock for all synced params; PPQN 4–960 (default 24; ≥48 needed for 1/64 grains/notes). | p.92 | [MC] |
| MID-09 | Clock/transport out | Send clock, START/STOP from sequencer. | p.75–76; p.93 | [MC] |
| MID-10 | MIDI clock starts sequencer | Toggle. | p.93 | [MC] |
| MID-11 | Velocity curve | Global. | p.92 | [MC] |
| MID-12 | Channel mode messages | All sound off (incl. FX tails), all notes off (envelopes release), reset CCs, local off, mono/poly mode. | CC 120–127 p.145 | [MC] |
| MID-13 | Play mode per layer | [▶] button plays a continuous note per enabled layer (drone). Hold 3 s / triple-tap = all sound off. | p.16; p.92; p.132 | [MC] |
| MID-14 | Layer/preset select via MIDI | Enable/focus layer, layer preset number, layer volume, layer channel map. | CC 3, 18; NRPN 176–183, 200–207 | [MC] |
| MID-15 | UI navigation via MIDI | Navigate left/right/up/down NRPNs. | NRPN 283–286 | [MC] |
| MID-16 | SysEx | Not implemented. | p.141 | [MC] |
| MID-17 | CC display on OSD | Optional. | p.93 | [MC] |
| MID-18 | Pots pickup | Soft-takeover option after preset/layer switch. | p.90 | [MC] |

### 11.2 Step sequencer

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| SEQ-01 | One sequencer per layer | Up to 4 running in multitimbral mode; enable per layer. | p.22; p.164 | [MC] |
| SEQ-02 | Length | Up to 64 steps (8 bars × 8 steps). | p.75 | [MC] |
| SEQ-03 | Notes per step | Up to 7 (chord or slice notes); polyphonic, overlaps allowed; mono patch → replacement (glide-friendly). | p.75–76 | [MC] |
| SEQ-04 | Step data | Note length, velocity, chance, 4 modulation values. | p.75 | [MC] |
| SEQ-05 | Mod lanes | 4 lanes → SEQ MOD 1–4 sources; slew 0–1000 ms; usable without notes (as complex LFOs). | p.76 | [MC] |
| SEQ-06 | Parameter locks | 16 lanes, each assigned to any modulation destination (or position with Shift+fader); empty steps "-"; live tweak overrides until next lock. | p.76–77 | [MC] |
| SEQ-07 | Ties | Tie neighbouring steps for long notes and legato/glide per row. | p.77 | [MC] |
| SEQ-08 | Transport controls | BPM (tap tempo), Rate (division), Position, Length, Play mode (Forward/Reverse/Ping-pong/Random), transpose, drunkness, metronome. | p.22; p.134; CC 34–39 | [MC] |
| SEQ-09 | Global vs free BPM | Per-layer free-running tempo option. | p.76 | [MC] |
| SEQ-10 | Position model | Seeded (derived from master counter) vs Incremental. | p.87 | [MC] |
| SEQ-11 | Step entry | Arrow navigation, fader coarse + encoder fine, Shift = whole row; copy/paste column; delete cell/column/all; MIDI record into sequencer. | p.16; p.76; p.133–134 | [MC] |
| SEQ-12 | Live performance | Jump to step with preset buttons; arm next bar (bank/subbank buttons); switch presets while running. | p.16; p.22; p.134 | [MC] |
| SEQ-13 | MIDI out of sequencer notes | Sends on layer channel; internal engine always also triggers. | p.75; p.93 | [MC] |
| SEQ-14 | Key track from sequencer | Sequencer notes drive Key track source. | p.79 | [MC] |
| SEQ-15 | "Rate sets time signature" | p.22 says Rate sets ¾, 4/4, 6/8; p.134 says rate division. | p.22; p.134 | **[UNV]** |

### 11.3 Performance controls

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| PRF-01 | Layer mix faders | ADSR faders become layer 1–4 volumes; Shift = per-layer position. | p.18; p.135–136 | [MC] |
| PRF-02 | Patch spray / patch bias | Boosts grain counts depending on which screen region (layer centre) is focused: bias, spray width, intensity. | p.19; NRPN 500–502 | [MC] (controls) / **[UNV]** (semantics) |
| PRF-03 | Feel button | Inspect control value without changing it; tooltips. | p.15 | [MC] (hardware UX) |
| PRF-04 | Copy/paste everywhere | Presets, matrix rows, steps, FX, templates, mtpresets. | p.15; p.133 | [MC] |
| PRF-05 | Undo | Revert to last save point (preset button or Shift+Enter). Saves/project loads set undo points; loading patches/samples does not. | p.110 | [MC] |
| PRF-06 | Computer keyboard shortcuts | Navigation, copy/paste, load/save, play/rec. | p.139–140 | [MC] |
| PRF-07 | DSP meter | CPU bar split into FX / USB audio / housekeeping / voice rendering. | p.87 | [MC] |

---

## 12. Effects

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| FX-01 | 4 FX slots per layer | Serial chain; each effect type once per layer. | p.81 | [MC] |
| FX-02 | Sends | Audio-in send and Synth send into the chain, −100…+24 dB (high level needed for non-linear FX). | p.81; p.33 | [MC] |
| FX-03 | Per-slot Dry/Wet | Also from panel (Wet knob / Shift). | p.84; p.138 | [MC] |
| FX-04 | FX1/FX2 panel knobs | Two assignable parameters of the selected slot. | p.82; p.84; p.138 | [MC] |
| FX-05 | FX chain presets | Load whole 4-slot chain; user slots ≥ 9; stored as JSON. | p.83–84 | [MC] |
| FX-06 | Single-FX presets | 8 factory + user presets per FX type; JSON. | p.84 | [MC] |
| FX-07 | Modulatable | Almost all FX params are matrix destinations. | p.85; p.159–161 | [MC] |
| FX-08 | FX tails on preset switch | Option: linger vs silence. | p.89 FX buffer reset | [MC] |
| FX-09 | Parameter curves | Freq/time/amp params use cubic response. | p.85 | [MC] |
| FX-10 | Reorder | Copy/paste swap or drag & drop. | p.82 | [MC] |

### 12.1 Effect types (14)

| ID | Effect | Documented parameters | Ref | Class |
|---|---|---|---|---|
| FX-T01 | Chorus | rate, feedback, centre, bandwidth, dry, wet | p.83; NRPN 425–430 | [MC] |
| FX-T02 | Compressor | threshold, knee, ratio, attack, release, dry, wet; VU meters | p.83; NRPN 431–437 | [MC] |
| FX-T03 | Delay | length, amount (feedback), dry, wet; independent stereo channels | p.83; NRPN 438–441 | [MC] |
| FX-T04 | Ping-pong delay | length, amount, dry, wet | p.83; NRPN 442–445 | [MC] |
| FX-T05 | Distortion ("Overdrive" in NRPN) | inverse-cubic type; amount, dry, wet | p.83; NRPN 446–448 | [MC] |
| FX-T06 | EQ3 | 3-band shelf; gains (silence…+6 dB), low/mid crossover 50 Hz–5 kHz, mid/high 200 Hz–18 kHz, dry, wet | p.83; NRPN 519–525 | [MC] |
| FX-T07 | Flanger | rate, feedback, centre, bandwidth, dry, wet | p.83; NRPN 449–454 | [MC] |
| FX-T08 | Reverb | simple, CPU-light; room size, damping, width, dry, wet | p.83; NRPN 467–471, 279–280 | [MC] |
| FX-T09 | Large reverb | pre-delay, time, room size, spread (M-S), early, tail, damping + 3-band EQ (6 params) | p.83; p.85; NRPN 455–462 | [MC] |
| FX-T10 | Reducer | bit depth, sample rate, dry, wet | p.83; NRPN 269–270, 465–466 | [MC] |
| FX-T11 | Resonator (MBR) | tone (LP↔BP↔HP), cutoff, resonance, detune (folds at 20 kHz), pitch, tilt, decay, bands 6–48, scale (incl. inharmonic e.g. metal rods), mono/stereo mode, width, noise, noise rate 0.05–50 Hz, pan, dry, wet | p.85–86; NRPN 506–518 | [MC] |
| FX-T12 | Ring modulator | coarse/fine freq, dry, wet; spec mentions "AM mode" | p.83; p.163; NRPN 472–475 | [MC]; AM mode [UNV] |
| FX-T13 | Saturator | tanh-type; drive, post gain, dry, wet | p.83; NRPN 492–495 | [MC] |
| FX-T14 | Vibrato | rate, centre, dry, wet | p.83; NRPN 476–479 | [MC] |

---

## 13. Preset & project management

| ID | Feature | Detail | Ref | Class |
|---|---|---|---|---|
| PRS-01 | Data hierarchy | Sample → Template (all sound parameters, hundreds) → Patch (= sample + template) → Preset (stored patch in a slot) → Mtpreset (up to 4 layers + volumes) → Project. System settings separate. | p.111–113 GR-MEGA Data types | [MC] |
| PRS-02 | Patch embeds sample relationship | Each patch has its own template because templates may be sample-specific (e.g. slice points). | p.111 | [MC] |
| PRS-03 | Presets per project | 128 = 4 banks × 4 sub-banks × 8 presets (e.g. A-2-7). | p.16; p.111; p.162 | [MC] |
| PRS-04 | Mtpresets per project | 128; 16 quick-access (Shift + buttons); "Fast MT select" option. Standalone/self-contained since fw 1.7 (previously references). | p.16; p.91; p.110; p.172 | [MC] |
| PRS-05 | Mtpreset sample detach | Option to store samples with the mtpreset on save. | p.95 | [MC] |
| PRS-06 | Projects | 50 slots; contain 128 presets, 128 mtpresets, name, global BPM, rate. | p.64; p.113 | [MC] |
| PRS-07 | Templates | Global library; default template = init patch (Shift+Delete). Used to copy macros/settings across patches. | p.62; p.111; p.129 | [MC] |
| PRS-08 | Comments | Up to 100 characters per patch. | p.52 | [MC] |
| PRS-09 | Sample name → patch name | One-click copy. | p.52 | [MC] |
| PRS-10 | Save semantics | Context-sensitive Save (1 layer → preset, multiple layers → mtpreset, after recording → choose sample/preset/mtpreset); expert save dialog; save-on-shutdown option. | p.94; p.109–110 | [MC] |
| PRS-11 | Import compatibility | Loads GR-1 patches (.grp + .wav folders) and performances; files interchangeable with GR-2. | p.108; p.111; p.171 | [MC] |
| PRS-12 | Sample references & portability | Samples live in internal storage paths; the manual does not say how patches reference samples. Release notes indicate samples are saved with presets/projects (optional HLAC compression) — see RN-16/18. | p.65; p.109; p.114; RN 1.8.1, 1.8.4 | [UNV] (partially confirmed via [RN]) |
| PRS-13 | Patch file format | Not documented in manual (FX presets are JSON, p.84). Release notes 1.8.4 refer to preset `.json` files with accompanying files. Internal schema unknown (and not needed — see Q-L01). | p.84; RN 1.8.4 | [UNV] (partially confirmed via [RN]; internal JSON schema still unverified) |
| PRS-14 | Factory content | Write-protected factory projects/samples; downloadable factory packs. | p.65; support page | [MC] |
| PRS-P1 | Portable bundles | "Collect & save" with relative sample paths + content hash, plus embedded-sample option for plugin state. | — | [PROP] |

---

## 14. System, UI and hardware-only features

| ID | Feature | Ref | Relevance to us |
|---|---|---|---|
| HW-01 | 7" 1024×600 display, dimmable | p.13 | N/A (hardware); informs UI density |
| HW-02 | 100 mm position slider, op encoder, 8 engine knobs, faders | p.14–22 | Informs MIDI-controller mapping & UI |
| HW-03 | CV in ×2 (±5 V), gate out | p.25 | N/A; optional: CV via DC-coupled audio inputs |
| HW-04 | Balanced I/O +4 dBu, ~114 dB SNR | p.25–26 | N/A |
| HW-05 | USB-A host (storage, keyboards, class-compliant audio) | p.26; p.72 | N/A |
| HW-06 | USB-C UAC2 multichannel audio (48 kHz/16-bit), MIDI, file sharing | p.27–28; p.95 | Maps to plugin multi-out busses |
| HW-07 | HDMI out, touchscreen, mouse | p.28; p.51 | N/A (desktop app is natively mouse-driven) |
| HW-08 | LAN: firmware update, SMB file sharing; RTP-MIDI planned | p.29; p.114 | N/A |
| HW-09 | Power/standby, safe shutdown | p.29–31 | N/A |
| HW-10 | File manager (dual pane, copy/move/delete, eject) | p.65–71 | N/A (macOS Finder); in-app browser still needed |
| HW-11 | Firmware update | p.87; p.166 | N/A (app updater instead) |
| UI-01 | UI prefs: control all layers, pot pickup, menu slider, direct shift, instant bank switch, tooltips, message duration, brightness | p.90–91 | Partially relevant (pickup, tooltips) |
| UI-02 | Text entry, button combos | p.131–138 | N/A (keyboard available) |
| UI-03 | Menu tiles; [Menu]+preset n shortcuts | p.51 | UX inspiration |

---

## 15. Manual inconsistencies and undocumented items (feed into OPEN_QUESTIONS.md)

| # | Topic | Conflict | Refs |
|---|---|---|---|
| C-01 | Grain size range | 5–5000 ms (prose) vs 0.1 ms–5 s (MIDI table) | p.39; p.142 |
| C-02 | Max granular sample length | 5m49s (sampling menu, FAQ) vs "3 minutes max in granular" (specs) | p.74; p.171; p.162 |
| C-03 | Ratio modes | 6 in table incl. TimeStretch vs 5 in NRPN enums vs 3 named in specs | p.40–41; p.143; p.147; p.162 |
| C-04 | Scan modes | "four modes" but three listed; spec adds "kill" | p.58; p.162 |
| C-05 | Looped release ON/OFF meaning | Opposite descriptions | p.58–59 vs p.102–103 |
| C-06 | Macro count | 10 (prose, destinations) vs 20 (NRPN) | p.23; p.161; p.150 |
| C-07 | Mod source count | 26 (p.78), 24 (specs), 25 + None (table) | p.78; p.163; p.157–158 |
| C-08 | Mod destination count | 140 / 150 / 149 enumerated | p.23; p.76; p.78; p.158–161 |
| C-09 | Spectral C+Shift | Amp smear (table) vs FFT reducer (prose/NRPN) — **resolved by RN 1.8.0** (reducer merged into amp threshold) | p.37; p.49; p.148 |
| C-10 | Tape chord tracking head | First vs lowest note ("TBV") | p.48 |
| C-11 | Grain key trigger values | ON/OFF (p.54) vs Off/On/Legato (p.135) | p.54; p.135 |
| C-12 | Sequencer Rate | "time signature" vs rate division | p.22; p.134 |
| C-13 | Voice sync vs "Clock retrigger" | Screenshot label differs from prose — RN 1.8.0 documents a rename to "voice sync"; label match still to confirm | p.54–55 |
| C-14 | Spawn modes | 4 in prose vs 3 in specs | p.55; p.162 |
| C-15 | Envelope minimum | 0.1 ms (specs) vs 0 ms (MIDI table) | p.163; p.143 |
| U-01 | Undocumented features present in NRPN table | Master EQ, envelope loop modes, grain probability, drawing modulator, mod pages, window param 3, filter mode/routing/bypass (partly clarified by release notes, §16) | p.146–156 |
| U-02 | Undocumented algorithms | Auto-slice detector, gran-filter centroid analysis, grain stealing law, density normalization law, tape slew law, PV hop/overlap, sub-osc waveform, pan-spray distributions | various |
| U-03 | Undocumented file formats | Patch/project/template file formats; sample referencing | p.108–113 |

## 16. Manual vs firmware revisions

### 16.1 Revision scope

| Item | Evidence | Class |
|---|---|---|
| Manual 1.8.d applies to firmware 1.8.x (1.8.0, 1.8.1, …); manual letter suffix = documentation fixes only | p.6 Document version | [MC] |
| Latest firmware at time of research: 1.8.4 (10 Nov 2026); 1.8.1–1.8.4 are bug-fix/QoL releases | Support page; RN 1.8.1–1.8.4 | [MC via RN] |
| Manual screenshots show firmware 1.8.0 pre-release builds (e.g. "FW 1.8.0-a14") | p.54 screenshot | [MC] |
| NRPN table generated from fw 1.8.0 source enum | p.142 | [MC] |
| Hardware revision Oct 2025 changes capabilities: RTPGHI spectral mode, ~10,000 vs ~5,000 total grains, faster internal storage, display switching | p.57; p.162; p.164; p.28 | [MC] |
| Firmware roadmap 1.9–1.12 (Dec 2026 – Jul 2027) exists on the support page; not reviewed | Support page | [UNV] |

### 16.2 Feature-relevant changes documented in firmware 1.8.0 release notes (not, or only partly, in the manual)

Every row below is, by construction, release-note-sourced. The Class column still names one of the four canonical classes (§0) for each fact, with **[MC via RN]** meaning "stated as fact in the release notes with the same confidence as a manual statement" — it is not a separate evidence tier.

| ID | Change | Relevance | Class |
|---|---|---|---|
| RN-01 | Grain probability on Shift + knob A | Resolves GRN-15 control location | [MC via RN] |
| RN-02 | Pitch-quantization per voice / per grain; pitch spray polarity (voicing menu) | Confirms PQ-03, GRN-14 | [MC via RN] |
| RN-03 | Draw XY touchscreen modulator with waypoints | Clarifies MTX-12 | [MC via RN] |
| RN-04 | FFT reducer merged into amp threshold | Resolves SPC-16 / C-09 | [MC via RN] |
| RN-05 | "voice retrigger" renamed "voice sync" | Clarifies SCH-04 / C-13 | [MC via RN] |
| RN-06 | Filter slopes added (Shift + LP/HP cutoff); SVF notch mode is the classic SVF notch | Confirms FLT-02; FLT-03 includes a notch mode | [MC via RN] |
| RN-07 | EQ-3 FX: per-band bypass and visualization | Extends FX-T06 | [MC via RN] |
| RN-08 | Master EQ: Q up to 18, band gain up to 24 dB | Extends SIG-12 | [MC via RN] |
| RN-09 | Elektron-style preset switching while sequencing (System menu → preset change) | Performance workflow not in manual | [MC via RN] |
| RN-10 | Patches saved on ≥1.8.0 change some parameter scaling (e.g. grain rate) vs older firmware | Lesson: version the parameter scaling in our preset format | [MC via RN] |
| RN-11 | Destination lists in matrix/macro menus filtered by category | UX pattern for 149 destinations | [MC via RN] |
| RN-12 | Resonator BANDS/SCALE/MODE no longer assignable to FX knobs (discrete params) | Supports our "Stepped" parameter class | [MC via RN] |
| RN-13 | Bug fix: Random1–4 sources all had the same value | Indicates per-source independent RNG streams intended | [MC via RN] |
| RN-14 | Sub-oscillator quantizes with the main voice quantization | Confirms PQ-04 | [MC via RN] |
| RN-15 | Favorites (stars) in file chooser/manager | Browser UX | [MC via RN] |

### 16.3 Changes in 1.8.1–1.8.4 relevant to the model

| ID | Change | Relevance | Class |
|---|---|---|---|
| RN-16 | "Patch samples as HLAC" — patch/project saves can compress their samples (1.8.1) | Indicates samples are stored with patches/projects (PRS-12) | [RN] / [INT] |
| RN-17 | Project folders renamed `NNN-projectname` (1.8.2); "patch" renamed "preset", `presets` folder (1.8.3) | Storage naming; terminology "preset" | [MC via RN] |
| RN-18 | Presets stored as `.json` with accompanying files; loadable via template type (1.8.4) | Answers PRS-13 partially: preset format is JSON + sample files | [MC via RN] |
| RN-19 | POS X **and POS Y** can modulate per-voice destinations (1.8.4) | A "Pos Y" source exists beyond the manual's source table | [RN] / [UNV] |
| RN-20 | USB-C mode "IMPROVED" (1.8.4); interrupt-storm fix (1.8.1) | Hardware-only | N/A |
| RN-21 | Bug fixes: last slice silent at direction −100 %; looping slices became one-shots; free vs snapped scan mismatch; ping-pong scan not restarting at fader position | Edge cases to cover in our test suite | [MC via RN] |

*End of GR_MEGA_FEATURE_MAP.md*

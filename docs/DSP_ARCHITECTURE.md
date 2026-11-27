# DSP and Application Architecture

**Status:** Investigation only; no framework or implementation approved. This revision (Issue #1) adds candidate module boundaries, concurrency/memory strategy, a framework comparison and recommendations. Recommendations are **not** decisions (DEC-005 remains *Proposed*).
**Version:** 0.5 (Phase 3 architecture proposal reconciliation, 2026-11-27)

**Change summary (v0.4 → v0.5):**
- Reconciled sample/plugin state policy with DEC-025: unsaved recording buffers are embedded; library samples remain external references with relink/collect.
- Reconciled R1 macro count with DEC-036 (ten macros; no R1 expansion to twenty).
- Reconciled R1 MIDI scope: MIDI Clock input in Standalone; no R1 MIDI feedback, clock output or sequencer MIDI output commitment.
- Labeled Granular/Sampler as R1 engines and Slice/Tape/Spectral as later-tier extension points.
- Added cross-references to the Phase 3 threading, parameter, sample/state, plugin bus, Freeze and traceability proposals.

**Status remains:** Investigation and proposal only; no framework, component topology, numeric resource budget or implementation is approved.
**Inputs:** GR_MEGA_FEATURE_MAP.md, GRANULAR_SYNTH_REQUIREMENTS.md, CLAUDE.md engineering principles.

Every item marked **⚑ DECISION** needs ChatGPT review and owner approval before Phase 3 is closed. They are collected in OPEN_QUESTIONS.md §B with options and trade-offs. Nothing here instructs implementation.

**Phase 3 companion proposals:** `PARAMETER_ID_SPECIFICATION.md`, `THREADING_AND_OWNERSHIP_SPECIFICATION.md`, `SAMPLE_AND_STATE_LIFECYCLE_SPECIFICATION.md`, `PLUGIN_COMPONENT_AND_BUS_SPECIFICATION.md`, `FREEZE_BEHAVIOR_SPECIFICATION.md`, and `PHASE3_ARCHITECTURE_TRACEABILITY.md`. Consolidated review proposals are in `PHASE3_ARCHITECTURE_DECISION_REGISTER.md`.

**Evidence convention (owner review of PR #2, 2026-11-27, item 3):** Where this document describes GR-MEGA behavior (e.g. "benchmark does X"), that claim is traceable to a GR_MEGA_FEATURE_MAP.md row and carries that row's evidence class (Manual-confirmed / Interpretation / Proposal / Unverified — see feature map §0). Where this document must specify an *algorithm* the manual does not document — the grain-stealing law (§6.3), the density-normalization law (§6.6), auto-slice detection, the tape slew law, the phase-vocoder overlap/window, or any other internal mechanism GR-MEGA does not publish — that algorithm is **GRANULAR SYNTH's own original design**, not a reconstruction of GR-MEGA's confirmed internal implementation, and is tagged [UNV]/[PROP] in the feature map (see feature map §15, risk R-08). Numeric engineering figures (CPU/memory/timing estimates) are separately marked as calculated estimates in §5.3 and §6.10 and must not be read as measured results.

## 0. Baseline carried over from v0.1

### Goals (unchanged)
- Separate DSP engine from UI, host integration and persistence.
- Reuse a shared instrument engine for standalone and plug-in front ends where feasible.
- Make real-time behavior predictable, measurable and testable.
- Support incremental engine development.

### Real-time constraints (unchanged; refined in §4)
- No synchronous file access in the audio callback.
- No mutex waits or blocking communication in the audio callback.
- Avoid allocation/deallocation in the audio callback.
- Use bounded communication between UI/background and audio threads.
- Prepare sample data off-thread and transfer ownership safely.
- Define deterministic behavior when CPU or voice/grain budgets are exceeded.
- Smooth parameters where abrupt changes could cause discontinuities.
- Test thread safety, denormals, sample-rate changes, buffer-size changes and reset behavior.

### v0.1 candidate modules → v0.2 module mapping (§3)

| v0.1 module | v0.2 location |
|---|---|
| 1. Audio Core | `host/` engine facade + `engine/seq` transport + `core/rt` |
| 2. Voice Manager | `engine/voice` |
| 3. Granular Engine | `engine/grain` + `engine/engines` |
| 4. Sample Store | `sample/` |
| 5. Modulation System | `engine/mod` |
| 6. Effects Graph | `engine/fx` |
| 7. Parameter Model | `state/` parameter registry (§9) |
| 8. Preset/Project Layer | `state/` serialization + loader/housekeeping threads |
| 9. Host Adapters | `host/` |
| 10. UI | `ui/` |

---

## 1. Design goals (in priority order)

1. **Real-time safety** — the audio thread never waits, allocates, or touches the file system.
2. **Sound quality** — click-free under any modulation; better interpolation than the benchmark where CPU allows.
3. **Determinism & testability** — the DSP core is a plain C++ library, host-agnostic, renderable offline with seeded RNGs.
4. **One core, three shells** — standalone, AU and VST3 wrap the *same* engine and state code (GS-PLT-004).
5. **Scalability** — CPU cost degrades gracefully (grain budget, quality tiers) instead of dropping out.

---

## 2. Framework and technology comparison (⚑ DECISION Q-T01, Q-T02)

Criteria are those listed in v0.1: licensing, macOS deployment, AU/VST3 support, UI flexibility, testing, maintainability, developer expertise. Facts verified on 2026-11-27; licence terms must be re-checked at decision time (Q-L04).

| Option | Licensing | AU / VST3 / Standalone | macOS deployment | UI flexibility | Testing / RT tooling | Maintainability & ecosystem | Expertise risk |
|---|---|---|---|---|---|---|---|
| **A. C++20 + JUCE 8** (recommended) | Dual: AGPLv3 or commercial EULA; free Starter tier up to a revenue/funding cap (currently $20k/yr), then Indie (≤$300k) / Pro | All three, plus AUv3; mature multi-bus support | Xcode/CMake, signing & notarization well trodden | Own component toolkit; OpenGL/Metal/WebView options; very flexible | No built-in RT checker; Catch2/GTest; pluginval from same ecosystem | Largest community, long track record | Lowest — most audio C++ examples and hires use JUCE |
| **B. C++ + iPlug2** | Permissive zlib-like licence, free for closed source | VST3, AUv2, AUv3, AAX, CLAP and standalone macOS apps | CMake/Xcode | IGraphics (NanoVG/Skia, HiDPI) — fewer widgets than JUCE | DIY | Smaller maintainer base; some areas pre-release | Medium |
| **C. CLAP-first core + clap-wrapper** | clap-wrapper is MIT; VST3 SDK MIT since 3.8 | CLAP native; wrapper projects to VST3, AUv2 and a simple standalone | DIY packaging | Bring-your-own UI (e.g. JUCE/iPlug2 UI, or native) | DIY; clap-validator | Thin, modern APIs; fewer batteries (no device I/O helpers beyond simple standalone) | Medium–High |
| **D. Rust + nih-plug** | ISC framework; upstream VST3 export used GPLv3 `vst3-sys` bindings; community forks moving to MIT bindings | VST3 + CLAP + standalone; **no AU** (needs clap-wrapper for AUv2) | cargo + bundling scripts | egui/iced/VIZIA — less suited to dense pro UIs | Allocation assertion feature available | Original author no longer maintains it; hard forks continue | High — smaller audio-Rust talent pool |
| **E. Native: Swift/AUv3 + raw VST3 SDK** | Apple SDK + MIT VST3 | AUv3 natural; AUv2 legacy; VST3 separate code path | Best Apple integration, sandboxing | SwiftUI/AppKit | Swift unsuitable for RT DSP → DSP in C++ anyway | Two format integrations to maintain | High |

**Recommendation:** Option A with a framework-free DSP core (Q-T02) so that a later move to B or C costs only the `host/` and `ui/` layers. CLAP can be added to A via the MIT `clap-juce-extensions` project if wanted (OPT).

### Supporting libraries (proposed)

| Concern | Recommendation | Alternatives |
|---|---|---|
| FFT & vector math | Apple **Accelerate/vDSP** behind a thin interface (portable fallback: PFFFT) | FFTW (GPL — avoid), KissFFT (slower) |
| Audio file decode | macOS AudioToolbox (ExtAudioFile) for WAV/AIFF/CAF/FLAC; portable fallback dr_libs/libsndfile | — |
| Offline sample-rate conversion | High-quality windowed-sinc SRC (libsamplerate/r8brain class, permissive licences) | Framework resamplers (lower quality) |
| Build / CI | CMake; GitHub Actions macOS runners | Projucer |
| Tests | Catch2 or GoogleTest; pluginval; `auval` | — |

## 3. Module decomposition

```
granular-synth/
├── core/            # framework-free; header-light; unit-tested
│   ├── rt/          # lock-free SPSC queues, triple buffer, deferred-release pool, RT allocator guards
│   ├── math/        # fast approximations, SIMD helpers, dB/pitch conversions, curve laws
│   ├── dsp/         # interpolators, windows, SVF filters, envelopes, LFOs, smoothers, FFT wrapper
│   └── rng/         # seedable, splittable PRNG (xoshiro/PCG class)
├── sample/          # SampleBuffer, decoding, SRC, analysis (peaks, onsets, centroid map), recorder
├── engine/
│   ├── grain/       # Grain, GrainPool, GrainScheduler, WindowBank
│   ├── engines/     # GranularEngine, SamplerEngine (R1); SliceEngine (L2), TapeEngine (L2), SpectralEngine (L3)
│   ├── voice/       # Voice, VoiceAllocator, chord/arp, quantizer, sub-osc
│   ├── mod/         # ModSource registry, ModMatrix, Macros, per-voice ModState
│   ├── fx/          # FX slot host + effect implementations
│   ├── seq/         # Transport/clock abstraction, StepSequencer (R1; central across four layers)
│   └── layer/       # Layer = patch runtime + voices + mod + FX chain
├── state/           # Parameter registry (IDs, ranges, curves, smoothing class), patch/multi/library model, serialization, migration
├── host/            # Engine facade; plugin adapters (AU/VST3 via JUCE); standalone app shell
└── ui/              # Editor components; reads state via snapshots; never touches DSP objects directly
```

Dependency rule: `ui → host → state/engine → sample → core`. `core` and `engine` must compile and run in a headless test executable.

---

## 4. Threading and real-time model

### 4.1 Threads

| Thread | Owner | Responsibilities | Forbidden |
|---|---|---|---|
| **Audio (RT)** | Host / CoreAudio | Render; apply queued commands; publish meters/positions | alloc/free, locks, I/O, logging, ObjC/Swift runtime, syscalls with unbounded latency |
| **RT workers** (optional, ⚑ Q-T08) | Engine, joined to the host's **audio workgroup** (macOS 11+) | Parallel voice/layer rendering when the grain load is high | Same as audio thread |
| **Message / UI** | App / host | Editor, parameter edits from UI, preset browser | Blocking the audio thread in any way |
| **Loader** | Engine | Decode, SRC, normalize, analysis (peaks, onsets, centroid map), mip-map build | Touching live DSP objects |
| **Housekeeping** | Engine | Deferred destruction of retired objects, autosave, disk writes for recordings | — |

### 4.2 Communication patterns

| Direction | Mechanism |
|---|---|
| Host automation → DSP | Parameter values in atomics (relaxed) read once per block; smoothing done in DSP |
| UI/loader → DSP (structural changes: new sample, new FX type, matrix row edit, preset load) | **Command objects** pushed into a lock-free SPSC FIFO. Commands carry *fully pre-built* objects (e.g., a ready SampleBuffer, a constructed effect). The audio thread swaps pointers only. |
| DSP → UI (grain positions, meters, record head, CPU) | Triple buffer / seqlock snapshot written once per block; UI polls at 30–60 Hz |
| DSP → housekeeping (retired objects) | "Garbage" SPSC FIFO; objects destroyed off the RT thread |
| Recording data → disk (resample to WAV) | Lock-free ring buffer drained by housekeeping thread |

### 4.3 Preset/patch change protocol (click-free, RT-safe)

1. Message thread builds the complete new `LayerRuntime` (parameters, matrix, FX instances, sample handle) off-thread.
2. Command enqueued. Audio thread begins a short fade (≈5–10 ms) on the outgoing layer, or—per FX tail setting (GS-FX-006)—lets FX tails ring.
3. Pointer swap at fade end; old runtime goes to the garbage queue.

### 4.4 RT-safety enforcement

- Debug/test builds install an allocation hook (operator new/malloc interposition) that asserts when called on a thread flagged as RT.
- CI runs an offline render of a stress-preset set with the hook active.
- Clang thread-safety annotations on shared structures; `[[clang::nonblocking]]` / RealtimeSanitizer (RTSan, available in recent Clang) evaluated for CI (⚑ Q-T09).

---

## 5. Sample management

### 5.1 SampleBuffer

- Immutable after construction; **de-interleaved float32**; mono or stereo.
- **Guard padding** (e.g. 64 frames) before start and after end so interpolators never branch on bounds; padding content depends on loop mode (zeros vs wrapped copy), regenerated when flags change by building a *new* buffer view off-thread.
- Reference-counted handle; the audio thread never drops the last reference (it hands the handle to the garbage queue instead).
- Stores: native sample rate, length, channel count, peak overview (multi-resolution, for waveform), optional analysis products (onset list for auto-slice, spectral-centroid map for gran filter, zero-crossing index).

### 5.2 Rate handling (⚑ Q-T04)

| Option | Pros | Cons |
|---|---|---|
| **A. Keep source rate; per-grain increment includes `srcRate / hostRate`** (recommended) | No offline SRC latency; memory not inflated at 96/192 kHz; one code path | Interpolator quality matters more when rates differ |
| B. Convert to host rate on load (benchmark converts to 48 kHz optionally) | Simplest playback | Reload on host rate change; 2–4× memory at high rates |
| C. Convert to a fixed internal rate (48 kHz) and run the engine at 48 kHz with SRC at the I/O | Matches benchmark exactly | Adds latency and SRC cost on output; poor plugin citizenship |

### 5.3 Memory budget

**[ESTIMATE — owner review item 2, 2026-11-27.]** The figures below are arithmetic from fixed constants (sample rate × bytes/sample × channels × duration) for raw audio payload only. They are **not measured process memory** and exclude allocator overhead, analysis buffers (peak pyramid, onset/centroid data), guard padding, and runtime structures — actual RSS will be higher and has not been measured on any build, because no build exists yet. Treat as a sizing-order-of-magnitude input to Q-T05, not a memory budget commitment.

Stereo float32 @ 48 kHz = 384 kB/s (exact arithmetic, not an estimate).

| Content | Size (raw audio payload only) |
|---|---|
| 5 min 49 s stereo (benchmark granular ceiling) | ≈ 134 MB |
| 6 min stereo per layer (GS-SMG-019) × 4 layers | ≈ 553 MB |
| + full octave mip-map pyramid (L2, optional) | × ≈ 2.0 (rough multiplier, unmeasured) |
| 60 min stereo sampler (L2) | ≈ 1.38 GB → disk streaming or 16-bit RAM storage |

Recommendation (proposal, not a decision): **RAM-resident** for granular/slice/spectral (grains access memory randomly; streaming is impractical), with a per-instance memory gauge (benchmark has RAM/disk gauges, SMG-09). Disk streaming only for the long-sample Sampler mode (L2, ⚑ Q-T05). Optional 16-bit integer storage (with float conversion on read) halves RAM at negligible cost if needed.

### 5.4 Loading pipeline (Loader thread)

```
file → decode (AudioToolbox) → float32 de-interleave → [optional normalize]
     → peak pyramid (progressive: publish coarse overview first)
     → analysis jobs (onsets, centroid map, zero crossings) — cancellable, results delivered as separate commands
     → SampleBuffer complete → command to audio thread → pointer swap at next note / with crossfade
```

The previous sample keeps playing until the swap (GS-SMG-003). Analysis that is only needed by specific features (gran filter, auto-slice) runs lazily.

### 5.5 Live recording buffer

- Pre-allocated to the configured size (AUTO mode grows in **pre-allocated chunks** prepared by the loader thread before they are needed — never allocates on RT).
- Single writer (audio thread, input callback), many readers (grains). Write index published with release semantics.
- Readers near the write head: grain positions clamped to a **guard distance** behind the record head when "scrolling record" is active; in loop-record mode grains may read across the wrap point by design (benchmark behavior).
- Feedback path (synth → record, L2) writes post-mix of the *previous* block to avoid an algebraic loop.
- Background WAV writing via ring buffer to the housekeeping thread.

### 5.6 Plugin input for live granulation (product scope approved; implementation Q-P06)

- **VST3:** instrument with an optional stereo audio input bus (main or sidechain) — widely supported.
- **AU:** an instrument (`aumu`) has no main audio input in typical hosts, and side-chain support for instruments is host-dependent. The established route for a MIDI-played audio processor is the **music-effect** (`aumf`) type, which Logic Pro places in an audio-effect / MIDI-controlled-effect slot rather than the instrument slot. Option: ship both `aumu` and `aumf` variants of the same binary. Live input in plug-in contexts is an approved product requirement (DEC-020). The exact component/bus strategy remains an architecture decision: verify host behavior in Logic, Live and other target hosts before selecting the AU/VST3 implementation.

---

## 6. Granular engine

### 6.1 Data structures (illustrative, not code)

```
Grain (≈ 64–96 bytes, POD, structure-of-arrays inside GrainPool for SIMD):
  readPos        : double  (source frames; double avoids drift on 5-min buffers)
  readInc        : double  (±, includes pitch, direction, srcRate/hostRate)
  winPhase       : float   (0..1)
  winInc         : float   (1 / grainLengthInOutputSamples)
  winShape       : WindowSnapshot (params captured at spawn, see 6.4)
  gainL, gainR   : float   (pan, pan spray, chord/density normalization)
  chordIndex     : uint8
  birthPitch     : float   (for Local pitch ON)
  state          : Active | Stealing(fadeRemaining)

GrainPool: fixed capacity 128 per voice, preallocated per voice at instance creation.
GrainScheduler (per voice): phase accumulator in output samples, current interval,
                            sync state, spawn-mode state, RNG stream.
```

### 6.2 Scheduling algorithm

Per render block, per voice:

1. Resolve **effective rate and size** from the ratio mode (GRN-20…25):
   - Free: rate = A, size = B
   - DensitySize: size = B, rate = D / size
   - DensityRate: rate = A, size = D / rate
   - ScanRate: rate = A·|scan|, size = B / |scan| (with ε-floor and defined behavior at scan = 0 — ⚑ Q-M03)
   - ScanOverlap: rate = A·|scan|, size = B
   - TimeStretch: 2 grains, interval = size/2, constant output overlap
2. If **sync** is on, the interval comes from the transport (host playhead / internal clock) at the chosen division; spawn times aligned to the musical grid. With **voice sync** on, all voices of the layer share one phase.
3. Emit spawns at **fractional sample offsets** inside the block (sub-sample accurate: the first output sample of a grain is computed with the fractional phase applied to both read position and window phase).
4. For each spawn:
   - position = basePos(scan-integrated) + spray·rand(polarity) (Gran-filter weighting if enabled, L2)
   - direction = rand < dirProbability ? +1 : −1
   - pitch = notePitch + pitchKnob + pitchSpray·rand(polarity) + mod, then chord/arp note, then scale quantization (per grain or per voice)
   - pan = pan + panSpray·dist(rand)
   - optional grain probability skip (L2)
5. **Spawn modes** on note-on: Gradual (start scheduler at phase 0), Direct (pre-populate `round(density)` grains with randomized window phases so the cloud is already "full"), Direct reset (same, but all positions within the spray range), Recycle (revive the last voice's grain set).
6. **Idle shutdown:** a voice with no active grains and no gate stops scheduling (no CPU), mirroring SCH-06.

### 6.3 Grain stealing (benchmark: click-free, shortens grains)

When `active ≥ capacity × (1 − threshold·k)`: select the *oldest* (or longest-remaining) grains and switch them to a **short release ramp** (e.g. 2–10 ms, raised-cosine) instead of truncating. Threshold 0 = no stealing (scheduler skips spawns when full, reproducing the benchmark's "bunching"). Exact law is undocumented (U-02) and will be tuned by listening tests.

A second, **global** budget (GS-ARC-004) applies the same mechanism across voices/layers, preferring to steal from voices in release.

### 6.4 Windows

- PowAR and Raised-Cosine families, parameters: tilt, curve, sides (PowAR), AM (raised cosine).
- **Window snapshot per grain:** at spawn, the grain captures the window parameters (or a pointer to an immutable precomputed table version). Changing the window affects only new grains → eliminates the benchmark's crackle (WIN-08 → WIN-P1).
- Implementation choice (⚑ Q-T06): (a) analytic evaluation per sample with fast pow/cos approximations; (b) table bank: tables regenerated off-thread when parameters change by more than a tolerance, versioned, ref-held by grains. Recommendation: (b) for PowAR/RC base shapes + analytic AM multiplier — cheap, deterministic, modulation-safe.

### 6.5 Interpolation (GS-GRN-020)

| Quality | Method | Cost (rel.) | Use |
|---|---|---|---|
| Draft | Linear | 1× | Benchmark-equivalent granular |
| Standard (default) | 4-point cubic Hermite | ~2× | Granular/slice default |
| High | 8–16-tap polyphase windowed sinc | ~5–8× | Sampler/tape; granular when CPU allows |
| + Mip-map (L2) | Read from octave-down pre-filtered copy when `|readInc| > 1` | small | Reduces aliasing on upward pitch cheaply |

### 6.6 Gain normalization

Benchmark: voice gain −12 dB default; normalization reaches max level at density ≈17 (SIG-08/09). Proposal: `g = 1 / sqrt(max(1, density / d0))` (energy-preserving for uncorrelated grains) with `d0` tuned so the benchmark's "max volume at density 17" behavior is matched; ON/OFF per patch. ⚑ exact law to be validated by measurement (Q-M09).

### 6.7 Chord distribution and arp

Granular engines: each new grain takes the next chord note per arp mode (up, down, down-up, up-down, random, shuffle, forward, backward, fwd-bwd, bwd-fwd); level unchanged because grains are distributed, not multiplied. Other engines: chord = N play heads/copies, level stacks (CHD-04).

### 6.8 Per-grain vs continuous parameters

| Class | Examples | Applied |
|---|---|---|
| **Spawn-sampled** | position, spray, pitch, pitch spray, pan, pan spray, direction, size, window snapshot | Read once when a grain is born. Naturally zipper-free. |
| **Grain-continuous** | pitch when Local pitch OFF, global pan of running grains (optional) | Updated per control sub-block, smoothed |
| **Voice-continuous** | filter cutoff/res, amp, sub-osc level | Smoothed per sample / sub-block |

This classification is part of the parameter registry (see §9).

### 6.9 Rendering loop optimisation

- Structure-of-arrays grain storage; process grains in groups of 4/8 with NEON/SSE (Universal binary).
- Window and interpolation computed per grain per sample, accumulated into per-voice stereo buffers; filters run once per voice after summation (benchmark topology).
- Avoid per-sample branches: loop-mode handling via guard padding and per-grain "samples until boundary" precomputation; handle wrap/bounce at segment boundaries.
- Denormal protection (FTZ/DAZ) on RT threads.

### 6.10 CPU feasibility (key risk)

**[ESTIMATE — owner review item 2, 2026-11-27: not a reproducible measurement.]** Benchmark worst case ≈ 10,240 grains × 48,000 = **491 M grain-samples/s** is a specification-derived maximum (manual grain/voice limits × sample rate), not a measured figure. With cubic stereo interpolation (~25–30 flops per grain-sample, itself a calculated assumption about our own future implementation) that is a **calculated estimate** of ≈ 12–15 GFLOP/s. Whether this is actually beyond one CPU core on the eventual reference machine (Q-P10) has not been benchmarked on any hardware or any GRANULAR SYNTH code — there is no code yet. Treat "beyond a single core" as a planning hypothesis that motivates the mitigations below, not an engineering conclusion. It must be confirmed by a real measurement in Phase 3 (DEVELOPMENT_ROADMAP.md Phase 3 exit criterion 3) before any budget default is fixed.

Planned mitigations (proposals, not yet implemented or validated):

1. **Global grain budget** (default to be set from an actual benchmark measurement on reference hardware, Q-T07 — no default exists yet), with **click-free stealing** above it. "Click-free" here is a design goal for the stealing algorithm (§6.3), not a measured or demonstrated property — it has not been implemented or listened to yet.
2. **Quality tiers** per layer (Draft/Standard/High) — proposed, unimplemented.
3. **Optional parallel rendering** of layers/voices on RT worker threads joined to the host's audio workgroup (⚑ Q-T08) — a strategy proposal, not a decision; feasibility and host support are unverified. Not all hosts expose workgroups; must fall back cleanly to single-threaded.
4. Typical musical patches (density 4–40, 8 voices) are far below the calculated worst case; the budget is intended to protect only extremes — this too is an expectation, not a measured result.

---

## 7. Other engines (common interface)

All engines implement one voice-level interface: `noteOn / noteOff / render(block, ModState) / isActive / reset`. Engines share position/scan/flags/looping services so behavior is identical across engines where the benchmark says it should be (ARCH-02).

| Engine | Core model | Notes |
|---|---|---|
| **Granular Slice** (L2 — deferred, DEC-018) | GranularEngine with position fixed to slice start/region; key → slice index; per-slice filter override (L2) | Slice table immutable, swapped like samples; auto-slice runs on loader thread with preview overlay (benchmark crackles here; we must not) |
| **Sampler** (R1) | 1 play head (+chord heads ≤ 12); loop confinement with one-shot / wrap / bounce; looped release; optional loop crossfade | Polyphase interpolation; long-file streaming L2 |
| **Tape** (L2) | Target position = slider + scan + mod; head follows via critically-damped 2nd-order follower with slew time; readInc = Δhead per sample → pitch from motion; chord heads as pitch-bent copies | Needs high-quality interpolation and anti-aliasing at extreme speeds; reverse and stop behavior must be click-free |
| **Spectral** (L3) | STFT phase vocoder; FFT 256–8192, Hann window, 4× overlap (⚑ Q-T11); per-bin magnitude/phase manipulations: threshold, phase smear (phase randomization), amp smear (magnitude smoothing across bins), freeze (magnitude/phase hold with decay), bend (log-map warp vs linear shift), fold, pitch blend (second shifted layer), formant lock/shift (cepstral or LPC envelope), spectral window shaping | Fixed per-voice latency = FFT size; quality mode with phase-gradient heap integration (published RTPGHI method); chord ×12 by resynthesizing pitch-shifted copies from one analysis |

---

## 8. Voice and layer architecture

```
Instance
 ├─ Transport/Clock  (host playhead | internal | MIDI clock)
 ├─ Global grain budget, RNG root, meters
 ├─ Layer[4]
 │    ├─ PatchRuntime (parameter values, matrix, macros, slices, flags, FX chain)
 │    ├─ SampleBuffer handle (+ recorder if armed)
 │    ├─ Layer mod sources: LFO1–4 (mono mode), Random seed, follower, seq mod lanes (L2)
 │    ├─ VoiceAllocator (polyphony 1–20, stealing policy, glide, env pile-up, MPE note→voice map)
 │    ├─ Voice[20] (preallocated)
 │    │     ├─ Engine voice state (scheduler + GrainPool[128] | play heads | STFT state)
 │    │     ├─ Env ×4 (pitch, filter, amp, aux), Random1–4 (per note), poly LFO state (if Poly)
 │    │     ├─ Sub-osc, LPF (SVF cascade), HPF (SVF cascade)
 │    │     └─ ModState (per-voice evaluated destinations)
 │    ├─ FX chain [4 slots] + audio-in send + synth send
 │    └─ M-S → patch volume → layer output bus
 └─ Layer mix → wet; + dry input → master → main out (+ 4 layer outs)
```

- **Voice allocation:** reuse same-note voice when env pile-up is ON; otherwise steal: released & not sustained → oldest → keyboard-position heuristic (benchmark VOI-02), with a 2–5 ms fade instead of an abrupt cut (GS-VOI-001).
- **MPE:** each voice binds to a member channel; per-note pitch, pressure, timbre feed voice-level mod sources.
- **Filters:** TPT state-variable filter (benchmark hints "trap-SVF", FLT-03); 6 dB = 1-pole TPT; 12/18/24 dB by cascading; cutoff modulation at audio rate safe by construction.

---

## 9. Parameter system

### 9.1 Registry

Every parameter is declared once with: stable string ID (`L1.grain.size`), numeric host ID, display name, range, unit, **response curve** (linear / cubic / exponential / log — benchmark uses these per parameter, see manual MIDI table), default, **smoothing class**, modulatable flag, automatable flag, MIDI default CC.

### 9.2 Smoothing classes

| Class | Behavior | Examples |
|---|---|---|
| `Audio` | Per-sample one-pole or linear ramp (5–20 ms) | volumes, pan, filter cutoff, FX mix |
| `Control` | Linear ramp over control sub-block (16–32 samples) | LFO rate, envelope amounts |
| `Spawn` | No smoothing; sampled at grain birth | grain size, spray, position for new grains |
| `Stepped` | Discrete; change applied at safe point with crossfade if needed | engine type, FFT size, filter slope, window type, ratio mode |
| `Structural` | Requires runtime rebuild off-thread | FX type, sample, slice table |

### 9.3 Modulation evaluation

- Control rate: proposed 16-sample sub-block (Q-T13), subject to measurement and approval; per voice for polyphonic sources and per layer for shared sources.
- `effective = curve(base + Σ (srcᵢ · amountᵢ · polarityᵢ · curveᵢ) + macro contributions)`, clamped to range.
- Audio-class destinations interpolate linearly across the sub-block.
- Spawn-class destinations are read at grain birth from the latest sub-block value.
- Matrix: 50 rows; sources registry as benchmark (25) + our additions (XY pad); destinations = R1 modulatable parameters.
- Macros: exactly ten per patch in R1 (DEC-036); each macro = shared per-layer base value plus destination mappings. Per-note sources may produce per-voice destination values under DEC-031. Macros are host-automatable and may be modulation sources/destinations where specified.

### 9.4 Host exposure (⚑ Q-T12)

Expected parameter count ≈ 250–350 per layer → ~1,000–1,400 total. Proposal: expose **all** automatable parameters with stable IDs, grouped by layer (VST3 Units / AU parameter groups), with the ten R1 macros per patch (DEC-036) and approved performance controls prioritized in discoverable groups for quick DAW mapping. Structural parameters (sample, FX type) are not host-automatable.

---

## 10. Effects and mixing

- Per-layer chain of 4 slots; each slot hosts any effect implementation through one interface (`prepare / process / reset / tail`).
- Effects constructed off-thread when type changes; swapped in via command; old instance retired via garbage queue; short crossfade on swap.
- Synth send and audio-in send with −100…+24 dB gain feed the chain (FX-02).
- Reverbs/delays implement `tailLength` for host tail reporting; "FX buffer reset" option clears state on preset change.
- Master bus: optional EQ/limiter (L3/OPT).
- All FX parameters registered in the parameter registry → automatable and modulatable.

---

## 11. Transport, clock, sequencer

- **Transport abstraction**: provides musical time (PPQ position, tempo, bar, playing state) per block from (a) host playhead (plugin), (b) internal clock (standalone), (c) MIDI clock in (standalone, R1), and (d) Ableton Link (R1 product requirement; adapter design to be validated).
- Sample-accurate events: tempo-synced grain spawns, LFO phase reset, sequencer steps computed from PPQ ranges within each block.
- Sequencer (R1, DEC-019): one central pattern/step timeline controls all four layers; events are assigned to layers and turned into internal grain triggers, note events and parameter/modulation overrides. Pattern/event data is persistent state; runtime scheduling and parameter overrides must be bounded and real-time safe. Detailed behavior is in R1_PRODUCT_SPECIFICATION.md. Do not assume the sequencer is per-layer.

---

## 12. Plugin and standalone compatibility

| Topic | Plan |
|---|---|
| Shared core | Identical `Engine` object in all formats; format code only adapts buses, MIDI, parameters, state |
| Buses | Main stereo out + 4 optional stereo layer outs; required live-input capability (VST3 input bus; AU component/bus strategy to be validated — Q-P06) |
| MIDI | Notes, CC, MPE, pitch bend and Program Change where host delivers it; R1 MIDI Clock input in Standalone; MIDI feedback, clock output and sequencer MIDI output are not R1 commitments |
| State | Versioned, self-describing state; unsaved recording buffers embedded in plugin state; library samples remain external references with relink/collect (DEC-025) |
| Latency | 0 for granular/sampler/tape; spectral latency reported when it processes live input (Q-T10) |
| Sample rate / block size | Fully variable; all time constants in seconds; `prepare()` rebuilds rate-dependent tables off the RT path |
| Offline render | Deterministic; uses same code paths; respects host "realtime vs offline" flag only for quality-tier boosts (optional) |
| Standalone extras | Audio device & MIDI device selection, input monitoring, internal clock, MIDI Clock input, Ableton Link integration, file-based projects |

---

## 12a. Live-performance / controller-only operability (owner decisions DEC-015, DEC-016, 2026-11-27)

Architectural implications of DEC-015 (both studio and live performance are primary) and DEC-016 (standalone must be fully operable for essential live operations via MIDI controller alone). Full behavioral specification is in docs/LIVE_PERFORMANCE_SPECIFICATION.md; this section only notes what falls out of it for §4 (threading) and §9 (parameter system). None of this changes the framework decision (Q-T01) or any engine/scope decision (DEC-008/009/010).

- **Parameter registry (§9):** the registry should be able to flag a parameter as "performance-critical" / grouped for fast controller mapping, independent of its automatable/modulatable flags. This is a registry *capability*, not a decision about which parameters get flagged — that is Phase 4/5 work.
- **MIDI Learn (GS-MID-003) → structural state:** global and per-preset controller mappings are structural data (like a patch), not audio-thread state — they follow the existing Command-object / Structural-smoothing-class path (§4.2, §9.2), not a new communication mechanism.
- **Controller feedback:** not required in R1 (DEC-021). If added in a later release, sending MIDI back to a controller is Message/UI-thread work (§4.1), never audio-thread work.
- **Standalone MIDI I/O:** R1 requires MIDI input for controller operation and external MIDI Clock. MIDI feedback/output to controller LEDs or motorized faders is deferred and is not a R1 dependency.
- **Sequencer and plug-in live input:** product scope is approved by DEC-019/DEC-020. Detailed scheduling, AU/VST3 component and bus integration, threading and host validation remain Phase 3 architecture work.

---

## 13. CPU optimisation checklist

1. Idle voices and idle grain schedulers cost zero.
2. SoA grain storage + SIMD across grains.
3. Guard-padded buffers → branchless interpolation.
4. Window tables with versioning instead of per-sample transcendental math.
5. Control-rate modulation (16–32 samples) with linear interpolation for audio-class targets.
6. Filters per voice, not per grain.
7. Quality tiers + global grain budget.
8. Optional parallel layer/voice rendering on workgroup-joined RT workers.
9. vDSP for FFT, mixing, gain ramps.
10. Profiling harness with fixed stress presets; per-category CPU meter (voices / FX / housekeeping) like the benchmark (PRF-07).

---

## 14. Testability hooks (detail in TESTING_STRATEGY.md, Phase 5)

- Headless offline renderer: preset + MIDI file + seed → WAV; used for golden-file regression with tolerance.
- Unit tests: interpolators (frequency response/aliasing), windows (shape, sum-to-constant at known overlaps), scheduler timing (spawn jitter = 0 at integer and fractional rates), ratio-mode math, stealing (no discontinuity > threshold), envelopes (times, curves), SVF (magnitude response), loop/bounce/one-shot edge cases, parameter curves.
- Discontinuity detector (max |Δ| and high-frequency energy spike) run on every golden render.
- RT-safety: allocation/lock detector, RTSan (if adopted).
- Plugin validation: `auval`, pluginval strictness 10, host smoke tests.

---

## 15. Architectural risks

| # | Risk | Impact | Mitigation |
|---|---|---|---|
| R-01 | Worst-case grain load exceeds one core | Dropouts | Global budget, quality tiers, optional workgroup threading |
| R-02 | Parameter-ID scheme changed after release | Breaks user automation/presets | Freeze ID scheme in Phase 3 (Q-T12) |
| R-03 | Plugin state size with embedded unsaved recordings (potentially large) | Slow DAW saves, host limits, host state-size constraints | Implement DEC-025: unsaved recording buffers are embedded; define bounded serialization, chunking/failure behavior and save/restore tests; library assets remain references |
| R-04 | AU live-input requirement | Two AU variants, Logic UX differences | Decide early (Q-P06) |
| R-05 | Spectral engine CPU/latency | Dropouts with multiple layers | Defer to L3; per-instance limit; latency reporting |
| R-06 | Window/parameter modulation clicks | Quality complaints | Spawn-sampled params, window snapshots, smoothing classes |
| R-07 | Large sample loading on 4 layers | Memory pressure | Memory gauge, 16-bit storage option, limits |
| R-08 | Undocumented benchmark algorithms (stealing, normalization, auto-slice, gran filter) | Behavior differs from GR-MEGA | Treat as our own design; listening tests; owner sign-off |
| R-09 | Framework licence terms | Cost/legal | Review JUCE EULA before commitment (Q-L04) |

---

## 16. Decisions requiring approval (index)

| ID | Decision | Recommendation |
|---|---|---|
| Q-T01 | Language & framework | C++20 + JUCE 8 |
| Q-T02 | DSP core independence | Framework-free core library |
| Q-T03 | macOS minimum version & architectures | macOS 12+, Universal 2 (Q-T03 lists alternatives) |
| Q-T04 | Sample-rate handling | Keep source rate, rate-compensated increments |
| Q-T05 | Long-sample strategy | RAM for granular; streaming for sampler (L2) |
| Q-T06 | Window implementation | Versioned table bank + per-grain snapshot |
| Q-T07 | Grain budget & reference hardware | Benchmark first, then fix default |
| Q-T08 | Multithreaded rendering | Design for it, enable after R1 profiling |
| Q-T09 | RT-safety tooling | Allocation hook + RTSan in CI |
| Q-T10 | Spectral latency policy | Report latency only when processing live input |
| Q-T11 | PV overlap/window | Hann, 4× overlap baseline |
| Q-T12 | Parameter ID scheme & host exposure | Stable string IDs, expose all, grouped by layer |
| Q-P06 | Plugin audio input / AU type | VST3 input bus + AU `aumu` and `aumf` variants |
| Q-P08 / DEC-025 | Unsaved recording buffers embedded in plugin state; library samples remain external references with relink/collect |
| Q-P02 | Plugin formats | AU + VST3 (R1); CLAP optional |
| Q-P03 | Distribution, signing, notarization | Decide before Phase 6 |
| Q-P20 | First-release engines | Granular + Slice + Sampler; Tape L2; Spectral L3 |

All items from the v0.1 "Decisions required" list (framework/language; threading model; target macOS and CPU architectures; initial release engines; sample formats and streaming; measured CPU/memory budgets; plugin formats and distribution/signing) are covered by the rows above.

*End of DSP_ARCHITECTURE.md*

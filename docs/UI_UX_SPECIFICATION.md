# UI/UX Specification

**Status:** Discovery not started (Phase 4 proper has not begun). This revision only records two owner-approved constraints that this future discovery work must satisfy; it does not start Phase 4 or decide any screen/workflow design.

## Owner-approved constraints (record only; detailed design is Phase 4)
- **DEC-015 (2026-11-27):** both studio sound design and live performance are primary use cases — the UI must serve both, not treat live performance as secondary.
- **DEC-016 (2026-11-27):** the standalone UI's essential live-performance operations must be fully reachable via MIDI controller, without keyboard or mouse (GS-MID-008, GS-UI-007). See docs/LIVE_PERFORMANCE_SPECIFICATION.md for the controller-mapping architecture, MIDI Learn UX, conflict handling and acceptance criteria this implies — that document is a specification input to Phase 4, not a substitute for it.

## Design objectives
- Fast access to core granular sound-shaping controls.
- Support detailed sound design and immediate musical performance.
- Keep high-frequency performance controls accessible.
- Visualize sample position, grain activity, modulation and signal level.
- Remain usable at common macOS display sizes and scaling settings.
- Make every essential live-performance control reachable and learnable via MIDI controller (DEC-016); keyboard/mouse remain required only for setup-time operations.

## Areas to investigate
- Main instrument and layer navigation.
- Sample browser/editor.
- Waveform and spectrogram display.
- Grain visualization and parameter editing.
- Modulation routing.
- Sequencer and performance controls.
- Effects and preset browser.
- MIDI learn and parameter automation, including global vs. per-preset mapping UI and conflict resolution (see docs/LIVE_PERFORMANCE_SPECIFICATION.md).
- Controller feedback presentation (motorized faders, LED rings, displays) where a connected controller supports it.
- Accessibility, keyboard navigation and scaling.

Extract workflows from the reference; do not copy hardware controls literally without a software UX rationale.

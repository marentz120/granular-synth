# FX Module Integration Plan

**Version:** 0.1 (planning baseline, 2026-11-28)  
**Status:** Planning only; no code, API, framework, DSP algorithm, third-party dependency or implementation is approved.  
**Decision authority:** DEC-023 (R1 effect scope), DEC-056 (delivery sequencing).  
**Related documents:** `EFFECTS_SPECIFICATION.md`, `DEVELOPMENT_ROADMAP.md` M14, `PHASE5_DECISION_CLOSURE_REGISTER.md`.

## 1. Purpose

Prepare GRANULAR SYNTH for a self-contained, modular FX subsystem that can be integrated at the end of Phase 6 without redesigning the completed instrument core.

In this document, **FX module / add-in module** means an internal, separately bounded product subsystem. It does **not** mean a user-installable third-party plug-in format, public SDK, binary module loader or external effect marketplace. Those would require separate product and technical decisions.

## 2. Scope retained for R1

DEC-023 remains in force. The final R1 FX module must deliver:

- Four reorderable FX slots per layer.
- Delay, Ping-pong Delay, Reverb, Chorus, EQ3, Saturator and Compressor.
- Per-slot dry/wet, bypass and ordering behavior.
- Audio-input and synthesized-signal sends.
- Automation and modulation for eligible continuous FX parameters.
- Patch/preset state integration and copy/paste behavior.
- Linger/Cut tail policy on preset switching.
- FX-specific UI and observable acceptance coverage.

This plan changes delivery order only. It does not remove effects from R1 or authorize shipping R1 without them.

## 3. Planned module responsibilities

The final module is expected to own or coordinate these product responsibilities:

1. **Slot management:** four ordered slots per layer, enabled/disabled state, effect type and wet/dry state.
2. **Effect implementations:** the seven approved R1 effect types, each with its own documented controls.
3. **Signal routing:** layer synth path and supported live-input send into the layer FX chain.
4. **Parameter integration:** stable parameter identity, automation and modulation integration under the approved global parameter model.
5. **State integration:** save/restore, migration, copy/paste and invalid/unknown effect-state handling.
6. **Performance/UI integration:** effect selection, slot reorder, bypass, parameter editing and accessible value display.
7. **Tail lifecycle:** Linger/Cut behavior during preset changes and safe slot replacement.
8. **Validation:** audio correctness, discontinuity behavior, real-time safety, resource use and host consistency.

This is a responsibility inventory, not a class diagram or API specification.

## 4. Preparation required before M14

Earlier milestones should preserve these conditions without implementing FX:

- The layer signal-flow and live-input routing documentation must identify the future FX insertion point.
- The parameter model must be capable of registering future per-layer FX parameters, with stable identity and automation/modulation metadata once Q-T12 is approved.
- Preset/state planning must leave room for versioned FX slot state and migration, without freezing an unapproved FX schema.
- UI navigation/layout planning should reserve a coherent location for layer FX controls, but must not build FX-specific controls before M14.
- The audio lifecycle and structural-change contracts must be reviewed for safe future effect creation, replacement, reordering and retirement. The actual mechanism remains an engineering decision.

No placeholder DSP, third-party DSP package or empty public extension API is required merely to satisfy this preparation plan.

## 5. M14 entry gates

M14 must not begin until:

- Core engine, sampler/granular voice path, four-layer model, state baseline, sequencer and primary UI milestones are accepted.
- The approved technical architecture and parameter registry decisions needed by FX are closed by their proper authority.
- The FX parameter contract is complete: units, ranges, defaults, EQ3 crossover constraints, delay sync/time semantics, send behavior and automation scaling.
- Each of the seven effect types has a bounded, reviewable implementation and validation plan.
- Dependency and licence review is complete for any proposed third-party code. No dependency is preselected by this plan.

## 6. M14 exit gates

- All seven R1 effect types are available in the product.
- Four-slot per-layer routing, reorder, bypass, wet/dry and send behavior meet the approved specification.
- Eligible FX parameters participate correctly in host automation and the modulation matrix.
- Preset save/restore, copy/paste, migration and Linger/Cut behavior are covered.
- FX UI is complete and consistent with the approved Phase 4 interaction specification.
- Audio correctness, parameter transitions, real-time safety, CPU/memory, multi-layer load and host behavior have passed their approved tests.
- No effect or third-party dependency is shipped without licence and provenance review.
- FX requirement traceability and acceptance evidence are complete before Phase 6/R1 exit.

## 7. Explicit non-goals

- No public third-party add-in SDK or external plug-in hosting.
- No user-loadable effect binaries or marketplace.
- No commitment to JUCE, Signalsmith, DaisySP or another DSP package.
- No copying of GR-MEGA proprietary code, firmware, assets or protected design.
- No DSP implementation, benchmark, test execution, PR merge or release authorization.

## 8. Open dependencies

The FX parameter contract remains open in `PHASE5_DECISION_CLOSURE_REGISTER.md`. Technical architecture, parameter IDs, threading/ownership, host integration, DSP algorithm selection, numeric resource budgets and dependency licensing remain subject to their existing decision gates.

## 9. Change control

Any proposal to remove one of the seven R1 effect types, move FX outside R1, introduce a public extension SDK, or authorize early FX implementation requires a separate explicit owner decision. DEC-056 only changes sequencing.

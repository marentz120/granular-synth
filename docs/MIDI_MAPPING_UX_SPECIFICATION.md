# GRANULAR SYNTH — MIDI Mapping UX Specification

**Version:** 0.1  
**Status:** Phase 4 discovery draft — owner review required  
**Authority:** `LIVE_PERFORMANCE_SPECIFICATION.md`, DEC-016 and DEC-021 remain authoritative. This document specifies UI workflows only.

## 1. Goals and constraints
- Generic class-compliant MIDI controllers; no named-controller templates in R1.
- MIDI Learn plus manual CC, channel and Note assignment.
- Soft takeover to avoid parameter jumps.
- Every essential live operation remains controller-reachable.
- R1 does not require MIDI feedback (DEC-021).

## 2. Mapping workspace
Provide a MIDI Mapping workspace with:
- Mapping list: source, channel, message type, target, scope and enabled/invalid state.
- Learn action and manual mapping editor.
- Search/filter by target, source and layer.
- Conflict and invalid-target indicators.
- Explicit Unmap and Reset Scope actions.

Provide a performance-safe way to enter/exit Learn when needed mid-set. Exact shortcut remains open.

## 3. MIDI Learn workflow
Proposed flow:
1. Select a target control/parameter.
2. Activate Learn.
3. Move/send the desired control on the physical MIDI device.
4. Display captured message type, channel, number and resolution.
5. Confirm or cancel.
6. Show the resulting mapping in the list.

Learn must not silently replace an existing mapping. Unsupported or ambiguous events must be explained and allow retry. Learn entry/exit must be reachable without keyboard/mouse where needed during performance, consistent with GS-MID-008.

## 4. Manual mapping
Fields:
- Message type: CC, 14-bit CC/NRPN, or Note where semantically appropriate.
- Channel or channel scope.
- Controller/note number.
- Target parameter/action.
- Scaling/curve and soft-takeover option where supported.

Do not model every structural action as a continuous parameter. Transport, Panic, Pattern recall and step editing may require action/event bindings; exact event model remains an engineering decision.

## 5. Scope and precedence
Two scopes from LIVE_PERFORMANCE_SPECIFICATION.md:
- **Global:** transport, panic, macro-bank select and preset/patch navigation.
- **Preset-specific:** macro destinations, per-layer performance controls and chord/hold behavior.

The source document proposes preset-specific mapping overrides global mapping for the same physical source while that preset is active; otherwise global applies. Treat this as a reviewable design direction, not a newly approved architecture.

Mapping scope is distinct from active Layer focus. A mapping explicitly targeting Layer 3 must not be redirected to the selected layer.

## 6. Conflict handling
When a new mapping collides with an existing mapping in the same scope:
- Show existing and proposed mappings together.
- Offer Replace, Keep Existing or Cancel.
- Never silently overwrite.
- Cross-scope collisions follow the scope precedence rule.
- Provide explicit Unmap and Reset Scope actions.
- A stale target must become visibly invalid/unmapped, never reassigned to an unrelated target.

## 7. Soft takeover
When enabled, a physical control does not change the target until its value crosses/picks up the current software value. UI indicates waiting-for-pickup and clears it after takeover. Exact pickup algorithm/tolerance is a technical contract and remains open.

## 8. Performance-time usability
Existing mappings, required mapping-set recall, Learn entry/exit when needed, sequencer editing, Fill, Mute, Freeze, Panic and other essential operations must be controller-reachable when needed during performance. An operation is not setup-only merely because it can be prepared in advance; apply LIVE_PERFORMANCE_SPECIFICATION.md §8.

## 9. Feedback and diagnostics
R1 has no MIDI feedback requirement. The touchscreen may show inbound activity, last received message and mapping status, but must not imply LED-ring/fader/display feedback is included.

Show connected/disconnected device status, last received message, active/invalid mapping counts, conflicts and soft-takeover waiting state.

## 10. Open decisions and technical gates
- Mapping wire format and maximum simultaneous mapping count.
- Action/event mapping model for non-parameter actions.
- Exact Learn shortcut and controller-only navigation model.
- Soft-takeover pickup algorithm and tolerance.
- Mapping-set recall and persistence model.
- Device hot-plug/reconnect and duplicate-device identity.
- Acceptance tests belong to Phase 5.

No framework, parameter-ID scheme, DSP behavior or implementation API is approved here.

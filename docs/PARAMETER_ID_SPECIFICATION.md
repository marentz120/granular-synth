# Parameter ID and Host Exposure Specification

**Version:** 0.1 — Phase 3 proposal  
**Status:** Proposed; requires owner approval before implementation or publication of host automation IDs.  
**Authority:** GS-PLT-004, GS-MID-003, DEC-021, DEC-031, DEC-036; Q-T12.  
**This document does not authorize implementation.**

## 1. Goals

- Stable automation identity across Standalone, AU and VST3.
- One canonical parameter definition shared by UI, DSP, presets, modulation, MIDI mapping and host adapters.
- No parameter identity derived from UI order, runtime object address, localized name or registration order.
- Safe evolution: parameter IDs already shipped in a public build must never be reused or silently retargeted.

## 2. Proposed identity model

Each parameter has two identities:

1. **Canonical string ID** — permanent semantic identity used by the engine, state, tests, logs and cross-format mapping.
2. **Host numeric ID** — stable 32-bit unsigned integer used by plugin formats/host APIs where required.

Proposed canonical grammar:

`gs.<scope>.<index>.<domain>.<parameter>`

Examples:
- `gs.global.0.transport.tempo`
- `gs.layer.1.grain.size`
- `gs.layer.4.filter.lpf.cutoff`
- `gs.layer.2.mod.lfo.1.rate`
- `gs.performance.0.fill.amount`

Indices are 1-based for layers and LFOs, and 0 for singleton/global scopes. IDs are ASCII lowercase, dot-separated, invariant and not localized. Renaming a display label does not change the ID. A semantic change that makes an old automation lane mean something different requires a new ID.

## 3. Numeric host ID allocation — proposal

Do not compute host IDs from a runtime hash. Hash collisions, implementation differences and accidental changes to the hash algorithm create unacceptable long-term risk.

Use a checked-in, generated registry manifest with explicit numeric assignments:
- Global parameters: reserved range `0x00000001–0x0000FFFF`.
- Layer parameters: reserved range `0x00010000–0x0004FFFF`, with one stable subrange per layer.
- Performance/global live controls: reserved range `0x00050000–0x0005FFFF`.
- Future expansion: `0x00060000–0x7FFFFFFF`.
- `0x80000000–0xFFFFFFFF` reserved for internal/test use; never expose to hosts.

These ranges are a proposal and must be checked against JUCE and AU/VST3 adapter constraints before approval. IDs are allocated once, recorded in source control, never reused, and tombstoned when a parameter is removed. New IDs are appended to the manifest; insertion or sorting must not renumber existing entries.

## 4. Parameter descriptor schema

Each registry entry must include:

| Field | Meaning |
|---|---|
| `stringId` | Canonical permanent identity |
| `hostId` | Stable numeric ID, or null if not host exposed |
| `name` / `category` | Display metadata; localizable separately |
| `scope` | Global, layer, performance, sequencer or other defined scope |
| `type` | Continuous, boolean, enum or trigger/action |
| `unit` | dB, Hz, ms, semitones, %, beats, enum label, etc. |
| `plainRange` | Minimum/maximum or explicit enum values |
| `normalizedMapping` | Bidirectional normalized↔plain conversion |
| `defaultValue` | Factory default in plain units |
| `responseCurve` | Linear, log, exponential, custom table or explicit conversion |
| `smoothingClass` | Audio, Control, Spawn, Stepped or Structural |
| `automatable` | Whether host automation is supported |
| `modulatable` | Whether modulation matrix may target it |
| `midiMappable` | Whether MIDI Learn/manual mapping may target it |
| `performanceCritical` | Candidate flag for controller/live mapping workflows |
| `stateVersion` | Version of the descriptor semantics |
| `deprecated` | Retired/tombstoned without reuse |

A descriptor must define finite behavior for NaN, infinities, out-of-range values, denormals and invalid enum values.

## 5. Host exposure proposal

Q-T12 proposes exposing all automatable R1 parameters, grouped by global/layer/performance scope, with performance macros and high-value performance controls easy to find.

Proposed rules:
- Expose every parameter marked `automatable=true`; do not expose purely structural operations as continuous host parameters.
- A sample/file selection is structural and should use state/UI workflows unless a host-safe discrete selector is explicitly justified.
- A parameter's host-visible status must be explicit; it must not be inferred from the existence of a UI control.
- Expose the ten R1 macros per patch (DEC-036) and approved performance controls.
- Preserve the same semantic parameter value and range in Standalone, AU and VST3.
- Use host parameter gestures/begin-change/end-change semantics correctly; UI automation must not generate feedback loops.
- Define behavior when a host supplies a value while a structural patch change is being prepared.

The exact total parameter count must be generated from the R1 registry; the earlier estimate of 1,000–1,400 is not an authoritative count.

## 6. State and migration rules

- Preset/patch state stores canonical string IDs and plain values, or a versioned compact representation with a lossless mapping to those IDs.
- Host automation state maps through the stable numeric host ID manifest.
- Unknown IDs in a newer preset are ignored with a user-visible compatibility warning; known parameters retain values.
- Missing values use the default for the preset's declared schema version, then migrate explicitly.
- Removed parameters are tombstoned; their host IDs are never reassigned.
- Renames are display metadata changes only. Semantic changes require a new ID and a migration rule.
- A registry validation test must fail on duplicate string IDs, duplicate numeric IDs, unassigned exposed parameters, invalid ranges or missing conversion functions.

## 7. Modulation and MIDI mapping

- Modulation and MIDI mappings reference canonical string IDs, not host numeric IDs.
- A parameter may be host-automatable without being modulatable, or vice versa, subject to its descriptor flags.
- Structural parameters are not targets of continuous modulation.
- DEC-031 applies: macro base values are shared per layer patch; per-note sources may create per-voice destination values.
- DEC-036 applies: ten macros per patch in R1.
- MIDI mapping scopes and conflict rules remain governed by `LIVE_PERFORMANCE_SPECIFICATION.md` and `MIDI_SPECIFICATION.md`.

## 8. Required tests before approval

- Manifest uniqueness and reserved-range validation.
- Golden mapping tests: string ID ↔ numeric ID ↔ descriptor.
- Stability test comparing a prior released manifest to the current manifest; no existing numeric ID changes or reuse.
- Normalized/plain round-trip and boundary tests for every parameter type.
- Host automation write/read, gestures, save/reopen and offline-render tests for AU and VST3.
- Preset migration tests across schema versions.
- Host automation and MIDI mapping to the same parameter without race or feedback loops.
- Modulation and automation precedence tests.
- Unknown/deprecated parameter recovery tests.

## 9. Decisions required

| Question | Proposal | Approval needed |
|---|---|---|
| Canonical string ID grammar | `gs.<scope>.<index>.<domain>.<parameter>` | Owner |
| Numeric host ID model | Explicit checked-in manifest; append-only/tombstoned | Owner |
| Exposure policy | All explicitly automatable R1 parameters; structural actions excluded | Owner |
| Parameter metadata contract | Schema in §4 | Owner |
| Macro/performance priority | Expose ten macros and performance controls in discoverable groups | Owner |

**Status:** This is a proposal for Q-T12. Do not publish these IDs in a plugin build until the owner approves the scheme and the complete R1 registry has been generated and reviewed.

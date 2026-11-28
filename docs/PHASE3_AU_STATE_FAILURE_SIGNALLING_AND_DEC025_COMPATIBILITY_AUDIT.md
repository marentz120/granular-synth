# Phase 3 AU State Failure Signalling & DEC-025 Compatibility Audit

**Status:** Research and architecture evidence; proposal only  
**Checked:** 2026-11-28  
**Scope:** AUv3 property surface, AUv2 bridged property surface, document-save failure semantics, restore fallback, DEC-025 payload integrity, and AU state-object ownership.

## 1. Executive finding

The earlier shorthand that “AU has no explicit getter error channel” is too broad.

There are two API surfaces:

1. **AUv3 (AUAudioUnit)** exposes nullable dictionary properties:
   - **fullState: [String: Any]?**
   - **fullStateForDocument: [String: Any]?**

   Apple documents both as bridged to their AUv2 property equivalents. The Swift/Objective-C property signatures do not expose a throws, NSError-out parameter, or separate result value. The public documentation does not specify how a nil AUv3 getter result is translated by the bridge into an AUv2 status, or what a host must do with nil during document save.

2. **AUv2 (AudioUnitGetProperty)** returns an **OSStatus**. The ClassInfo and ClassInfoFromDocument properties are read/write CFDictionary state properties. This gives the AUv2 property call an explicit API-level result code. It does **not** prove how a particular host handles a failed getter during project save.

The most consequential documented behavior is on **document restore**: when restoring a document, a host should try **kAudioUnitProperty_ClassInfoFromDocument** first; if the Audio Unit returns an error or does not implement that property, the host should use the same preset with **kAudioUnitProperty_ClassInfo**. This is a restore fallback rule, not a documented guarantee that a failed save is rolled back.

Therefore, AU failure signalling is not a single yes/no capability. It is format-surface-specific, and save behavior must be distinguished from restore fallback.

## 2. Evidence and exact API boundaries

### 2.1 AUv3 dictionary properties

Apple documents **fullState** as an optional dictionary suitable for a user preset and says hosts may use it to save/restore an Audio Unit in a preset or document. It is bridged to **kAudioUnitProperty_ClassInfo**.

Apple documents **fullStateForDocument** as an optional dictionary suitable for a user document. It is bridged to **kAudioUnitProperty_ClassInfoFromDocument**. A subclass that does not implement this property interfaces with **fullState**.

The documented signatures are nullable dictionary getters/setters. They do not expose an error result alongside the getter. The public documentation reviewed here does not define:
- whether nil means “not implemented”, “no state”, or “serialization failure” in every host path;
- the exact nil-to-AUv2-OSStatus bridge mapping;
- whether a host aborts, retries, falls back, or saves without the required state after a nil result;
- a host-visible transaction or rollback guarantee for document save.

Do not infer these semantics from the optional type alone.

### 2.2 AUv2 property calls

Apple documents **AudioUnitGetProperty** as returning an **OSStatus** result code. **kAudioUnitProperty_ClassInfo** and **kAudioUnitProperty_ClassInfoFromDocument** are read/write CFDictionary properties on the global scope.

For **kAudioUnitProperty_ClassInfoFromDocument**, Apple explicitly documents the **restore** selection rule: try the document-specific property first; if it returns an error or is not implemented, use the same preset with **kAudioUnitProperty_ClassInfo**.

The documentation does not establish that hosts apply that same fallback during **save**, nor that a save-time property error aborts the DAW project write. These are separate host observations.

### 2.3 Object ownership and memory accounting

Apple's **AudioUnitGetProperty** documentation says that retrieving a Core Foundation object duplicates the object and that the caller is responsible for releasing it. Treat the returned CF object as an owned reference.

“Duplicates the object” must not be interpreted as proof of a deep copy of every nested immutable value or audio payload. The memory ledger must distinguish:
- the owned CF reference/object lifetime;
- the underlying dictionary and Data backing allocations actually created;
- any additional host or plug-in materialization/copy proven by observation.

Do not multiply payload bytes by a guessed copy factor. Do not omit an observed owned materialization from aggregate peak-memory admission.

## 3. Save semantics versus restore semantics

| Surface / operation | Documented result surface | Documented fallback | What remains unproven |
|---|---|---|---|
| AUv3 fullStateForDocument getter during save | Optional dictionary; no separate error parameter | No save fallback guarantee established | Meaning of nil, bridge mapping, host save outcome |
| AUv2 ClassInfoFromDocument getter during save | AudioUnitGetProperty returns OSStatus | No save fallback guarantee established | Whether each host aborts, retries, or writes incomplete state |
| AUv3 document restore | Optional dictionary bridged to AUv2 property | AUv2 restore fallback may apply through the bridge; exact nil mapping is unspecified | Host's actual path and whether fallback occurs |
| AUv2 document restore | OSStatus from property call | Apple says host should use ClassInfo if document property errors or is not implemented | Host behavior for malformed/partial dictionaries and payload loss |
| VST3 component state | Explicit tresult | No universal host rollback guarantee | Host-visible handling of failed/short state transfer |

The restore fallback matters to GRANULAR SYNTH because **fullState** and **fullStateForDocument** have distinct intended scopes. A preset-safe state may not contain the unsaved recording and Freeze payload required by DEC-025/DEC-042 document persistence. A host restore fallback from document state to preset state must not be described as a successful complete document restore unless the required document payload is demonstrably present.

## 4. DEC-025 compatibility contract

The following constraints remain mandatory:

1. A successful DAW document-state result must contain every required unsaved recording payload and every required persisted Freeze payload in one document-state transaction. Each Freeze payload retains its already-approved immutable Source Snapshot generation/range; it is not recaptured from the live recording ring.
2. A stale descriptor, missing chunk, failed integrity check, or incomplete required payload must never be represented as a valid complete document state.
3. The AU adapter must not silently substitute a preset-scoped **fullState** dictionary for required document state.
4. The adapter must not assume that returning nil from an AUv3 getter cancels the host's save.
5. The adapter must not assume that an AUv2 non-success **OSStatus** cancels save, or that the restore fallback rule also applies to save.
6. If a host's actual save path cannot preserve the required DEC-025 payload and exposes no reliable failure path, record a host compatibility blocker. Do not weaken DEC-025 by omitting the audio or by returning a stale snapshot.
7. If a host later restores through the ClassInfo fallback, validate the restored container kind and required-chunk manifest. A parameter-only preset is not a complete document restore.

This is a technical compatibility contract, not a new product decision. It does not decide whether reusable user presets should contain recordings or Freeze payloads; that Q-T15 scope question remains open.

## 5. Candidate AU adapter behavior (not a frozen ABI)

- Implement and test the document-specific state path separately from the user-preset path; do not rely accidentally on the base-class fallback from **fullStateForDocument** to **fullState**.
- Build the complete document dictionary from a prevalidated immutable snapshot. Its manifest must identify the state/container kind and enumerate every required audio chunk.
- Do not construct a syntactically valid but incomplete document dictionary when capture freshness, retention, admission, or chunk integrity fails.
- For AUv2, propagate the actual non-success result from the property getter when the implementation cannot provide a valid value. No specific OSStatus constant is selected in this audit.
- For AUv3, treat nil as an API-surface limitation until the bridge and each host's behavior are observed. Do not invent an NSError channel that the property signature does not provide.
- Record internal diagnostic cause and snapshot generation outside the persisted payload; diagnostics do not replace host-visible failure semantics.
- During restore, distinguish document-container validation from preset-container validation. Do not report a ClassInfo fallback as satisfying DEC-025 unless all required document payloads are present.

The exact callback implementation, bridge behavior, status mapping, and host-visible failure policy remain Q-T15/Q-P09 evidence gates.

## 6. Host evidence matrix

| Host / format | API evidence | Save failure behavior | Restore fallback behavior | Status |
|---|---|---|---|---|
| Logic Pro / AU | AUv3/AUv2 API applies; exact component path must be pinned | Not established by reviewed host guide | Not established by reviewed host guide beyond Apple's API rule | Prototype required |
| Ableton Live / AU | AU state is stored as opaque plug-in state in the Live Set | Binary state storage is documented; nil/OSStatus failure outcome is not | Not established | Prototype required |
| REAPER / AU | AU support and project/FX persistence are documented at product level | Not established | Not established | Prototype required |
| Ableton Live / VST3 | VST3 result/stream surface applies | Host rollback/failure remains unproven | VST3-specific restore behavior requires evidence | Separate VST3 gate |

No host has been tested or certified by this audit. Do not generalize one host's result to the others.

## 7. Phase 5 fixture families (planned, not executed)

| Fixture | Stimulus | Required evidence/assertion |
|---|---|---|
| AUv2 ClassInfo success | Query a valid complete document state | OSStatus, returned dictionary validity, exact required-chunk manifest |
| AUv2 ClassInfoFromDocument failure | Inject a non-success property getter result | Record host save outcome separately from restore fallback |
| AUv2 restore fallback | Return an error for ClassInfoFromDocument, provide a preset ClassInfo | Observe whether host attempts fallback; verify a preset-only state is not counted as complete DEC-025 restore |
| AUv3 valid document state | Return a complete fullStateForDocument dictionary | Bridge path, payload hash and host save/reopen round-trip |
| AUv3 nil document state | Return nil only in an authorized host prototype | Observe bridge status, host save result, fallback/retry behavior; no assumed cancellation |
| AUv3 missing override | Use the documented base fallback path in a controlled fixture | Determine whether fullState is queried and whether required document chunks survive |
| Save versus restore separation | Inject failure independently during getter/save and setter/restore | Record separate host-visible outcomes; do not infer one from the other |
| Preset/document scope mismatch | Document payload exists only in document state | Detect any restore that silently falls back to parameter-only preset state |
| CF object ownership | Repeatedly query/release AUv2 state objects | No leaked owned CF references; account for actual Data backing without assuming deep copies |
| Materialization admission | Use deterministic payload sizes in an authorized prototype | Record owned allocations/copies and peak memory; no numeric budget is inferred here |

Execution requires pinned host, OS, AU format/version and plug-in builds plus explicit host-prototype authorization. This audit does not authorize a prototype or benchmark.

## 8. Threading and memory consequences

- The AU property documentation does not establish a universal callback thread or render-quiescence guarantee. The getter must not assume the render thread has stopped.
- Snapshot capture and validation remain bounded and generation-consistent. No disk I/O, blocking worker wait, or unbounded serialization is permitted on a render path.
- The AUv2 returned CF object has an owned lifetime and must be released by its owner. The release and destruction path must not be moved onto the render thread if doing so can trigger non-bounded work.
- AUv3 dictionary/Data materialization must be measured in a future authorized prototype. The current API documentation does not prove whether nested payloads are copied, retained, or serialized eagerly by a host.
- Admission counts actual plug-in-owned dictionary/Data/staging allocations and observed copies. Unknown host-internal copies remain an evidence limitation, not a guessed multiplier.
- Save, restore, recording retention and Freeze continue to use the shared aggregate physical-memory and retention-capacity ledger.

## 9. Open gates and product consistency

| Gate | Status |
|---|---|
| DEC-025 | Unchanged: required unsaved recording payloads remain embedded in DAW document state; no silent omission |
| DEC-042 | Unchanged: Source Snapshot Freeze remains approved; mixed-layer preparation-failure policy remains owner-gated |
| Q-T15 | Open: AUv3 nil mapping, AUv2 save-error handling, container, payload limits, migration and user-preset scope |
| Q-P09 | Open: pinned Logic/Live/REAPER versions and observed save/restore behavior |
| AR-06 | Open owner decision; this audit does not select old-voice treatment |
| 124 R1 requirements | Unchanged |
| Acceptance/release-gate links | 53/124 unchanged; candidate symbols/fixtures do not count as coverage |

No product behavior, requirement tier, AC ID, public ABI, numeric limit, host certification or owner decision is introduced.

## 10. Primary Apple references

- [AUAudioUnit.fullState](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstate)
- [AUAudioUnit.fullStateForDocument](https://developer.apple.com/documentation/audiotoolbox/auaudiounit/fullstatefordocument)
- [kAudioUnitProperty_ClassInfo](https://developer.apple.com/documentation/audiotoolbox/kaudiounitproperty_classinfo)
- [kAudioUnitProperty_ClassInfoFromDocument](https://developer.apple.com/documentation/audiotoolbox/kaudiounitproperty_classinfofromdocument)
- [AudioUnitGetProperty](https://developer.apple.com/documentation/audiotoolbox/audiounitgetproperty(_:_:_:_:_:_:))
- [Technical Note TN2157 — saving and restoring Audio Unit presets](https://developer.apple.com/library/archive/technotes/tn2157/_index.html)

**Completion boundary:** architecture/research/test design only. No DSP, Plugin Shell, host prototype, benchmark, Phase 5 execution or PR merge is authorized.

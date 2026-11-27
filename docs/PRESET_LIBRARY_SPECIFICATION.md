# Presets, Projects and Library Specification

**Version:** 0.1 — Phase 2 working specification  
**Status:** Product policies Q-P07/Q-P08 approved by DEC-022/025; technical serialization and implementation remain Phase 3. No implementation authorized.  
**Authority:** GRANULAR_SYNTH_REQUIREMENTS.md GS-PRS-001…010; R1_PRODUCT_SPECIFICATION.md §§4.4–4.5; DEC-015…021.  
**Benchmark evidence:** GR_MEGA_FEATURE_MAP.md §13.

## 1. Purpose

Define the user-visible state hierarchy and persistence behavior for samples, layer patches, four-layer multi states, projects, patterns, performance sets and the searchable library. This document defines product semantics only. File formats, serialization schema, database/index design, embedding strategy and plugin-state byte limits remain Phase 3 decisions.

## 2. Conceptual objects

| Object | Product meaning | Required content |
|---|---|---|
| Sample | Audio asset or recorded buffer | Audio content/reference, channel layout, sample rate, asset identity and applicable edit metadata |
| Template | Reusable parameter set that can be applied without replacing the current sample | Eligible sound parameters; excludes sample audio and sample-specific data unless explicitly selected |
| Patch | One layer's sound state | Engine selection, sample relationship, engine/voice/envelope/filter/modulation/macro/FX state |
| Multi | Four-layer instrument state | Four layer patches, layer mix and layer-level routing/configuration |
| Preset | Named, recallable saved sound state | A Patch or Multi recall target; exact user-facing distinction remains to be finalized |
| Pattern | Central sequencer content | Events, timing, lanes, parameter locks and pattern-local settings |
| Performance Set | Ordered set-list of Presets and Patterns | Ordered entries and recall/transition information |
| Project | User workspace/library container | Organization and references to saved assets, Presets, Patterns and Performance Sets |
| System settings | Application/device preferences | Kept separate from sound patches and project content |

The existing requirements use “Patch”, “Preset”, “Multi” and “Project” in overlapping ways. The product must expose an understandable hierarchy without inheriting the benchmark's fixed slot counts.

## 3. R1 library and browser

- Provide searchable preset browsing with banks, tags, favourites and text search (GS-PRS-002).
- The library is not limited to a fixed number of projects, banks or slots. Performance bank/slot views may be provided as a recall-oriented presentation, not as a storage ceiling.
- Provide a user-selectable default template used to initialize a new patch.
- Provide templates that contain parameter state without sample audio and allow applying a template to the current patch (GS-PRS-003).
- Search/filter operations must not interrupt audio processing.
- Factory content, if included, is write-protected; user content remains editable.
- The conceptual model is settled by DEC-022: the searchable desktop library is the canonical storage/discovery model; performance banks index library objects and Performance Sets preserve ordered references.

## 4. Save and recall semantics

- A Patch save persists the complete state of one layer.
- A Multi save persists the four-layer state and mix.
- A Pattern save is explicit; editing a pattern during playback does not auto-overwrite its saved version.
- A Performance Set preserves ordered references to Presets and Patterns.
- Changing Preset does not replace or stop the active Pattern (R1_PRODUCT_SPECIFICATION.md §4.4).
- Unsaved Pattern edits are preserved as a temporary Draft when leaving the Pattern; only the latest explicitly saved version persists across application restart (R1_PRODUCT_SPECIFICATION.md §4.5).
- Undo/Redo covers editing operations and parameter changes per GS-PRS-007 and the approved sequencer edit model.
- Saving must be atomic from the user's perspective: an interrupted save must not leave a partially written preset that crashes the application. The implementation and recovery strategy are Phase 3.
- Save prompts and dirty-state indicators must distinguish Patch, Multi, Pattern and Project changes; exact UI is Phase 4.

## 5. Templates and copy/paste

- Applying a template copies its included sound parameters to the target Patch.
- Applying a template must not silently replace the target sample asset or sample-specific flags/markers unless the user explicitly chooses an operation that includes those fields.
- Copy/paste supports Patch, Layer, matrix row and FX slot as required by GS-PRS-008.
- Copy/paste preserves references only when the referenced object remains valid; missing references must be surfaced and handled safely.
- A template's exact parameter inclusion/exclusion list is to be published with the final R1 parameter inventory.

## 6. Sample portability and relinking

- Sample references must be portable using relative paths plus content hash and a relink workflow (GS-PRS-005).
- Provide a collect-samples export bundle that includes all external sample dependencies required by the selected project/preset/performance set.
- Relinking must verify asset identity using the stored content hash; a file with a different hash must not be silently substituted.
- Missing assets must not crash the host; the user must be told what is missing (GS-NFR-012).
- DEC-025 settles the product policy: unsaved recording buffers are embedded in DAW plugin state; existing library samples remain external references with relink/collect support. Exact serialization and state-size handling remain Phase 3.
- The product must not assume that an external path remains available after a project is moved to another machine.

## 7. Versioning and migration

- Preset state is versioned and human-readable at the logical-format level (GS-PRS-004).
- Parameter scaling/meaning must be versioned so an existing preset does not change sound merely because a later release changes UI range or internal scaling.
- Future versions must provide forward migration for older supported preset versions.
- Unknown fields may be preserved or safely ignored; unknown parameters must not crash the host.
- Migration must be deterministic and covered by fixtures in the test suite.
- Exact schema and file extension are not decided here.

## 8. Patch metadata

- Favourites, tags, banks and searchable names are R1.
- Comments per patch are present in benchmark mapping (PRS-08) but current GRANULAR SYNTH requirement GS-PRS-009 is proposed R1. Retain the requirement pending owner review; benchmark character limit of 100 is not imposed unless approved.
- One-click sample-name-to-patch-name is benchmark convenience only, not a current R1 requirement.
- Importing GR-1/GR-MEGA patches is optional and blocked pending legal review (Q-L01); no reverse engineering or proprietary format compatibility work is authorized.

## 9. Edge cases

- Missing sample: restore patch metadata, mark sample missing, allow relink, do not crash.
- Hash mismatch: warn and require explicit confirmation before using a different asset.
- Corrupted preset: isolate the invalid object, recover other valid data where possible, and show an actionable error.
- Unknown preset version: reject safely or migrate only when a deterministic migration exists.
- Duplicate names: allow distinct assets with unique internal identities; do not overwrite by display name.
- Interrupted save: retain previous valid saved version.
- Project relocation: relative paths and collect-samples bundle preserve portability.
- Pattern draft: draft is temporary and does not silently become the latest saved pattern.

## 10. Open questions

| ID | Question | Status |
|---|---|---|
| Q-P07 | Exact conceptual and UX distinction among Patch, Preset, Multi, Project and global Library | **ANSWERED — DEC-022**; UI details remain Phase 4 |
| Q-P08 | Embed versus reference recorded buffers and samples in plugin state | **Product policy ANSWERED — DEC-025**; serialization and size policy Phase 3 |
| GS-PRS-006 | Plugin state includes recorded unsaved buffers | R1 product policy: embed unsaved buffers (DEC-025); exact mechanism Phase 3 |
| GS-PRS-009 | Patch comments | R1 requirement; no benchmark-specific 100-character limit imposed |
| GS-PRS-010 / Q-L01 | GR-1/GR-MEGA import | Optional and legally blocked; excluded from R1 |
| Q-T12 | Host parameter IDs/exposure | Phase 3 |

## 11. Acceptance intent

Tests must cover save/recall of Patch and Multi state, template application without sample replacement, independent Pattern persistence, Preset changes while Pattern continues, temporary Draft restoration during the session, Undo/Redo, search/tags/favourites, portable sample bundles, relinking/hash mismatch, missing/corrupt assets, version migration and plugin state restoration. Exact UI tests belong to Phase 4/5.

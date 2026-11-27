# Phase 3 R1 API Symbol Traceability

**Version:** 0.3 — render-capacity and logical input routing symbols added, 2026-11-27  
**Status:** Proposed class/API-symbol traceability; no implementation, ABI or public API approval.  
**Baseline:** docs/GRANULAR_SYNTH_REQUIREMENTS.md R1-tier rows, DEC-041, docs/R1_ACCEPTANCE_CRITERIA.md, and candidate symbols in docs/PHASE3_API_AND_SIGNAL_FLOW_SPECIFICATION.md §14.  
**Branch baseline:** Phase 3 architecture branch; this document is documentation-only.

## 1. Purpose and interpretation

This matrix advances the existing 124/124 requirement-to-module/interface mapping by assigning each R1 requirement to one or more candidate API symbols and operations. It is a design traceability artifact, not a source-code inventory: none of these candidate symbols is implemented, ABI-stable or approved as a final C++ signature. Symbols may be represented by a class, value type, free function or internal component after the framework and implementation boundaries are approved.

The mapping preserves the current 12 IF contracts. The IF column is copied from the existing architecture traceability matrix; the candidate API column is the new refinement. Requirement text and tier authority remain exclusively in GRANULAR_SYNTH_REQUIREMENTS.md and the approved scope/decision documents. L2/L3/OPT-only rows are not promoted.

## 2. Requirement → candidate API symbol → acceptance trace

| Requirement | Candidate API symbol / operation | Existing IF contracts | Current acceptance link | Open boundary / decision note |
|---|---|---|---|---|
| GS-PLT-001 | IAUProcessAdapter::normalizeProcessContext; IVST3ProcessAdapter::normalizeProcessContext; IStandaloneDeviceAdapter::processDeviceBlock; IAudioEngine::prepare/process; RenderCapacity::maxFramesPerBlock/lifecycleGeneration | IF-01, IF-11, IF-12 | AC-AUD-001 | — |
| GS-PLT-002 | ReleasePackagingManifest::architectures; BuildMatrix::packageUniversal2 | IF-01, IF-11, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | Q-T03 פתוח: Universal 2 וגרסת macOS מינימלית אינן מאושרות. |
| GS-PLT-003 | IAudioEngine::process; ProcessBlock::frames; RenderCapacity::maxFramesPerBlock; BlockContext::sampleRate | IF-01, IF-11, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | טווח 44.1–192 kHz ובלוקים 16–4096 כפוף ל-Q-T17; אין כאן התחייבות תאימות שנבדקה. |
| GS-PLT-004 | IOfflineRenderer::render; IAudioEngine::process; IRandomStream::seed | IF-01, IF-11, IF-12 | §7 — shared-core/Standalone↔plugin behavior consistency | נדרשת הבהרה של bit-identical בין arm64/x86_64; מדיניות FP עדיין לא הוגדרה. |
| GS-ARC-001 | LayerRuntime::ownedPatch/voices/modulation/fx/channel; ILayerProcessor::processVoices/processInput/processEffects/processOutput | IF-01, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-ARC-002 | IVoiceAllocator::allocate; VoicePoolConfig::polyphonyLimit | IF-01, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-ARC-003 | IGrainPool::tryAcquire/release; GrainPoolConfig::perVoiceLimit | IF-01, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-ARC-004 | IGrainBudgetPolicy::admit/selectReclamationCandidates; IGrainPool::tryAcquire | IF-01, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | AR-02 פתוח; תקציב הגרגרים, מדיניות reclaim וספי degradation לא נבחרו ולא נמדדו. |
| GS-ARC-005 | ILayerEngineSelector::prepareEngine/publishSelection; LayerEngineKind | IF-01, IF-03, IF-09, IF-10, IF-12 | AC-SMP-001 | — |
| GS-ARC-006 | IAUProcessAdapter::normalizeProcessContext; IVST3ProcessAdapter::normalizeProcessContext; IStandaloneDeviceAdapter::mapOutputChannels; ProcessBlock::mainOutput/layerOutput/inputBuses/layerInput; IMasterMixer::mixLayers | IF-01, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | מיפוי bus הוא חוזה אדפטר; פריסת buses סופית לכל Host עדיין דורשת אימות. |
| GS-ARC-007 | ILayerProcessor::processVoices/processInput/processEffects/processOutput; ProcessBlock::inputBuses/layerInput; IMasterMixer::mixLayers | IF-01, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-ARC-008 | IVoiceGainPolicy::gainForVoice; IGrainNormalizer::gainForCount; IMasterMixer::mixLayers | IF-01, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-001 | IGrainScheduler::schedule; GrainRate::grainsPerSecond | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-002 | IGrainScheduler::schedule; GrainParameters::sizeMilliseconds | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-003 | IGrainParameterModel::density; IUiSnapshotSource::tryReadLatest | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-004 | IGrainParameterModel::position; GrainPosition::normalizedDouble | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-005 | IGrainRandomizer::sampleSprayOffset | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-006 | IGrainSpatializer::distributePan | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-007 | IGrainSpatializer::applyMidSideWidth | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-008 | IGrainDirectionSelector::chooseDirection | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-009 | IGrainScanController::sourcePosition | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-010 | IGrainPitchMapper::resolvePitch; IScaleQuantizer::quantizeNote | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-011 | IGrainRatioController::resolvePlaybackRatio | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-012 | IGrainWindowBank::lookup; GrainWindow::evaluate | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-013 | IGrainWindowTransition::processActiveGrains | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-014 | IGrainScheduler::onNote/onTransportTick; ITransport::snapshotForBlock | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-015 | IGrainSpawnPolicy::emit | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-016 | IGrainPitchState::captureBirthPitch | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-017 | IGrainBudgetPolicy::selectReclamationCandidates; IGrainPool::release | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | AR-02 פתוח; סף stealing ומשך fade נשארים לא מאושרים. |
| GS-GRN-018 | IGrainNormalizer::gainForCount | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-GRN-019 | IGrainScheduler::scheduleAtSampleOffset; IAudioEngine::process | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | דרישת sample-offset נשמרת; AR-01 עדיין פתוח לגבי tie-break ו-writer precedence. |
| GS-GRN-020 | IGrainInterpolator::sample | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-CHD-001 | IChordProcessor::expand; ChordSlotSet::validate | IF-08, IF-09, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-CHD-002 | IChordProcessor::distributeAcrossGrains/stackVoices | IF-08, IF-09, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-CHD-003 | IGranularArpeggiator::eventsForBlock | IF-08, IF-09, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-CHD-004 | IChordRecorder::capturePlayedNotes | IF-08, IF-09, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-CHD-005 | IScaleQuantizer::quantizeNote; ISubOscillator::quantizedPitch | IF-08, IF-09, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMP-001 | ISamplerVoice::start/process; IPlaybackHead::advance | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | AC-SMP-002, AC-SMP-004 | — |
| GS-SMP-002 | ISamplerLoopController::resolveBoundary/onVoiceRelease | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | AC-SMP-003, AC-SMP-007 | — |
| GS-SMP-003 | ISamplerInterpolator::sample | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | AC-SMP-005 | — |
| GS-SMP-004 | ISamplerVoice::retrigger; SamplerLayerConfig::envPileUpEnabled | IF-02, IF-03, IF-05, IF-09, IF-10, IF-12 | AC-SMP-006 | — |
| GS-VOI-001 | IVoiceAllocator::chooseVictim/steal | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-002 | IVoice::setSustain; IMidiChannelModeHandler::applyCC64 | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-003 | IVoicePitchController::glideTo | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-004 | IVoiceVelocityMapper::mapVelocity | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-005 | IVoicePitchController::applyPitchBend/transpose | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-006 | IEnvelopeBank::processStage | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-008 | IVoiceFilterChain::processLpfThenHpf | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-009 | IFilterKeyTracker::cutoffForNote | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-VOI-010 | ISubOscillator::process | IF-02, IF-03, IF-09, IF-10, IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMG-001 | ISampleImportService::decode/prepare; ISampleStore::prepareAsset; IResampler::prepare | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-012 | — |
| GS-SMG-002 | ISampleBrowser::search/audition; ISampleDropAdapter::acceptFileDrop | IF-01, IF-05, IF-06, IF-07, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMG-003 | ISampleImportService::prepare; IAudioCommandSink::tryPublish | IF-01, IF-05, IF-06, IF-07, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMG-004 | ISampleNormalizer::normalizeOnImport | IF-01, IF-05, IF-06, IF-07, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMG-005 | IWaveformViewModel::snapshot; IUiSnapshotSource::tryReadLatest | IF-01, IF-05, IF-06, IF-07, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMG-006 | ISampleRegionController::setFlags; IScanController::positionForBlock | IF-01, IF-05, IF-06, IF-07, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMG-007 | ISampleEditService::crop/softCut/normalize; IUndoHistory::apply | IF-01, IF-05, IF-06, IF-07, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SMG-008 | IRecordingController::start/processCapture/stop; IRecorder::processCapture | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-008 | — |
| GS-SMG-009 | ILayerProcessor::processInput; ISampleAsset::channelData | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-009 | — |
| GS-SMG-010 | IRecordingPlayhead::position; IRecorder::processCapture | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-008 | — |
| GS-SMG-011 | IRecordingBufferPolicy::reserve/initialize; IMemoryAdmission::reserve | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-008 | — |
| GS-SMG-012 | IAUProcessAdapter::normalizeProcessContext; IVST3ProcessAdapter::normalizeProcessContext; ProcessBlock::inputBuses/layerInput; ILayerProcessor::processInput | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-003, AC-AUD-004 | — |
| GS-SMG-017 | ISampleExportService::exportWav | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-010 | — |
| GS-SMG-019 | IMemoryAdmission::reserve; ISampleStore::prepareAsset | IF-01, IF-05, IF-06, IF-07, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | DEC-028: יעד שש דקות סטריאו ל-layer כפוף להיתכנות בטוחה; לא בוצע Benchmark. |
| GS-SMG-020 | IRecorder::processCapture; RecordTap; IMasterMixer::mixLayers | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-006, AC-AUD-007 | — |
| GS-SMG-021 | IFreezeController::requestLayerFreeze/requestGlobalFreeze/requestThaw | IF-01, IF-05, IF-06, IF-07, IF-11 | AC-AUD-005 | DEC-042 מאושרת; מדיניות Global Freeze במקרה של הכנה חלקית עדיין דורשת החלטת בעלים. |
| GS-MOD-001 | ILfo::advance; LfoConfig::rate/waveform/phase | IF-02, IF-03, IF-04, IF-09 | AC-MOD-001, AC-MOD-002 | — |
| GS-MOD-002 | ILfo::phaseMode/advance; LfoConfig::defaultPhaseMode | IF-02, IF-03, IF-04, IF-09 | AC-MOD-002 | DEC-024 מאושרת: Mono כברירת מחדל ו-Poly אופציונלי לכל LFO. |
| GS-MOD-003 | IModulationMatrix::addRow/evaluate | IF-02, IF-03, IF-04, IF-09 | AC-MOD-003 | — |
| GS-MOD-004 | IModulationSourceRegistry::read; IRandomStream::nextForNote | IF-02, IF-03, IF-04, IF-09 | AC-MOD-004, AC-MOD-008, AC-MOD-009, AC-MOD-010 | — |
| GS-MOD-005 | IModulationMatrix::evaluate; IParameterRegistry::find | IF-02, IF-03, IF-04, IF-09 | AC-MOD-004 | — |
| GS-MOD-006 | IModulationAssignmentService::assign; ILfo::bindDestination | IF-02, IF-03, IF-04, IF-09 | AC-MOD-005 | — |
| GS-MOD-007 | IModulationMatrix::copyRow/pasteRow/deleteRow | IF-02, IF-03, IF-04, IF-09 | AC-MOD-003 | — |
| GS-MOD-008 | IMacroBank::value/resolveDestinations; IParameterRegistry::find | IF-02, IF-03, IF-04, IF-09 | AC-MOD-006 | DEC-036 מאושרת: עשרה macros לכל Patch. |
| GS-MOD-009 | IModulationMonitor::snapshot; IUiSnapshotSource::tryReadLatest | IF-02, IF-03, IF-04, IF-09 | AC-MOD-007 | — |
| GS-SEQ-001 | ISequencer::eventsForBlock; ISequencerPattern::eventAtStep | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SEQ-002 | ISequencerModulationLane::evaluate; IParameterWriterPolicy::resolve | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | AR-01 פתוח; precedence בין sequencer locks, host automation, MIDI ו-UI אינו מוכרע. |
| GS-SEQ-003 | ISequencerTimingGrid::quantize; ISequencer::eventsForBlock | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SEQ-004 | IPatternStore::requestSwitchAtBarBoundary; IPerformanceSet::nextPattern | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SEQ-005 | ITransport::snapshotForBlock/setAuthority; ITransport::acceptMidiClock | IF-03, IF-08, IF-09 | AC-MID-007 | DEC-019 קובע סנכרון מלא; יש לשמור מקור clock יחיד ולמנוע כפילות בין Host ל-MIDI Clock. |
| GS-SEQ-008 | ILayerDroneController::setLatchedNote; IVoiceAllocator::panic | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SEQ-009 | ILayerPerformanceController::setMix/setPosition; IMasterMixer::mixLayers | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SEQ-011 | ISequencerFillController::setIntensity/trigger; ISequencerMuteController::setLayerMute/setStepMute | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-SEQ-012 | ISequencerEditService::beginDraft/commitAtBarBoundary/undo/redo | IF-03, IF-08, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-MID-001 | IMidiEventRouter::routeHostEvents/routeDeviceEvent; ILayerMidiChannelMap::matches | IF-01, IF-08, IF-09 | AC-MID-001 | — |
| GS-MID-002 | IMpeDecoder::decode; IVoice::applyPerNotePitch/pressure/timbre | IF-01, IF-08, IF-09 | AC-MID-002 | DEC-038 מאושרת: pitch bend per-note בטווח 48 semitones. |
| GS-MID-003 | IMidiLearnService::bind; IControllerMap::apply; IParameterRegistry::find | IF-01, IF-08, IF-09 | AC-MID-003, AC-MID-004 | — |
| GS-MID-005 | IProgramChangeRouter::recallMappedProgram; IPerformanceBank::lookup | IF-01, IF-08, IF-09 | AC-MID-005 | DEC-033 מאושרת: Program Change רק למיפוי מפורש ב-active bank. |
| GS-MID-006 | IMidiChannelModeHandler::apply; IVoiceAllocator::allNotesOff/allSoundOff; IParameterRuntime::resetControllers | IF-01, IF-08, IF-09 | AC-MID-006 | — |
| GS-MID-007 | ITransport::acceptMidiClock/snapshotForBlock | IF-01, IF-08, IF-09 | AC-MID-007 | DEC-039 מאושרת: Start reset, Continue resume, Stop halt; אובדן pulses אחרי שני beat intervals עוצר progression; SPP ignored. |
| GS-MID-008 | IControllerMap::dispatchEssentialOperation; IFreezeController; ISequencerEditService | IF-01, IF-08, IF-09 | AC-MID-008 | DEC-016/021: controller-only עבור essential live operations; ללא MIDI feedback או named-controller templates. |
| GS-FX-001 | IEffectChain::reorder/process; IEffectSlot::setMix/setSends | IF-03, IF-10 | AC-FX-001, AC-FX-002, AC-FX-003 | — |
| GS-FX-002 | IEffectFactory::create; EffectType | IF-03, IF-10 | AC-FX-004 | — |
| GS-FX-004 | IParameterRegistry::find; IModulationMatrix::evaluate; IEffectSlot::process | IF-03, IF-10 | AC-FX-005 | — |
| GS-FX-006 | IEffectTailPolicy::onPresetSwitch; IEffectChain::process | IF-03, IF-10 | AC-FX-006, AC-FX-007 | — |
| GS-PRS-001 | IPresetGraph::validate; IStateService::captureSnapshot/serialize/prepareRestore | IF-02, IF-05, IF-11 | AC-PRS-002 | — |
| GS-PRS-002 | IPresetLibrary::search/favorite/createInitPatch; IPerformanceBank::lookup | IF-02, IF-05, IF-11 | AC-PRS-001 | — |
| GS-PRS-003 | IPatchTemplateService::applyToCurrentPatch | IF-02, IF-05, IF-11 | AC-PRS-004 | — |
| GS-PRS-004 | IPresetCodec::serialize/inspectAndMigrate; ParameterScaleVersion | IF-02, IF-05, IF-11 | AC-PRS-008 | — |
| GS-PRS-005 | ISampleCollector::collect; ISampleRelinker::resolveByHash | IF-02, IF-05, IF-11 | AC-PRS-005, AC-PRS-006 | — |
| GS-PRS-006 | IStateService::captureSnapshot/serialize/prepareRestore; StateSnapshot | IF-02, IF-05, IF-11 | AC-PRS-007 | DEC-025 מאושרת: הקלטות לא שמורות מוטמעות; דגימות ספרייה נשארות references חיצוניים עם relink/collect. |
| GS-PRS-007 | IUndoHistory::apply/undo/redo | IF-02, IF-05, IF-11 | AC-PRS-009 | — |
| GS-PRS-008 | IClipboardService::copy/pastePatch/pasteLayer/pasteMatrixRow/pasteFxSlot | IF-02, IF-05, IF-11 | AC-PRS-009 | — |
| GS-PRS-009 | IPresetCommentStore::set/get | IF-02, IF-05, IF-11 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-UI-001 | IWaveformViewModel::snapshot; IUiSnapshotSource::tryReadLatest | IF-02, IF-04, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-UI-002 | IEngineControlMap::resolveEightKnobBank; IControllerMap::bind | IF-02, IF-04, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-UI-003 | IWindowShapeViewModel::shape; IGrainParameterModel::density | IF-02, IF-04, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-UI-004 | IEditorLayoutModel::resize/scale/navigate; ISharedEditorViewFactory::create | IF-02, IF-04, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-UI-005 | IEngineMetricsSnapshot::read; IUiSnapshotSource::tryReadLatest | IF-02, IF-04, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-UI-006 | ISharedEditorViewFactory::create; IAUProcessAdapter; IVST3ProcessAdapter | IF-02, IF-04, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-UI-007 | IMidiLearnService::bind; IControllerMap::resolveConflict | IF-02, IF-04, IF-09 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-NFR-001 | IRealtimeAudit::checkCallbackPath; IAudioEngine::process; IAudioCommandSink::tryPublish | IF-01…IF-12 | AC-AUD-026, AC-AUD-027, AC-AUD-013 | — |
| GS-NFR-002 | IRealtimeLoadAcceptanceProfile::evaluate | IF-01…IF-12 | §7–8 — no-dropout reference-load gate; threshold deferred to Phase 5 | Q-T07 ומכונת reference עדיין פתוחים; אין לקבוע threshold או להריץ benchmark ללא אישור. |
| GS-NFR-003 | IParameterSmoother::process; IGrainWindowTransition::processActiveGrains; IAudioCommandSink::tryPublish | IF-01…IF-12 | AC-FX-007 | — |
| GS-NFR-004 | IAudioEngine::reportedLatencySamples; IHostLatencyAdapter::reportLatency | IF-01…IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | Zero-added-latency נדרש למנועי R1; אין להסיק התחייבות ל-Slice/Tape/Spectral שאינם R1. |
| GS-NFR-005 | IRandomStream::seed; IOfflineRenderer::render | IF-01…IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-NFR-006 | IParameterRegistry::find; HostParameterManifest::appendOnlyLookup | IF-01…IF-12 | §7 — host-visible parameter stability gate | Q-T12 פתוח; אין להקפיא host parameter IDs או manifest לפני אישור. |
| GS-NFR-007 | ReleaseValidationMatrix::auval/pluginval/hostMatrix | IF-01…IF-12 | §7 — AU validation/pluginval and host compatibility gate | Q-P09 פתוח; רשימת Hosts וגרסאות validation עדיין לא אושרה. |
| GS-NFR-008 | IMemoryAdmission::reserve/release; IRecordingBufferPolicy::reserve | IF-01…IF-12 | AC-AUD-011 | — |
| GS-NFR-009 | ISampleImportService::prepare; IProgressiveWaveformModel::publishPartialPeaks | IF-01…IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-NFR-010 | ISampleRatePolicy::prepareForRate; IAudioEngine::process | IF-01…IF-12 | AC-AUD-014 | — |
| GS-NFR-011 | IDspTestRegistry::casesForComponent; IOfflineRenderer::render | IF-01…IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | — |
| GS-NFR-012 | IStateService::inspectAndMigrate; ISampleRelinker::resolveByHash | IF-01…IF-12 | AC-SMP-008, AC-PRS-010 | — |
| GS-NFR-013 | ReleasePackagingManifest::minimumMacOS/signAndNotarize | IF-01…IF-12 | אין AC מפורש — Phase 5: להוסיף AC או rationale | Q-T03 פתוח; minimum macOS, signing/notarization policy טרם נקבעו. |

**Inventory check:** 124 unique R1 requirement IDs; 49 have a direct AC-table reference and four additional IDs have a cross-domain release-gate link (53 total); 71 have neither. AC trace shorthand is expanded before counting. No acceptance IDs were invented to fill the remaining gaps.

## 3. Acceptance-criteria coverage gap retained explicitly

The **71 R1 IDs without a direct AC-table reference or a separate cross-domain release-gate link** are:

- GS-PLT-002
- GS-PLT-003
- GS-ARC-001
- GS-ARC-002
- GS-ARC-003
- GS-ARC-004
- GS-ARC-006
- GS-ARC-007
- GS-ARC-008
- GS-GRN-001
- GS-GRN-002
- GS-GRN-003
- GS-GRN-004
- GS-GRN-005
- GS-GRN-006
- GS-GRN-007
- GS-GRN-008
- GS-GRN-009
- GS-GRN-010
- GS-GRN-011
- GS-GRN-012
- GS-GRN-013
- GS-GRN-014
- GS-GRN-015
- GS-GRN-016
- GS-GRN-017
- GS-GRN-018
- GS-GRN-019
- GS-GRN-020
- GS-CHD-001
- GS-CHD-002
- GS-CHD-003
- GS-CHD-004
- GS-CHD-005
- GS-VOI-001
- GS-VOI-002
- GS-VOI-003
- GS-VOI-004
- GS-VOI-005
- GS-VOI-006
- GS-VOI-008
- GS-VOI-009
- GS-VOI-010
- GS-SMG-002
- GS-SMG-003
- GS-SMG-004
- GS-SMG-005
- GS-SMG-006
- GS-SMG-007
- GS-SMG-019
- GS-SEQ-001
- GS-SEQ-002
- GS-SEQ-003
- GS-SEQ-004
- GS-SEQ-008
- GS-SEQ-009
- GS-SEQ-011
- GS-SEQ-012
- GS-PRS-009
- GS-UI-001
- GS-UI-002
- GS-UI-003
- GS-UI-004
- GS-UI-005
- GS-UI-006
- GS-UI-007
- GS-NFR-004
- GS-NFR-005
- GS-NFR-009
- GS-NFR-011
- GS-NFR-013

These rows are not treated as untestable or waived. Phase 5 must add a requirement-specific acceptance/test link or document a precise rationale linking the requirement to an existing cross-cutting criterion. The current count is 49 unique R1 IDs referenced by explicit AC table rows, plus four additional IDs covered by the cross-domain release gates in `R1_ACCEPTANCE_CRITERIA.md` §7 (`GS-PLT-004`, `GS-NFR-002`, `GS-NFR-006`, `GS-NFR-007`). Thus 53/124 have an acceptance/release-gate link and 71/124 remain without one. A candidate API symbol is not acceptance coverage, and neither is evidence of a passing test. Shorthand references in the AC tables are expanded to their full requirement IDs for this count.

See `PHASE5_TEST_DESIGN_BACKLOG.md` for the requirement-level Phase 5 planning backlog.

## 4. Cross-cutting consistency checks

- **Layer model:** four independent layer runtimes; engine selection is Granular or Sampler for R1 only (DEC-017/018).
- **Signal path:** per-voice engine + sub-oscillator → LPF → HPF; layer FX chain remains four serial slots; the parallel dry live-input path is summed before master as required by GS-ARC-007.
- **Effects:** four slots and seven R1 effect types remain distinct concepts (DEC-023).
- **MIDI:** channel sharing reaches every matching layer (DEC-034); Program Change only resolves explicit active-bank mappings (DEC-033); MPE pitch range remains 48 semitones (DEC-038); MIDI Clock behavior remains DEC-039; no R1 MIDI feedback or named-controller templates (DEC-021).
- **Modulation:** ten macros per patch (DEC-036); LFO Poly/Mono per LFO with Mono default (DEC-024); macro base values remain separate from per-voice modulation contributions (DEC-031).
- **Sampler:** Env Pile-up remains OFF by default (DEC-037).
- **Freeze/state:** Source Snapshot Freeze remains DEC-042; unsaved recording buffers remain embedded under DEC-025. Global Freeze partial-preparation policy is still an owner decision and is not resolved by symbol mapping.
- **Resource constraints:** six-minute stereo sample target remains subject to safe measured feasibility (DEC-028); no numeric grain budget, memory headroom, callback capacity or performance threshold is introduced here.
- **Host formats:** AU/VST3/Standalone adapters normalize into the shared core; this document does not select an AU component subtype, framework, final bus layout or host matrix.

## 5. Open engineering and owner gates

- AR-01: same-offset event tie-break and parameter-writer precedence remain open.
- AR-02: deterministic voice/grain overload policy and numeric capacities remain open.
- Q-T01/Q-T03/Q-L04/Q-L05: framework, macOS deployment/architecture and licensing decisions remain open.
- Q-T12: final stable host parameter IDs and exposure manifest remain owner-gated.
- Q-T07: no benchmark or numeric global grain budget without separate explicit authorization.
- DEC-042: successful-subset versus all-or-nothing Global Freeze preparation failure semantics remain owner-gated.
- Q-P09: host/format validation matrix remains to be approved and executed on actual target hosts.

## 6. Exit meaning

This document closes only the candidate symbol assignment subtask for the 124 R1 rows. It does not close API design, method signature review, host adapter validation, test design, implementation readiness, acceptance coverage, CI, DSP implementation or release readiness. The next useful work is to complete the 71-row Phase 5 test-design backlog without inventing product behavior, then review the AU/VST3 callback and bus contracts against official host constraints once the relevant SDK/format choices are approved.

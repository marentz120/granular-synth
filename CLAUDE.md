# GRANULAR SYNTH — Shared Agent Instructions

These instructions apply to Claude Chat and Claude Code. The repository is the project source of truth.

## Mission
Develop GRANULAR SYNTH, a professional granular synthesizer for macOS. Proposed targets: standalone app, AU, and VST3. The primary functional benchmark is Tasty Chips GR-MEGA. Official support/manual source: https://www.tastychips.nl/gr-mega-support-page/

Use GR-MEGA as a functional reference only. Do not copy proprietary firmware or source code.

## Roles
- Human owner: final product and scope authority.
- ChatGPT: product/technical direction, architecture review, planning, milestone gates and quality review.
- Claude Chat: research, reference analysis, specifications, technical options, task preparation and review preparation.
- Claude Code: implementation, tests, debugging and code documentation.

Do not claim another agent approved a decision unless that approval is recorded in the repository or explicitly supplied by the owner.

## Source of truth
Before acting, read this file and relevant docs. Check the decisions log and current repository state. Record durable decisions, requirements and implementation outcomes here; chat history is not the source of truth. Never silently change approved requirements.

## Engineering principles
- Professional sound quality and stable real-time performance are mandatory.
- No blocking I/O, mutex waits, or unpredictable operations in the audio callback.
- Avoid heap allocation/deallocation in the audio callback.
- Define bounded resource limits and deterministic overload behavior.
- Prepare sample data off-thread and transfer ownership safely.
- Apply parameter smoothing where needed to prevent clicks and zipper noise.
- Separate DSP, UI, host integration and persistence.
- Make DSP independently testable; measure CPU, memory, latency and stability.
- Maintain consistent parameter semantics between standalone and plug-in formats.

## Reference-analysis discipline
For every GR-MEGA feature, label it:
- Manual-confirmed: cite manual revision, section and page.
- Interpretation: explain uncertainty.
- Proposal: GRANULAR SYNTH design recommendation.
- Unverified: evidence still required.

Never present assumptions as manufacturer specifications. Verify numeric limits against the actual target manual revision.

## Development phases and gates
1. Reference research and feature map.
2. Product requirements and release scope.
3. DSP/application architecture.
4. UI/UX specification.
5. Implementation plan and acceptance criteria.
6. Incremental implementation.
7. Testing, profiling and optimization.
8. Packaging and release.

Do not skip gates without owner approval. Do not attempt to build the entire instrument in one task.

## Claude Code workflow
- Inspect the repository and relevant docs before editing.
- Work on a dedicated feature branch.
- Make small, reviewable commits.
- Add/update tests with implementation.
- Run builds and relevant tests; report exact commands/results.
- Open a pull request for substantial work; do not merge it without review.
- Report changed files, known limitations, performance implications and unresolved risks.

## Documentation
Maintain the project documents listed in docs/README.md. Keep docs/DECISIONS_LOG.md and docs/OPEN_QUESTIONS.md current.

## Current authorization
Current phase: GR-MEGA reference research and product definition.

Authorized: read public documentation, build a traceable feature map, draft requirements, architecture options, roadmap and risk analysis.

Not yet authorized: production DSP implementation, framework lock-in, or scope expansion. Wait for technical review and owner approval before implementation.

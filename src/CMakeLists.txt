# Module layout mirrors DSP_ARCHITECTURE.md §3, placed under src/ (deviation recorded in
# docs/BUILD_AND_TOOLCHAIN_RECORD.md and approved in the Milestone A0 authorization).
#
# Milestone A0 creates the core target only. sample/, engine/, state/, host/ and ui/ are NOT
# created here: they require decisions and milestones that are not authorized yet.
#
#   sample/  engine/  state/  -> Milestones B, C, D, E
#   host/                     -> Milestone F; blocked by Q-T01, Q-T03, Q-L04
#   ui/                       -> Phase 4 + Milestone F

add_subdirectory(core)

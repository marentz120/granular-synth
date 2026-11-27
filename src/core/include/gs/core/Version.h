// GRANULAR SYNTH — core build identity.
//
// Milestone A0 placeholder. This header exists so that the framework-free core target has
// something to compile, link and assert against end to end. It carries no audio, DSP,
// parameter or product behaviour, and the version number is a build placeholder, not a
// release commitment.

#ifndef GS_CORE_VERSION_H
#define GS_CORE_VERSION_H

namespace gs::core {

/// Build-identity string supplied by the build system (project version).
/// Returns a null-terminated string with static storage duration.
const char* versionString() noexcept;

/// True when the core was compiled as a framework-independent translation unit.
/// Milestone A0 always returns true; it is a placeholder for the Q-T02 core-boundary
/// invariant and is asserted by the headless test target so the guarantee is visible
/// in test output rather than only in documentation.
bool isFrameworkIndependentBuild() noexcept;

} // namespace gs::core

#endif // GS_CORE_VERSION_H

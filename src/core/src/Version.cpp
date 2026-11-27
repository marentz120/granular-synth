#include "gs/core/Version.h"

#ifndef GS_CORE_VERSION_STRING
#define GS_CORE_VERSION_STRING "0.0.0-unconfigured"
#endif

namespace gs::core {

const char* versionString() noexcept {
    return GS_CORE_VERSION_STRING;
}

bool isFrameworkIndependentBuild() noexcept {
    return true;
}

} // namespace gs::core

#pragma once

#include "../filesystem/path.h"

namespace Engine {
namespace Dl {

    INTERFACES_EXPORT void *openDll(const std::string &name);
    INTERFACES_EXPORT void closeDll(void *handle);
    INTERFACES_EXPORT const void *getDllSymbol(void *dllHandle, const std::string &symbolName);
    INTERFACES_EXPORT Filesystem::Path getDllFullPath(void *dllHandle, const std::string &symbolName);
}
}
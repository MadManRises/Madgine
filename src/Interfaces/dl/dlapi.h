#pragma once

#include "../filesystem/path.h"
#include "Generic/genericresult.h"

namespace Engine {
namespace Dl {

    ENUM_BASE(DlAPIResult, GenericResult,
        DUMMY);

    INTERFACES_EXPORT void *openDll(const std::string &name);
    INTERFACES_EXPORT void closeDll(void *handle);
    INTERFACES_EXPORT const void *getDllSymbol(void *dllHandle, const std::string &symbolName);
    INTERFACES_EXPORT Filesystem::Path getDllFullPath(void *dllHandle, const std::string &symbolName);
    INTERFACES_EXPORT DlAPIResult getError(const char *op);
}
}
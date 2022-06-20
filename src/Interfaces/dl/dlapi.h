#pragma once

#include "../filesystem/path.h"
#include "Generic/genericresult.h"
#include "Generic/functor.h"

namespace Engine {
namespace Dl {

    ENUM_BASE(DlAPIResult, GenericResult,
        DEPENDENCY_ERROR)

    INTERFACES_EXPORT void closeDll(void *handle);
    using DlHandle = std::unique_ptr<void, Functor<&closeDll>>;
    INTERFACES_EXPORT DlHandle openDll(const std::string &name);

    INTERFACES_EXPORT const void *getDllSymbol(const DlHandle &dllHandle, const std::string &symbolName);
    INTERFACES_EXPORT Filesystem::Path getDllFullPath(const DlHandle &dllHandle, const std::string &symbolName);
    INTERFACES_EXPORT DlAPIResult getError(const char *op);

    
}
}
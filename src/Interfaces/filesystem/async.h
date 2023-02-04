#pragma once

#include "asyncoperations.h"

namespace Engine {
namespace Filesystem {

    INTERFACES_EXPORT void checkAsyncIOCompletion();
    INTERFACES_EXPORT void cancelAllAsyncIO();
    INTERFACES_EXPORT size_t pendingIOOperationCount();

    INTERFACES_EXPORT AsyncFileRead readFileAsync(const Path &path);

}
}
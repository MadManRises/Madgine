#pragma once

#include "pyobjectptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {
    
        struct MADGINE_PYTHON3_EXPORT PyDictPtr : PyObjectPtr {
            PyDictPtr() = default;
            PyDictPtr(const std::map<std::string, PyObjectPtr> &values);

            using PyObjectPtr::operator=;
        };
    
    }
}
}
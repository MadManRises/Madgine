#pragma once

#include "Meta/keyvalue/typedscopeptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        extern PyTypeObject PyTypedScopePtrType;

        struct PyTypedScopePtr {
            PyObject_HEAD
                TypedScopePtr mPtr;
        };

    }
}
}

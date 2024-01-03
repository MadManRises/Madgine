#pragma once

#include "pyobjectptr.h"

typedef struct _frame PyFrameObject;

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT PyFramePtr : PyObjectPtr {
            PyFramePtr() = default;
            PyFramePtr(PyFrameObject *frame);

            static PyFramePtr fromBorrowed(PyFrameObject *frame);

            using PyObjectPtr::operator=;

            operator PyFrameObject *() const;

            PyFrameObject *release();

        };

    
    }
}
}
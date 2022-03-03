#include "../../python3lib.h"

#include "pyquaternion.h"

#include "../pyobjectutil.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyQuaternion_str(PyQuaternion *self)
        {
            std::stringstream ss;
            ss << self->mQuaternion;
            return PyUnicode_FromString(ss.str().c_str());
        }

        PyTypeObject PyQuaternionType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.Quaternion",
            .tp_basicsize = sizeof(PyQuaternion),
            .tp_itemsize = 0,
            .tp_dealloc = &PyDealloc<PyQuaternion, &PyQuaternion::mQuaternion>,
            .tp_str = (reprfunc)PyQuaternion_str,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = "Python implementation of Quaternion",
            .tp_new = PyType_GenericNew,
        };

    }
}
}
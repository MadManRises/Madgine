#include "../../python3lib.h"

#include "pyquaternion.h"

#include "../pyobjectutil.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyQuaternion_str(PyQuaternion *self)
        {
            return PyUnicode_FromString((std::stringstream {} << self->mQuaternion).str().c_str());
        }

        PyTypeObject PyQuaternionType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.Quaternion",
            .tp_doc = "Python implementation of Quaternion",
            .tp_basicsize = sizeof(PyQuaternion),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyQuaternion::mQuaternion>,
            .tp_dealloc = &PyDealloc<PyQuaternion, &PyQuaternion::mQuaternion>,
            .tp_str = (reprfunc)PyQuaternion_str,
        };

    }
}
}
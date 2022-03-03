#include "../../python3lib.h"

#include "pyvector3.h"

#include "../pyobjectutil.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyVector3_str(PyVector3 *self)
        {
            std::stringstream ss;
            ss << self->mVector;
            return PyUnicode_FromString(ss.str().c_str());
        }

        PyTypeObject PyVector3Type = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.Vector3",
            .tp_basicsize = sizeof(PyVector3),
            .tp_itemsize = 0,
            .tp_dealloc = &PyDealloc<PyVector3, &PyVector3::mVector>,
            .tp_str = (reprfunc)PyVector3_str,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = "Python implementation of Vector3",
            .tp_new = PyType_GenericNew,
        };

    }
}
}
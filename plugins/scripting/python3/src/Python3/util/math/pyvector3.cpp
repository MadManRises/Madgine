#include "../../python3lib.h"

#include "pyvector3.h"

#include "../pyobjectutil.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyVector3_str(PyVector3 *self)
        {
            return PyUnicode_FromString((std::stringstream {} << self->mVector).str().c_str());
        }

        PyTypeObject PyVector3Type = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.Vector3",
            .tp_doc = "Python implementation of Vector3",
            .tp_basicsize = sizeof(PyVector3),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyVector3::mVector>,
            .tp_dealloc = &PyDealloc<PyVector3, &PyVector3::mVector>,
            .tp_str = (reprfunc)PyVector3_str,
        };

    }
}
}
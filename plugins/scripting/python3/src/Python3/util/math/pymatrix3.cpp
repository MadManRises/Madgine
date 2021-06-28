#include "../../python3lib.h"

#include "pymatrix3.h"

#include "../pyobjectutil.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyMatrix3_str(PyMatrix3 *self)
        {
            std::stringstream ss;
            ss << self->mMatrix;
            return PyUnicode_FromString(ss.str().c_str());
        }

        PyTypeObject PyMatrix3Type = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.Matrix3",
            .tp_doc = "Python implementation of Matrix3",
            .tp_basicsize = sizeof(PyMatrix3),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyVector3::mVector>,
            .tp_dealloc = &PyDealloc<PyMatrix3, &PyMatrix3::mMatrix>,
            .tp_str = (reprfunc)PyMatrix3_str,
        };

    }
}
}
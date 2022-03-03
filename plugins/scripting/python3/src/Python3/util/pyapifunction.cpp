#include "../python3lib.h"

#include "pyapifunction.h"

#include "Meta/keyvalue/valuetype.h"

#include "pyobjectutil.h"

#include "../python3env.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyApiFunction_call(PyApiFunction *self, PyObject *args, PyObject *kwargs)
        {
            size_t argCount = PyTuple_Size(args);
            ArgumentList arguments;
            arguments.reserve(argCount);

            for (size_t i = 0; i < argCount; ++i) {
                fromPyObject(arguments.emplace_back(), PyTuple_GetItem(args, i));
            }
            
            ValueType retVal;
            Py_BEGIN_ALLOW_THREADS;
            self->mFunction(retVal, arguments);
            Py_END_ALLOW_THREADS;

            return toPyObject(retVal);
        }

        PyTypeObject PyApiFunctionType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.ApiFunction",
            .tp_basicsize = sizeof(PyApiFunction),
            .tp_itemsize = 0,
            .tp_dealloc = &PyDealloc<PyApiFunction, &PyApiFunction::mFunction>,
            .tp_call = (ternaryfunc)PyApiFunction_call,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = "Python implementation of ApiFunction",
            .tp_new = PyType_GenericNew,
        };

    }
}
}
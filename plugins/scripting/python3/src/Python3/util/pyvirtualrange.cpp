#include "../python3lib.h"

#include "pyvirtualrange.h"

#include "Meta/keyvalue/valuetype.h"

#include "pyobjectutil.h"

#include "Meta/keyvalue/keyvaluepair.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyVirtualSequenceRange_iter(PyVirtualSequenceRange *self)
        {
            return toPyObject(self->mRange.begin());
        }

        PyTypeObject PyVirtualSequenceRangeType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.VirtualSequenceRange",
            .tp_doc = "Python implementation of KeyValueVirtualSequenceRange",
            .tp_basicsize = sizeof(PyVirtualSequenceRange),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyVirtualSequenceRange::mRange>,
            .tp_dealloc = &PyDealloc<PyVirtualSequenceRange, &PyVirtualSequenceRange::mRange>,
            .tp_iter = (getiterfunc)PyVirtualSequenceRange_iter,
        };

        static PyObject *
        PyVirtualAssociativeRange_iter(PyVirtualAssociativeRange *self)
        {
            return toPyObject(self->mRange.begin());
        }

        static PyObject *
        PyVirtualAssociativeRange_subscript(PyVirtualAssociativeRange *self, PyObject *key)
        {
            ValueType val;
            fromPyObject(val, key);
            auto it = std::find_if(self->mRange.begin(), self->mRange.end(), [&](const KeyValuePair &p) { return p.mKey == val; });
            if (it == self->mRange.end()) {
                PyErr_Format(PyExc_KeyError, "No key %R in %R!", key, self);
                return NULL;
            }
            return toPyObject(it->mValue);
        }

        static PyMappingMethods PyVirtualAssociativeRangeMapping {
            .mp_subscript = (binaryfunc)&PyVirtualAssociativeRange_subscript,
        };

        PyTypeObject PyVirtualAssociativeRangeType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.VirtualAssociativeRange",
            .tp_doc = "Python implementation of KeyValueVirtualAssociativeRange",
            .tp_basicsize = sizeof(PyVirtualAssociativeRange),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyVirtualAssociativeRange::mRange>,
            .tp_dealloc = &PyDealloc<PyVirtualAssociativeRange, &PyVirtualAssociativeRange::mRange>,
            .tp_iter = (getiterfunc)PyVirtualAssociativeRange_iter,
            .tp_as_mapping = &PyVirtualAssociativeRangeMapping,
        };

    }
}
}
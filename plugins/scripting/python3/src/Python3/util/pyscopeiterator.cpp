#include "../python3lib.h"

#include "pyscopeiterator.h"

#include "pyobjectutil.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyScopeIterator_iter(PyObject *self)
        {
            Py_INCREF(self);
            return self;
        }

        static PyObject *
        PyScopeIterator_next(PyScopeIterator *self)
        {
            if (self->mIt == self->mIt.end())
                return NULL;
            ValueType v;
            self->mIt->value(v);
            PyObject *result = Py_BuildValue("sN", self->mIt->key(), toPyObject(v));
            ++self->mIt;
            return result;
        }

        PyTypeObject PyScopeIteratorType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.ScopeIterator",
            .tp_doc = "Python implementation of ScopeIterator",
            .tp_basicsize = sizeof(PyScopeIterator),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyScopeIterator::mIt>,
            .tp_dealloc = &PyDealloc<PyScopeIterator, &PyScopeIterator::mIt>,
            .tp_iter = (getiterfunc)PyScopeIterator_iter,
            .tp_iternext = (iternextfunc)PyScopeIterator_next,
        };

    }
}
}
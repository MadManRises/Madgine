#include "../python3lib.h"

#include "pytypedscopeptr.h"

#include "Meta/keyvalue/scopeiterator.h"

#include "Meta/keyvalue/valuetype.h"

#include "pyobjectutil.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyTypedScopePtr_get(PyTypedScopePtr *self, PyObject *args)
        {
            const char *name;

            if (!PyArg_Parse(args, "s", &name))
                return NULL;

            ScopeIterator it = self->mPtr.find(name);

            if (it == self->mPtr.end()) {
                PyErr_Format(PyExc_AttributeError, "Could not find attribute '%s' in %R!", name, self);
                return NULL;
            }
            ValueType v;
            it->value(v);
            return toPyObject(v);
        }

        static PyObject* TypedScopePtr_iter(const TypedScopePtr& p) {
            if (!p) {
                PyErr_SetString(PyExc_TypeError, "Nullptr is not iterable!");
                return NULL;
            }
            ScopeIterator proxyIt = p.find("__proxy");
            if (proxyIt != p.end()) {
                ValueType proxy;
                proxyIt->value(proxy);
                if (proxy.is<TypedScopePtr>()) {
                    return TypedScopePtr_iter(proxy.as<TypedScopePtr>());
                }
            }
            return toPyObject(p.begin());
        }

        static PyObject *
        PyTypedScopePtr_iter(PyTypedScopePtr *self)
        {
            return TypedScopePtr_iter(self->mPtr);
        }

        static PyObject *
        PyTypedScopePtr_str(PyTypedScopePtr *self)
        {
            return PyUnicode_FromString(self->mPtr.name().c_str());
        }

        PyTypeObject PyTypedScopePtrType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.TypedScopePtr",
            .tp_doc = "Python implementation of TypedScopePtr",
            .tp_basicsize = sizeof(PyTypedScopePtr),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyTypedScopePtr::mPtr>,
            .tp_dealloc = &PyDealloc<PyTypedScopePtr, &PyTypedScopePtr::mPtr>,
            .tp_getattro = (getattrofunc)PyTypedScopePtr_get,
            .tp_iter = (getiterfunc)PyTypedScopePtr_iter,
            .tp_str = (reprfunc)PyTypedScopePtr_str,
        };

    }
}
}
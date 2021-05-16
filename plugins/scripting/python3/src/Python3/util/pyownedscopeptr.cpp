#include "../python3lib.h"

#include "pyownedscopeptr.h"

#include "Meta/keyvalue/scopeiterator.h"

#include "Meta/keyvalue/valuetype.h"

#include "pyobjectutil.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyOwnedScopePtr_get(PyOwnedScopePtr *self, PyObject *args)
        {
            const char *name;

            if (!PyArg_Parse(args, "s", &name))
                return NULL;

            ScopeIterator it = self->mPtr.get().find(name);

            if (it == self->mPtr.get().end()) {
                PyErr_Format(PyExc_AttributeError, "Could not find attribute '%s' in %R!", name, self);
                return NULL;
            }
            ValueType v;
            it->value(v);
            return toPyObject(v);
        }

        static PyObject* OwnedScopePtr_iter(const TypedScopePtr& p) {
            if (!p) {
                PyErr_SetString(PyExc_TypeError, "Nullptr is not iterable!");
                return NULL;
            }
            ScopeIterator proxyIt = p.find("__proxy");
            if (proxyIt != p.end()){
                ValueType proxy;
                proxyIt->value(proxy);
                if (proxy.is<TypedScopePtr>()) {
                    return OwnedScopePtr_iter(proxy.as<TypedScopePtr>());
                }
            }
            return toPyObject(p.begin());
        }

        static PyObject *
        PyOwnedScopePtr_iter(PyOwnedScopePtr *self)
        {
            return OwnedScopePtr_iter(self->mPtr.get());
        }

        static PyObject *
        PyOwnedScopePtr_str(PyOwnedScopePtr *self)
        {
            return PyUnicode_FromString(self->mPtr.name().c_str());
        }

        PyTypeObject PyOwnedScopePtrType = {
            PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name
            = "Environment.OwnedScopePtr",
            .tp_doc = "Python implementation of OwnedScopePtr",
            .tp_basicsize = sizeof(PyOwnedScopePtr),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_new = PyType_GenericNew,
            //.tp_init = &PyInit<&PyOwnedScopePtr::mPtr>,
            .tp_dealloc = &PyDealloc<PyOwnedScopePtr, &PyOwnedScopePtr::mPtr>,
            .tp_getattro = (getattrofunc)PyOwnedScopePtr_get,
            .tp_iter = (getiterfunc)PyOwnedScopePtr_iter,
            .tp_str = (reprfunc)PyOwnedScopePtr_str,
        };

    }
}
}
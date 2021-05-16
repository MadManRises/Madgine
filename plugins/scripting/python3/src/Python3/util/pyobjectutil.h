#pragma once

namespace Engine {
namespace Scripting {
    namespace Python3 {

        /*template <typename T, typename V>
        void PyInit(V(T::* p), PyObject* self) {
            new (&((T *)(self)->*p)) V();
        }

        template <auto p>
        int PyInit(PyObject* self, PyObject* args, PyObject* kwds) {
            PyInit(p, self);
            return 0;
        }*/

        template <typename T, typename V>
        void PyDealloc(V(T::*p), PyObject *self)
        {
            ((T*)(self)->*p).~V();
        }

        template <typename T, auto p>
        void PyDealloc(PyObject *self)
        // destruct the object
        {
            PyDealloc(p, self);
            Py_TYPE(self)->tp_free(self);
        }

        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const ValueType &val);

        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(std::monostate);

        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(int i);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(bool b);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(float f);

        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const TypedScopePtr &scope);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const OwnedScopePtr &scope);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const ScopeIterator &it);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const ApiFunction &function);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const BoundApiFunction &function);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const KeyValueVirtualSequenceRange &range);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const KeyValueVirtualAssociativeRange &range);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const VirtualIterator<KeyValuePair> &it);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const VirtualIterator<ValueTypeRef> &it);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const CoWString &s);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const Vector3 &v);
        MADGINE_PYTHON3_EXPORT PyObject *toPyObject(const Quaternion &v);

        MADGINE_PYTHON3_EXPORT void fromPyObject(ValueType &retVal, PyObject *obj);

        MADGINE_PYTHON3_EXPORT ExtendedValueTypeDesc PyToValueTypeDesc(PyObject *obj);

    }
}
}
#include "../python3lib.h"

#include "pyobjectutil.h"

#include "math/pyquaternion.h"
#include "math/pyvector3.h"
#include "pyapifunction.h"
#include "pyboundapifunction.h"
#include "pyownedscopeptr.h"
#include "pyscopeiterator.h"
#include "pytypedscopeptr.h"
#include "pyvirtualiterator.h"
#include "pyvirtualrange.h"

#include "Meta/keyvalue/valuetype.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        PyObject *toPyObject(const ValueType &val)
        {
            return val.visit([](const auto &e) -> PyObject * {
                return toPyObject(e);
            });
        }

        PyObject *toPyObject(std::monostate)
        {
            Py_RETURN_NONE;
        }

        PyObject *toPyObject(int i)
        {
            return PyLong_FromLong(i);
        }

        PyObject *toPyObject(uint64_t i)
        {
            return PyLong_FromLong(i);
        }

        PyObject *toPyObject(bool b)
        {
            return PyBool_FromLong(b);
        }

        PyObject *toPyObject(float f)
        {
            return PyFloat_FromDouble(f);
        }

        PyObject *toPyObject(const TypedScopePtr &scope)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyTypedScopePtrType, NULL);
            new (&reinterpret_cast<PyTypedScopePtr *>(obj)->mPtr) TypedScopePtr(scope);
            return obj;
        }

        PyObject *toPyObject(const OwnedScopePtr &scope)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyOwnedScopePtrType, NULL);
            new (&reinterpret_cast<PyOwnedScopePtr *>(obj)->mPtr) OwnedScopePtr(scope);
            return obj;
        }

        PyObject *toPyObject(const ScopeIterator &it)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyScopeIteratorType, NULL);
            new (&reinterpret_cast<PyScopeIterator *>(obj)->mIt) ScopeIterator(it);
            return obj;
        }

        PyObject *toPyObject(const ApiFunction &function)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyApiFunctionType, NULL);
            new (&reinterpret_cast<PyApiFunction *>(obj)->mFunction) ApiFunction(function);
            return obj;
        }

        PyObject *toPyObject(const BoundApiFunction &function)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyBoundApiFunctionType, NULL);
            new (&reinterpret_cast<PyBoundApiFunction *>(obj)->mFunction) BoundApiFunction(function);
            return obj;
        }

        PyObject *toPyObject(const KeyValueVirtualSequenceRange &range)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyVirtualSequenceRangeType, NULL);
            new (&reinterpret_cast<PyVirtualSequenceRange *>(obj)->mRange) KeyValueVirtualSequenceRange(range);
            return obj;
        }

        PyObject *toPyObject(const KeyValueVirtualAssociativeRange &range)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyVirtualAssociativeRangeType, NULL);
            new (&reinterpret_cast<PyVirtualAssociativeRange *>(obj)->mRange) KeyValueVirtualAssociativeRange(range);
            return obj;
        }

        PyObject *toPyObject(const VirtualIterator<ValueTypeRef> &it)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyVirtualSequenceIteratorType, NULL);
            new (&reinterpret_cast<PyVirtualSequenceIterator *>(obj)->mIt) VirtualIterator<ValueTypeRef>(it);
            return obj;
        }

        PyObject *toPyObject(const VirtualIterator<KeyValuePair> &it)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyVirtualAssociativeIteratorType, NULL);
            new (&reinterpret_cast<PyVirtualAssociativeIterator *>(obj)->mIt) VirtualIterator<KeyValuePair>(it);
            return obj;
        }

        PyObject *toPyObject(const CoWString &s)
        {
            return PyUnicode_FromStringAndSize(s.data(), s.size());
        }

        PyObject *toPyObject(const Vector2 &v)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Vector2> yet");
            return NULL;
        }

        PyObject *toPyObject(const Vector3 &v)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyVector3Type, NULL);
            new (&reinterpret_cast<PyVector3 *>(obj)->mVector) Vector3(v);
            return obj;
        }

        PyObject *toPyObject(const Vector4 &v)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Vector4> yet");
            return NULL;
        }

        PyObject *toPyObject(const Quaternion &q)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyQuaternionType, NULL);
            new (&reinterpret_cast<PyQuaternion *>(obj)->mQuaternion) Quaternion(q);
            return obj;
        }

        PyObject *toPyObject(const ObjectPtr &o)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <ObjectPtr> yet");
            return NULL;
        }

        PyObject *toPyObject(const CoW<Matrix3> &m)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Matrix3> yet");
            return nullptr;
        }

        PyObject *toPyObject(const CoW<Matrix4> &m)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Matrix4> yet");
            return nullptr;
        }

        PyObject *toPyObject(const EnumHolder &e)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <EnumHolder> yet");
            return nullptr;
        }

        PyObject *toPyObject(const Function &f)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Function> yet");
            return nullptr;
        }

        void fromPyObject(ValueType &retVal, PyObject *obj)
        {
            if (PyUnicode_Check(obj)) {
                const char *s;
                if (!PyArg_Parse(obj, "s", &s))
                    throw 0;
                retVal = std::string { s };
            } else if (PyLong_Check(obj)) {
                int i;
                if (!PyArg_Parse(obj, "i", &i))
                    throw 0;
                retVal = i;
            } else {
                throw 0;
            }
        }

        ExtendedValueTypeDesc PyToValueTypeDesc(PyObject *obj)
        {
            assert(PyType_Check(obj));
            PyTypeObject *type = reinterpret_cast<PyTypeObject *>(obj);
            if (type == &PyUnicode_Type) {
                return toValueTypeDesc<std::string>();
            }
            throw 0;
        }

    }
}
}
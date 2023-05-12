#include "../python3lib.h"

#include "pyobjectutil.h"

#include "math/pymatrix3.h"
#include "math/pymatrix4.h"
#include "math/pyquaternion.h"
#include "math/pyvector3.h"
#include "math/pyvector4.h"
#include "pyapifunction.h"
#include "pyboundapifunction.h"
#include "pyownedscopeptr.h"
#include "pyscopeiterator.h"
#include "pytypedscopeptr.h"
#include "pyvirtualiterator.h"
#include "pyvirtualrange.h"
#include "pydictptr.h"
#include "pylistptr.h"

#include "pymoduleptr.h"

#include "Meta/keyvalue/valuetype.h"
#include "Meta/keyvalue/keyvaluepair.h"

#include "Meta/keyvalue/objectinstance.h"
#include "Meta/keyvalue/objectptr.h"

#include "pyobjectptr.h"

#include "python3lock.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct PyObjectInstance : ObjectInstance {

            PyObjectInstance(PyObject *obj)
                : mPtr(obj)
            {
            }

            virtual bool getValue(ValueType &retVal, std::string_view name) const override
            {
                fromPyObject(retVal, mPtr.get(name));
                return true;
            }

            virtual void setValue(std::string_view name, const ValueType &value) override
            {
                throw 0;
            }

            virtual void call(ValueType &retVal, const ArgumentList &args) override
            {
                Python3Lock lock;
                
                fromPyObject(retVal, mPtr.call(args));
            }

            PyObject *get() const
            {
                return mPtr;
            }

            PyObjectPtr mPtr;
        };

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
            PyObject *obj = PyObject_CallObject((PyObject *)&PyVector4Type, NULL);
            new (&reinterpret_cast<PyVector4 *>(obj)->mVector) Vector4(v);
            return NULL;
        }

        PyObject *toPyObject(const Vector2i &v)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Vector2> yet");
            return NULL;
        }

        PyObject *toPyObject(const Vector3i &v)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyVector3Type, NULL);
            new (&reinterpret_cast<PyVector3 *>(obj)->mVector) Vector3(v);
            return obj;
        }

        PyObject *toPyObject(const Vector4i &v)
        {
            /* PyObject *obj = PyObject_CallObject((PyObject *)&PyVector4Type, NULL);
            new (&reinterpret_cast<PyVector4 *>(obj)->mVector) Vector4(v);*/
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Vector4i> yet");
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
            if (!o)
                Py_RETURN_NONE;
            if (const PyObjectInstance *instance = dynamic_cast<const PyObjectInstance *>(o.get())) {
                PyObject *ptr = instance->get();
                Py_INCREF(ptr);
                return ptr;
            }
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <ObjectPtr> yet");
            return NULL;
        }

        PyObject *toPyObject(const CoW<Matrix3> &m)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyMatrix3Type, NULL);
            new (&reinterpret_cast<PyMatrix3 *>(obj)->mMatrix) Matrix3(m);
            return obj;
        }

        PyObject *toPyObject(const CoW<Matrix4> &m)
        {
            PyObject *obj = PyObject_CallObject((PyObject *)&PyMatrix4Type, NULL);
            new (&reinterpret_cast<PyMatrix4 *>(obj)->mMatrix) Matrix4(m);
            return nullptr;
        }

        PyObject *toPyObject(const EnumHolder &e)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <EnumHolder> yet");
            return nullptr;
        }

        PyObject *toPyObject(const KeyValueFunction &f)
        {
            PyErr_SetString(PyExc_NotImplementedError, "Can't convert type <Function> yet");
            return nullptr;
        }

        struct Functor_to_KeyValuePair {            
            void operator()(KeyValuePair &p, const std::pair<PyObject*, PyObject*> &o)
            {
                fromPyObject(p.mKey, o.first);
                ValueType v;
                fromPyObject(v, o.second);
                p.mValue = ValueTypeRef { v };
            }
        };

        struct Functor_to_ValueRef {
            void operator()(ValueTypeRef &r, PyObject *o)
            {
                ValueType v;
                fromPyObject(v, o);
                r = ValueTypeRef { v };
            }
        };

        void fromPyObject(ValueType &retVal, PyObject *obj)
        {
            if (!obj) {
                PyErr_Print();
                throw 0;
            } else if (obj == Py_None) {
                retVal = std::monostate {};
            } else if (PyUnicode_Check(obj)) {
                const char *s;
                if (!PyArg_Parse(obj, "s", &s))
                    throw 0;
                retVal = std::string { s };
            } else if (PyBool_Check(obj)){
                retVal = (obj == Py_True);
            } else if (PyLong_Check(obj)) {
                int i;
                if (!PyArg_Parse(obj, "i", &i))
                    throw 0;
                retVal = i;
            } else if (PyDict_Check(obj)) {
                Py_INCREF(obj);
                retVal = KeyValueVirtualAssociativeRange { PyDictPtr { obj }, Engine::type_holder<Functor_to_KeyValuePair> };
            } else if (PyList_Check(obj)) {
                Py_INCREF(obj);
                retVal = KeyValueVirtualSequenceRange { PyListPtr { obj }, Engine::type_holder<Functor_to_ValueRef> };
            } else if (obj->ob_type == &PyTypedScopePtrType) {
                retVal = reinterpret_cast<PyTypedScopePtr *>(obj)->mPtr;
            } else {
                Py_INCREF(obj);
                retVal = ObjectPtr { std::make_unique<PyObjectInstance>(obj) };
            }
        }

        ExtendedValueTypeDesc PyToValueTypeDesc(PyObject *obj)
        {
            assert(PyType_Check(obj));
            PyTypeObject *type = reinterpret_cast<PyTypeObject *>(obj);
            if (type == &PyUnicode_Type) {
                return toValueTypeDesc<std::string>();
            } else if (obj == PyModulePtr{ "inspect" }.get("Parameter").get("empty")) {
                return toValueTypeDesc<ValueType>();
            }
            throw 0;
        }

    }
}
}
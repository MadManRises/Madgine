#include "python3lib.h"

#include "python3env.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/core/keyvalueregistry.h"

#include "util/math/pymatrix3.h"
#include "util/math/pyquaternion.h"
#include "util/math/pyvector3.h"
#include "util/pyapifunction.h"
#include "util/pyboundapifunction.h"
#include "util/pyobjectutil.h"
#include "util/pyownedscopeptr.h"
#include "util/pyscopeiterator.h"
#include "util/pytypedscopeptr.h"
#include "util/pyvirtualiterator.h"
#include "util/pyvirtualrange.h"

#include "python3fileloader.h"

#include <iostream>

UNIQUECOMPONENT(Engine::Scripting::Python3::Python3Environment)

METATABLE_BEGIN(Engine::Scripting::Python3::Python3Environment)
METATABLE_END(Engine::Scripting::Python3::Python3Environment)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        static PyObject *
        PyEnvironment_get(PyObject *self, PyObject *args)
        {
            const char *name;

            if (!PyArg_ParseTuple(args, "s", &name))
                return NULL;

            auto it = KeyValueRegistry::globals().find(name);
            if (it != KeyValueRegistry::globals().end()) {
                return toPyObject(it->second);
            }

            auto it2 = KeyValueRegistry::workgroupLocals().find(name);
            if (it2 != KeyValueRegistry::workgroupLocals().end()) {
                return toPyObject(it2->second);
            }

            PyErr_Format(PyExc_AttributeError, "Could not find attribute '%s' in %R!", name, self);
            return NULL;
        }

        static PyMethodDef PyEnvironmentMethods[] = {
            { "__getattr__", PyEnvironment_get, METH_VARARGS,
                "Execute a shell command." },
            { NULL, NULL, 0, NULL } /* Sentinel */
        };

        static PyModuleDef PyEnv_module = {
            PyModuleDef_HEAD_INIT,
            "Environment", /* name of module */
            "test", /* module documentation, may be NULL */
            -1, /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
            PyEnvironmentMethods
        };

        PyMODINIT_FUNC
        PyInit_Environment(void)
        {

            if (PyType_Ready(&PyTypedScopePtrType) < 0)
                return NULL;
            if (PyType_Ready(&PyOwnedScopePtrType) < 0)
                return NULL;
            if (PyType_Ready(&PyApiFunctionType) < 0)
                return NULL;
            if (PyType_Ready(&PyBoundApiFunctionType) < 0)
                return NULL;
            if (PyType_Ready(&PyScopeIteratorType) < 0)
                return NULL;
            if (PyType_Ready(&PyVirtualSequenceRangeType) < 0)
                return NULL;
            if (PyType_Ready(&PyVirtualAssociativeRangeType) < 0)
                return NULL;
            if (PyType_Ready(&PyVirtualSequenceIteratorType) < 0)
                return NULL;
            if (PyType_Ready(&PyVirtualAssociativeIteratorType) < 0)
                return NULL;
            if (PyType_Ready(&PyVector3Type) < 0)
                return NULL;
            if (PyType_Ready(&PyMatrix3Type) < 0)
                return NULL;
            if (PyType_Ready(&PyQuaternionType) < 0)
                return NULL;

            PyObject *m = PyModule_Create(&PyEnv_module);
            if (m == NULL)
                return NULL;

            Py_INCREF(&PyVector3Type);
            if (PyModule_AddObject(m, "Vector3", (PyObject *)&PyVector3Type) < 0) {
                Py_DECREF(&PyVector3Type);
                Py_DECREF(m);
                return NULL;
            }
            Py_INCREF(&PyMatrix3Type);
            if (PyModule_AddObject(m, "Matrix3", (PyObject *)&PyMatrix3Type) < 0) {
                Py_DECREF(&PyMatrix3Type);
                Py_DECREF(m);
                return NULL;
            }
            Py_INCREF(&PyQuaternionType);
            if (PyModule_AddObject(m, "Quaternion", (PyObject *)&PyQuaternionType) < 0) {
                Py_DECREF(&PyQuaternionType);
                Py_DECREF(m);
                return NULL;
            }

            return m;
        }

        static std::atomic<size_t> sInstances = 0;

        static Python3StreamRedirect sStream { std::cout.rdbuf() };

        Python3Environment::Python3Environment(App::Application &app)
            : VirtualScope(app)
        {
        }

        Python3Environment::~Python3Environment()
        {
        }

        Threading::Task<bool> Python3Environment::init()
        {
            if (sInstances++ == 0) {

                wchar_t *program = Py_DecodeLocale("Madgine-Python3-Env", NULL);

                /* Add a built-in module, before Py_Initialize */
                if (PyImport_AppendInittab("Environment", PyInit_Environment) == -1) {
                    LOG("Error: could not extend built-in modules table");
                    co_return false;
                }

                /* Pass argv[0] to the Python interpreter */
                Py_SetProgramName(program);

                Py_InitializeEx(0);

                PyRun_SimpleString("import Environment");
                sStream.redirect("stdout");
                sStream.redirect("stderr");

                Python3FileLoader::getSingleton().setup();

                PyEval_SaveThread();

                /*Python3FileLoader::load("dump");
                Python3FileLoader::load("signature");
                Python3FileLoader::load("testnode");

                lock(std::cout.rdbuf());
                PyRun_SimpleString("from dump import dump");
                unlock();*/
            }
            co_return true;
        }

        Threading::Task<void> Python3Environment::finalize()
        {
            if (--sInstances == 0) {
                lock(std::cout.rdbuf());

                sStream.reset("stdout");
                sStream.reset("stderr");

                auto result = Py_FinalizeEx();
                assert(result == 0);
            }
            co_return;
        }

        void Python3Environment::execute(std::string_view command)
        {
            PyRun_SimpleString(command.data());
        }

        PyGILState_STATE Python3Environment::lock(std::streambuf *buf)
        {
            //assert(PyGILState_Check() == 0);
            PyGILState_STATE handle = PyGILState_Ensure();
            assert(PyGILState_Check() == 1);
            if (buf)
                sStream.setBuf(buf);
            return handle;
        }

        std::streambuf *Python3Environment::unlock(PyGILState_STATE handle)
        {
            std::streambuf *result = sStream.buf();
            assert(PyGILState_Check() == 1);
            PyGILState_Release(handle);
            //assert(PyGILState_Check() == 0);
            return result;
        }

        size_t Python3Environment::totalRefCount()
        {
            PyObjectPtr refCount = PyObject_CallObject(PySys_GetObject((char *)"gettotalrefcount"), NULL);
            return PyLong_AsSsize_t(refCount);
        }

    }
}
}

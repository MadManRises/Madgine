#include "python3lib.h"

#include "python3env.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/root/keyvalueregistry.h"

#include "util/math/pymatrix3.h"
#include "util/math/pymatrix4.h"
#include "util/math/pyquaternion.h"
#include "util/math/pyvector3.h"
#include "util/math/pyvector4.h"
#include "util/pyapifunction.h"
#include "util/pyboundapifunction.h"
#include "util/pyobjectutil.h"
#include "util/pyownedscopeptr.h"
#include "util/pyscopeiterator.h"
#include "util/pytypedscopeptr.h"
#include "util/pyvirtualiterator.h"
#include "util/pyvirtualrange.h"

#include "python3fileloader.h"

#include "util/pydictptr.h"
#include "util/pyframeptr.h"
#include "util/pylistptr.h"

#include "python3streamredirect.h"

#include "util/python3lock.h"

#include "Generic/cowstring.h"

#include <frameobject.h>

UNIQUECOMPONENT(Engine::Scripting::Python3::Python3Environment)

METATABLE_BEGIN(Engine::Scripting::Python3::Python3Environment)
METATABLE_END(Engine::Scripting::Python3::Python3Environment)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        extern PyTypeObject PySuspendExceptionType;

        extern PyTypeObject PyDebugLocationType;

        extern PyTypeObject PyBehaviorScopeType;

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

        static PyObject *
        PyEnvironment_dir(PyObject *self, PyObject *args)
        {
            PyObject *list = PyList_New(KeyValueRegistry::globals().size() + KeyValueRegistry::workgroupLocals().size());
            size_t i = 0;
            for (std::string_view key : kvKeys(KeyValueRegistry::globals())) {
                PyList_SetItem(list, i++, toPyObject(key));
            }
            for (std::string_view key : kvKeys(KeyValueRegistry::workgroupLocals())) {
                PyList_SetItem(list, i++, toPyObject(key));
            }

            return list;
        }

        static PyMethodDef PyEnvironmentMethods[] = {
            { "__getattr__", PyEnvironment_get, METH_VARARGS,
                "Execute a shell command." },
            { "__dir__", PyEnvironment_dir, METH_NOARGS, "List all Environment globals" },
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

        static PyMethodDef PyEngineMethods[] = {
            { NULL, NULL, 0, NULL } /* Sentinel */
        };

        static PyModuleDef PyEngine_module = {
            PyModuleDef_HEAD_INIT,
            "Engine", /* name of module */
            "Wrappers for engine C++ classes", /* module documentation, may be NULL */
            -1, /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
            PyEngineMethods
        };

        PyMODINIT_FUNC PyInit_Engine(void)
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
            if (PyType_Ready(&PyVector4Type) < 0)
                return NULL;
            if (PyType_Ready(&PyMatrix3Type) < 0)
                return NULL;
            if (PyType_Ready(&PyMatrix4Type) < 0)
                return NULL;
            if (PyType_Ready(&PyQuaternionType) < 0)
                return NULL;
            if (PyType_Ready(&PySuspendExceptionType) < 0)
                return NULL;
            if (PyType_Ready(&PyDebugLocationType) < 0)
                return NULL;
            if (PyType_Ready(&PyBehaviorScopeType) < 0)
                return NULL;

            PyObject *m = PyModule_Create(&PyEngine_module);
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

        PyMODINIT_FUNC PyInit_Environment(void)
        {

            PyObject *m = PyModule_Create(&PyEnv_module);
            if (m == NULL)
                return NULL;

            return m;
        }

        static Python3StreamRedirect sStream;
        static std::stop_token sStopToken;

        Python3Environment::Python3Environment(Root::Root &root)
            : RootComponent(root)
        {
            wchar_t *program = Py_DecodeLocale("Madgine-Python3-Env", NULL);

            Py_SetProgramName(program);

            /* Add a built-in module, before Py_Initialize */
            if (PyImport_AppendInittab("Engine", PyInit_Engine) == -1) {
                LOG("Error: could not extend built-in modules table");
                mErrorCode = -1;
                return;
            }

            /* Add a built-in module, before Py_Initialize */
            if (PyImport_AppendInittab("Environment", PyInit_Environment) == -1) {
                LOG("Error: could not extend built-in modules table");
                mErrorCode = -1;
                return;
            }

            Py_InitializeEx(0);

            setupExecution();

            PyRun_SimpleString("import Environment");
            PyRun_SimpleString("import Engine");
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

        Python3Environment::~Python3Environment()
        {
            lock([](std::string_view) {}, Execution::unstoppable_token {});

            sStream.reset("stdout");
            sStream.reset("stderr");

            auto result = Py_FinalizeEx();
            assert(result == 0);
        }

        std::string_view Python3Environment::key() const
        {
            return "Python3Environment";
        }

        extern PyFrameObject *sFrame;

        ExecutionSender Python3Environment::execute(std::string_view command, Closure<void(std::string_view)> out)
        {
            Python3Lock lock;

            PyObjectPtr code = Py_CompileString(command.data(), "<eval>", Py_single_input);
            if (code) {
                PyModulePtr main { "__main__" };

                PyFramePtr frame = PyFrame_New(
                    PyThreadState_Get(),
                    reinterpret_cast<PyCodeObject *>(static_cast<PyObject *>(code)),
                    main.getDict(),
                    main.getDict());

                return {std::move(frame), std::move(out)};
            } else {
                fetchError();
            }

            throw 0;
        }

        PyGILState_STATE Python3Environment::lock()
        {
            //assert(PyGILState_Check() == 0);
            PyGILState_STATE handle = PyGILState_Ensure();
            assert(PyGILState_Check() == 1);
            return handle;
        }

        Closure<void(std::string_view)> Python3Environment::unlock(PyGILState_STATE handle)
        {
            Closure<void(std::string_view)> result = std::move(sStream.out());
            assert(PyGILState_Check() == 1);
            PyGILState_Release(handle);
            return result;
        }

        void Python3Environment::lock(Closure<void(std::string_view)> out, std::stop_token st)
        {
            //assert(PyGILState_Check() == 0);
            PyGILState_STATE handle = PyGILState_Ensure();
            assert(PyGILState_Check() == 1);
            assert(handle == PyGILState_UNLOCKED);
            sStream.setOut(std::move(out));
            sStopToken = std::move(st);
        }

        std::pair<Closure<void(std::string_view)>, std::stop_token> Python3Environment::unlock()
        {
            std::pair<Closure<void(std::string_view)>, std::stop_token> result { sStream.out(), std::move(sStopToken) };
            sStream.setOut({});
            assert(PyGILState_Check() == 1);
            PyGILState_Release(PyGILState_UNLOCKED);
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
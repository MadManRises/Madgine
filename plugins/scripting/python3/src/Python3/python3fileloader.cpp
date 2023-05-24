#include "python3lib.h"

#include "python3fileloader.h"

#include "python3env.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "util/python3lock.h"

#include "util/pymoduleptr.h"

#include "util/pyobjectutil.h"

#include "util/pydictptr.h"

#include "Meta/keyvalue/valuetype.h"

#include <iostream>

UNIQUECOMPONENT(Engine::Scripting::Python3::Python3FileLoader)

METATABLE_BEGIN(Engine::Scripting::Python3::Python3FileLoader)
MEMBER(mResources)
FUNCTION(find_spec, name, import_path, target_module)
FUNCTION(create_module, spec)
FUNCTION(exec_module, module)
METATABLE_END(Engine::Scripting::Python3::Python3FileLoader)

METATABLE_BEGIN_BASE(Engine::Scripting::Python3::Python3FileLoader::Resource, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Scripting::Python3::Python3FileLoader::Resource)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3FileLoader::Python3FileLoader()
            : ResourceLoader({ ".py" }, { .mAutoLoad = true, .mInplaceReload = true })
        {
        }

        void Python3FileLoader::setup()
        {
            auto result = PyList_Append(PyModulePtr { "sys" }.get("meta_path"), toPyObject(TypedScopePtr { this }));
            assert(result == 0);
        }

        void Python3FileLoader::cleanup()
        {
            mTables.clear();
        }

        Threading::Task<bool> Python3FileLoader::loadImpl(PyModulePtr &module, ResourceDataInfo &info, Filesystem::FileEventType event)
        {
            Python3Lock lock { nullptr };

            if (!module) {
                module = { info.resource()->name() };
            } else {
                PyModulePtr { "importlib" }.call("reload", "(O)", (PyObject *)module);
            }

            co_return true;
        }

        void Python3FileLoader::unloadImpl(PyModulePtr &module)
        {
            Python3Lock lock { nullptr };
            module.reset();
        }

        Python3FileLoader::Python3FunctionTable::Python3FunctionTable(PyObjectPtr fn)
            : mFunctionObject(fn)
        {

            PyObjectPtr signature = PyModulePtr { "inspect" }.get("signature").call("(O)", (PyObject *)fn);
            PyObjectPtr parameters = signature.get("parameters");

            PyObject *key = NULL, *value = NULL;
            Py_ssize_t pos = 0;

            PyObjectPtr iter = PyObject_GetIter(parameters);

            while (PyObjectPtr key = PyIter_Next(iter)) {
                PyObjectPtr parameter = PyObject_GetItem(parameters, key);
                PyObjectPtr type = parameter.get("annotation");

                PyObjectPtr ascii = PyUnicode_AsASCIIString(key);
                mArgumentsNames.emplace_back(PyBytes_AsString(ascii));
                mArgumentsHolder.push_back({ PyToValueTypeDesc(type), mArgumentsNames.back() });
            }
            mArguments = mArgumentsHolder.data();
            mArgumentsCount = mArgumentsHolder.size();

            mIsMemberFunction = false;

            mReturnType = PyToValueTypeDesc(signature.get("return_annotation"));
            PyObjectPtr name = fn.get("__name__");
            PyObjectPtr ascii_name = PyUnicode_AsASCIIString(name);
            mNameHolder = PyBytes_AsString(ascii_name);
            mName = mNameHolder;

            mFunctionPtr = [](const FunctionTable *self, KeyValueReceiver &receiver, const ArgumentList &args) {
                Python3InnerLock lock;
                Python3Environment::getSingleton().execute(receiver,
                    reinterpret_cast<PyCodeObject *>(PyFunction_GetCode(static_cast<const Python3FunctionTable *>(self)->mFunctionObject)));
            };

            registerFunction(*this);
        }

        Python3FileLoader::Python3FunctionTable::~Python3FunctionTable()
        {
            unregisterFunction(*this);
        }

        void Python3FileLoader::find_spec(ValueType &result, std::string_view name, std::optional<std::string_view> import_path, ObjectPtr target_module)
        {
            Resource *res = get(name, this);
            if (!res)
                return;
            Python3InnerLock lock;
            PyObjectPtr spec = PyModulePtr { "importlib.machinery" }.get("ModuleSpec").call({ { { "loader_state", toPyObject(TypedScopePtr { res }) } } }, "sO", res->name().data(), toPyObject(TypedScopePtr { this }));
            result = fromPyObject(spec);
        }

        void Python3FileLoader::create_module(ValueType &result, ObjectPtr spec)
        {
            Python3InnerLock lock;
            ValueType resourcePtr = fromPyObject(PyObjectPtr { toPyObject(spec) }.get("loader_state"));            
            Resource *res = resourcePtr.as<TypedScopePtr>().safe_cast<Resource>();
            Handle handle = create(res, Filesystem::FileEventType::FILE_CREATED, this);
            handle.info()->setPersistent(true);
            PyModulePtr &module = *getDataPtr(handle, this, false);
            assert(!module);
            module = PyModulePtr::create(res->name());
            result = fromPyObject(module);
        }

        void Python3FileLoader::exec_module(ValueType &result, ObjectPtr module)
        {
            Python3InnerLock lock;
            
            PyObjectPtr moduleObject { toPyObject(module) };
            ValueType resourcePtr = fromPyObject(moduleObject.get("__spec__").get("loader_state"));
            Resource *res = resourcePtr.as<TypedScopePtr>().safe_cast<Resource>();

            PyModulePtr importlib { "importlib.util" };

            PyObjectPtr spec = importlib.get("spec_from_file_location").call("ss", res->name().data(), res->path().c_str());

            result = fromPyObject(spec.get("loader").get("exec_module").call("(O)", (PyObject *)moduleObject));

            PyObject *dict = PyModule_GetDict(moduleObject);

            PyObject *key, *value = NULL;
            Py_ssize_t pos = 0;

            while (PyDict_Next(dict, &pos, &key, &value)) {
                if (PyFunction_Check(value)) {
                    Python3FunctionTable &table = mTables.emplace_back(PyObjectPtr::fromBorrowed(value));
                }
            }

        }
    }
}
}

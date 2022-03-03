#include "python3lib.h"

#include "python3fileloader.h"

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

METATABLE_BEGIN_BASE(Engine::Scripting::Python3::Python3FileLoader::ResourceType, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Scripting::Python3::Python3FileLoader::ResourceType)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3FileLoader::Python3FileLoader()
            : ResourceLoader({ ".py" }, { .mInplaceReload = true })
        {
        }

        void Python3FileLoader::setup()
        {
            auto result = PyList_Append(PyModulePtr { "sys" }.get("meta_path"), toPyObject(TypedScopePtr { this }));
            assert(result == 0);
        }

        bool Python3FileLoader::loadImpl(PyModulePtr &module, ResourceDataInfo &info, Filesystem::FileEventType event)
        {
            Python3Lock lock;

            if (!module) {
                module = { info.resource()->name() };
            } else {
                PyModulePtr { "importlib" }.call("reload", "(O)", (PyObject *)module);
            }
            return true;
        }

        void Python3FileLoader::unloadImpl(PyModulePtr &module)
        {
            Python3Lock lock;
            module.reset();
        }

        void Python3FileLoader::find_spec(ValueType &retVal, std::string_view name, std::optional<std::string_view> import_path, ObjectPtr target_module)
        {
            ResourceType *res = get(name, this);
            if (!res)
                return;
            Python3Lock lock;
            PyObjectPtr spec = PyModulePtr { "importlib.machinery" }.call("ModuleSpec", { { { "loader_state", toPyObject(TypedScopePtr { res }) } } }, "sO", res->name().data(), toPyObject(TypedScopePtr { this }));
            return fromPyObject(retVal, spec);
        }

        void Python3FileLoader::create_module(ValueType &retVal, ObjectPtr spec)
        {
            Python3Lock lock;
            ValueType resourcePtr;
            fromPyObject(resourcePtr, PyObjectPtr { toPyObject(spec) }.get("loader_state"));
            ResourceType *res = resourcePtr.as<TypedScopePtr>().safe_cast<ResourceType>();
            HandleType handle = create(res, Filesystem::FileEventType::FILE_CREATED, this);
            handle.info()->setPersistent(true);
            PyModulePtr &module = *getDataPtr(handle, this, false);
            assert(!module);
            module = PyModulePtr::create(res->name());
            return fromPyObject(retVal, module);
        }

        void Python3FileLoader::exec_module(ValueType &retVal, ObjectPtr module)
        {
            Python3Lock lock;
            ValueType resourcePtr;
            PyObjectPtr moduleObject { toPyObject(module) };
            fromPyObject(resourcePtr, moduleObject.get("__spec__").get("loader_state"));
            ResourceType *res = resourcePtr.as<TypedScopePtr>().safe_cast<ResourceType>();

            PyModulePtr importlib { "importlib.util" };

            PyObjectPtr spec = importlib.call("spec_from_file_location", "ss", res->name().data(), res->path().c_str());

            return fromPyObject(retVal, spec.get("loader").call("exec_module", "(O)", (PyObject *)moduleObject));            
        }
    }
}
}
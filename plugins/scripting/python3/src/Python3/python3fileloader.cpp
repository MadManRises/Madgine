#include "python3lib.h"

#include "python3fileloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "util/python3lock.h"

#include "util/pymoduleptr.h"

#include <iostream>

UNIQUECOMPONENT(Engine::Scripting::Python3::Python3FileLoader)

METATABLE_BEGIN(Engine::Scripting::Python3::Python3FileLoader)
MEMBER(mResources)
METATABLE_END(Engine::Scripting::Python3::Python3FileLoader)

METATABLE_BEGIN_BASE(Engine::Scripting::Python3::Python3FileLoader::ResourceType, Engine::Resources::ResourceBase)
READONLY_PROPERTY(Data, dataPtr)
METATABLE_END(Engine::Scripting::Python3::Python3FileLoader::ResourceType)

namespace Engine {
namespace Scripting {
    namespace Python3 {

        Python3FileLoader::Python3FileLoader()
            : ResourceLoader({ ".py" })
        {
        }

        bool Python3FileLoader::loadImpl(PyModulePtr &module, ResourceType *res)
        {
            assert(!module);

            Python3Lock lock { std::cout.rdbuf() };

            module = PyModulePtr { "imp" }.call("load_source", "ss", res->name().data(), res->path().c_str());

            return true;
        }

        void Python3FileLoader::unloadImpl(PyModulePtr &module, ResourceType *res)
        {
            Python3Lock lock { std::cout.rdbuf() };
            module.reset();
        }

    }
}
}
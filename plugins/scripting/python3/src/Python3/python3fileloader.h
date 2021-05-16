#pragma once

#include "Madgine/resources/resourceloader.h"

#include "util/pymoduleptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3FileLoader : Resources::ResourceLoader<Python3FileLoader, PyModulePtr, std::list<Placeholder<0>>> {

            using Base = Resources::ResourceLoader<Python3FileLoader, PyModulePtr, std::list<Placeholder<0>>>;

            struct MADGINE_PYTHON3_EXPORT HandleType : Base::HandleType {
                using Base::HandleType::HandleType;
                HandleType(Base::HandleType handle)
                    : Base::HandleType(handle)
                {
                }

            };

            Python3FileLoader();

            bool loadImpl(PyModulePtr &module, ResourceType *res);
            void unloadImpl(PyModulePtr &module, ResourceType *res);
        };

    }
}
}

RegisterType(Engine::Scripting::Python3::Python3FileLoader);
#pragma once

#include "Madgine/resources/resourceloader.h"

#include "util/pymoduleptr.h"

#include "Meta/keyvalue/objectptr.h"

namespace Engine {
namespace Scripting {
    namespace Python3 {

        struct MADGINE_PYTHON3_EXPORT Python3FileLoader : Resources::ResourceLoader<Python3FileLoader, PyModulePtr, std::list<Placeholder<0>>> {

            using Base = Resources::ResourceLoader<Python3FileLoader, PyModulePtr, std::list<Placeholder<0>>>;

            struct MADGINE_PYTHON3_EXPORT Handle : Base::Handle {
                using Base::Handle::Handle;
                Handle(Base::Handle handle)
                    : Base::Handle(handle)
                {
                }
            };

            Python3FileLoader();

            void setup();

            bool loadImpl(PyModulePtr &module, ResourceDataInfo &info, Filesystem::FileEventType event);
            void unloadImpl(PyModulePtr &module);

            void find_spec(ValueType &retVal, std::string_view name, std::optional<std::string_view> import_path, ObjectPtr target_module);

            void create_module(ValueType &retVal, ObjectPtr spec);
            void exec_module(ValueType &retVal, ObjectPtr module);

        };

    }
}
}

REGISTER_TYPE(Engine::Scripting::Python3::Python3FileLoader)
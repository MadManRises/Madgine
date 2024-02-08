#pragma once

#include "Generic/genericresult.h"

namespace Engine {
namespace Resources {
    template <typename, typename>
    struct Handle;
    template <typename>
    struct ResourceData;
}
namespace Audio {
    struct AudioLoader;
}
}

DLL_IMPORT_VARIABLE(const Engine::MetaTable, table, SINGLE_ARG(Engine::NoneOf<Engine::Void, Engine::ScopePtr, Engine::ValueType, Engine::Flags<Engine::GenericResultRepresentation>, Engine::Resources::Handle<struct Engine::Audio::AudioLoader, struct Engine::Resources::ResourceData<struct Engine::Audio::AudioLoader> *> >));

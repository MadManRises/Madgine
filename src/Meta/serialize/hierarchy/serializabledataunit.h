#pragma once

#include "serializetable_forward.h"
#include "Generic/linestruct.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SerializableDataUnit {
    };

#define SERIALIZABLEUNIT(_Self)                                 \
    template <typename Tag, size_t...>                           \
    friend struct ::Engine::LineStruct;                         \
    friend struct ::Engine::Serialize::SerializeTableCallbacks; \
    DERIVE_FRIEND(onActivate, ::Engine::Serialize::)            \
    using Self = _Self;

}
}
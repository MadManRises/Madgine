#pragma once

#include "serializetable_forward.h"

namespace Engine {
namespace Serialize {

    struct META_EXPORT SerializableDataUnit {
    };

    namespace __serialize_impl__ {
        template <typename>
        struct SerializeInstance;
    }

#define SERIALIZABLEUNIT(_Self)                                                      \
    friend struct ::Engine::Serialize::__serialize_impl__::SerializeInstance<_Self>; \
    friend struct ::Engine::Serialize::SerializeTableCallbacks;                      \
    DERIVE_FRIEND(onActivate, ::Engine::Serialize::)                                 \
    using Self = _Self

}
}
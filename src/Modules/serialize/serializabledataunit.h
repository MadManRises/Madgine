#pragma once

namespace Engine {
namespace Serialize {


	struct MODULES_EXPORT SerializableDataUnit {
    };

	
    template <typename>
    struct __SerializeInstance;

#define SERIALIZABLEUNIT(_Self)                                    \
    friend struct ::Engine::Serialize::__SerializeInstance<_Self>; \
    friend struct ::Engine::Serialize::SerializeTableCallbacks;    \
    DERIVE_FRIEND(onActivate, ::Engine::Serialize::)               \
    using Self = _Self


}
}
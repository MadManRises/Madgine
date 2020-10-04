#pragma once

#include "../serializableunitptr.h"

namespace Engine {

namespace Serialize {
    template <typename T>
    struct NoParentUnit : T {
		using decay_t = T;

        template <typename... Args>
        NoParentUnit(Args &&... args)
            :  T(std::forward<Args>(args)...)
        {
            SerializableUnitPtr { this, &serializeTable<T>() }.sync();
        }

        ~NoParentUnit()
        {
            SerializableUnitPtr { this, &serializeTable<T>() }.unsync();
        }
    };
}

}

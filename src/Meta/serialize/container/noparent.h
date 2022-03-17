#pragma once

namespace Engine {
namespace Serialize {

    template <typename T>
    struct NoParent : T {
        using decay_t = T;

        template <typename... Args>
        NoParent(Args &&...args)
            : T(std::forward<Args>(args)...)
        {
            if constexpr (std::derived_from<T, TopLevelUnitBase>)
                this->sync();
            else
                setActive(*static_cast<T *>(this), true, true);
        }

        ~NoParent()
        {
            if constexpr (std::derived_from<T, TopLevelUnitBase>)
                this->unsync();
            else
                setActive(*static_cast<T *>(this), false, true);
        }
    };
}
}

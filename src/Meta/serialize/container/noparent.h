#pragma once

#include "../unithelper.h"

namespace Engine {

namespace Serialize {
    template <typename T>
    struct NoParentUnit : T {
		using decay_t = T;

        template <typename... Args>
        NoParentUnit(Args &&... args)
            :  T(std::forward<Args>(args)...)
        {
            if constexpr (std::is_base_of_v<TopLevelUnitBase, T>)
                this->sync();
            else
                UnitHelper<T>::setItemActive(*this, true, true);
        }

        ~NoParentUnit()
        {
            if constexpr (std::is_base_of_v<TopLevelUnitBase, T>)
                this->unsync();
            else
                UnitHelper<T>::setItemActive(*this, false, true);
        }
    };
}

}

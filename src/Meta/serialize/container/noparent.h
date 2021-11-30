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
            if constexpr (std::derived_from<T, TopLevelUnitBase>)
                this->sync();
            else
                UnitHelper<T>::setItemActive(*this, true, true);
        }

        ~NoParentUnit()
        {
            if constexpr (std::derived_from<T, TopLevelUnitBase>)
                this->unsync();
            else
                UnitHelper<T>::setItemActive(*this, false, true);
        }
    };
}

}

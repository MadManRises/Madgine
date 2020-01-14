#pragma once

namespace Engine {

namespace Serialize {
    template <typename T>
    struct NoParentUnit : T {
		using decay_t = T;

        template <typename... Args>
        NoParentUnit(Args &&... args)
            :  T(std::forward<Args>(args)...)
        {
            this->sync();
        }

        ~NoParentUnit()
        {
            this->unsync();
        }
    };
}

}

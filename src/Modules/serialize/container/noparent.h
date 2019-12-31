#pragma once

namespace Engine {

namespace Serialize {
    template <class T>
    struct NoParentUnit : T {
		using decay_t = T;

        template <class... Args>
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

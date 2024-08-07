#pragma once

#include "flagstub.h"

namespace Engine {
namespace Execution {

    template <typename... _Ty>
    struct Flag : FlagStub<_Ty...> {
        using FlagStub<_Ty...>::FlagStub;

        void emplace(_Ty &&...args)
        {
            {
                std::lock_guard guard { this->mStack.mutex() };
                this->mValue.emplace(std::forward<_Ty>(args)...);
            }

            while (Connection<FlagStub<_Ty...>, _Ty...> *current = this->mStack.pop()){
                TupleUnpacker::invokeExpand(&Connection<FlagStub<_Ty...>, _Ty...>::set_value, current, *this->mValue);
            }
        }
    };
}
}

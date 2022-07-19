#pragma once

#include "uniquecomponentregistry.h"

namespace Engine {
namespace UniqueComponent {

    template <typename Registry, typename __Base, typename... _Ty>
    struct Selector {
        typedef typename Registry::Base Base;

        Selector(_Ty... arg, size_t index = 0)
            : mValue(Registry::sComponents().at(index)(arg...))
        {
        }

        Selector(const Selector &) = delete;
        void operator=(const Selector &) = delete;

        Base *operator->() const
        {
            return mValue.get();
        }

        operator Base *() const
        {
            return mValue.get();
        }

        Base *get() const
        {
            return mValue.get();
        }

    private:
        std::unique_ptr<Base> mValue;
    };

}
}
#pragma once

#include "uniquecomponentregistry.h"

#include "uniquecomponent.h"

#include "Generic/container/emplace.h"

namespace Engine {
namespace UniqueComponent {

    template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
    struct Container : C {

        typedef typename Registry::F F;
        typedef typename Registry::Base Base;

        Container(_Ty... arg)
        {
            size_t count = Registry::sComponents().size();
            mSortedComponents.reserve(count);
            if constexpr (InstanceOf<C, std::vector>) {
                this->reserve(count);
            }
            for (auto f : Registry::sComponents()) {
                auto p = f(arg...);
                mSortedComponents.push_back(p.get());
                Engine::emplace(static_cast<typename base_container<C>::type &>(*this), C::end(), std::move(p));
            }
        }

        Container(const Container &) = delete;
        void operator=(const Container &) = delete;

        template <typename T>
        T &get()
        {
            return static_cast<T &>(get(component_index<T>()));
        }

        Base &get(size_t i)
        {
            return *mSortedComponents[i];
        }

    private:
        std::vector<Base *> mSortedComponents;
    };
}

template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct underlying_container<UniqueComponent::Container<C, Registry, __dont_remove_Base, _Ty...>> {
    typedef C type;
};

}
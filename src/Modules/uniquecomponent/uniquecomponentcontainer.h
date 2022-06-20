#pragma once

#include "uniquecomponentregistry.h"

#include "uniquecomponent.h"

namespace Engine {
namespace UniqueComponent {

    template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
    struct Container : C {

        typedef C container;

        typedef typename Registry::F F;
        typedef typename Registry::Base Base;

        typedef typename container::value_type value_type;

        Container(_Ty... arg)
        {
            size_t count = Registry::sComponents().size();
            mSortedComponents.reserve(count);
            if constexpr (InstanceOf<container, std::vector>) {
                this->reserve(count);
            }
            for (auto f : Registry::sComponents()) {
                auto p = f(arg...);
                mSortedComponents.push_back(p.get());
                container_traits<container>::emplace(*this, container::end(), std::move(p));
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

template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct container_traits<UniqueComponent::Container<C, Registry, __dont_remove_Base, _Ty...>> : container_traits<C> {
    typedef typename UniqueComponent::Container<C, Registry, __dont_remove_Base, _Ty...>::iterator iterator;
    typedef typename UniqueComponent::Container<C, Registry, __dont_remove_Base, _Ty...>::const_iterator const_iterator;
    typedef typename UniqueComponent::Container<C, Registry, __dont_remove_Base, _Ty...>::reverse_iterator reverse_iterator;
    typedef typename UniqueComponent::Container<C, Registry, __dont_remove_Base, _Ty...>::const_reverse_iterator const_reverse_iterator;
};

}
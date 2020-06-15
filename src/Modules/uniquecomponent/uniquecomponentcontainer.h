#pragma once

#include "uniquecomponentregistry.h"

#include "uniquecomponent.h"

#include "../generic/replace.h"

namespace Engine {

template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct UniqueComponentContainer : replace<C>::template type<std::unique_ptr<typename Registry::Base>>
#if ENABLE_PLUGINS
    ,
                                      ComponentRegistryListener
#endif
{

    typedef typename replace<C>::template type<std::unique_ptr<typename Registry::Base>> container;

    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

    typedef typename container::value_type value_type;    

    typedef typename container_traits<container>::const_iterator const_iterator;

    UniqueComponentContainer(_Ty... arg)
#if ENABLE_PLUGINS
        : mArg(arg...)
#endif
    {
        size_t count = Registry::sComponents().size();
        mSortedComponents.reserve(count);
        if constexpr (is_instance_v<container, std::vector>) {
            this->reserve(count);
        }
        for (auto f : Registry::sComponents()) {
            value_type p = f(arg...);
            mSortedComponents.push_back(p.get());
            container_traits<container>::emplace(*this, container::end(), std::move(p));
        }
#if ENABLE_PLUGINS
        Registry::addListener({ this, &UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>::sUpdateComponents });
#endif
    }

    ~UniqueComponentContainer() {
#if ENABLE_PLUGINS
        Registry::removeListener(this);
#endif
    }

    UniqueComponentContainer(const UniqueComponentContainer &) = delete;
    void operator=(const UniqueComponentContainer &) = delete;

    const_iterator begin() const
    {
        return container::begin();
    }

    const_iterator end() const
    {
        return container::end();
    }

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

#if ENABLE_PLUGINS

protected:
    static void sUpdateComponents(ComponentRegistryListener *listener, CollectorInfoBase *info, bool add, const std::vector<F> &vals)
    {
        static_cast<UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...> *>(listener)->updateComponents(info, add, vals);
    }

    void updateComponents(CollectorInfoBase *info, bool add, const std::vector<F> &vals)
    {
        if (add) {
            assert(this->size() == info->mBaseIndex);
            mSortedComponents.reserve(info->mBaseIndex + vals.size());
            if constexpr (is_instance_v<container, std::vector>) {
                this->reserve(info->mBaseIndex + vals.size());
            }
            for (const F &f : vals) {
                value_type p = TupleUnpacker::invokeFromTuple(f, mArg);
                mSortedComponents.push_back(p.get());
                container_traits<container>::emplace(*this, container::end(), std::move(p));
            }
        } else {
            size_t from = info->mBaseIndex;
            size_t to = info->mBaseIndex + info->mElementInfos.size();
            for (size_t i = from; i != to; ++i) {
                this->erase(std::find_if(container::begin(), container::end(), [&](const value_type &p) { return p.get() == mSortedComponents[i]; }));
            }
            mSortedComponents.erase(mSortedComponents.begin() + from, mSortedComponents.begin() + to);
        }
    }

private:
    std::tuple<_Ty...> mArg;

#endif
};

template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct underlying_container<UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>> {
    typedef typename replace<C>::template type<std::unique_ptr<typename Registry::Base>> type;
};

}
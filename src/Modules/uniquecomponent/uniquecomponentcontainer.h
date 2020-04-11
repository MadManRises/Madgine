#pragma once

#include "uniquecomponentregistry.h"

#include "uniquecomponent.h"

namespace Engine {

template <typename C, typename Registry, typename... _Ty>
struct UniqueComponentContainerImpl : C
#if ENABLE_PLUGINS
    ,
                                      ComponentRegistryListener
#endif
{
    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

    typedef typename C::value_type value_type;

    typedef C Container;

    struct traits : container_traits<Container> {
    };

    typedef typename traits::const_iterator const_iterator;

    UniqueComponentContainerImpl(_Ty... arg)
#if ENABLE_PLUGINS
        : mArg(arg...)
#endif
    {
        size_t count = Registry::sComponents().size();
        mSortedComponents.reserve(count);
        if constexpr (is_instance_v<C, std::vector>) {
            this->reserve(count);
        }
        for (auto f : Registry::sComponents()) {
            value_type p = f(arg...);
            mSortedComponents.push_back(p.get());
            traits::emplace(*this, Container::end(), std::move(p));
        }
#if ENABLE_PLUGINS
        Registry::addListener({ this, &UniqueComponentContainerImpl<C, Registry, _Ty...>::sUpdateComponents });
#endif
    }

    ~UniqueComponentContainerImpl() {
#if ENABLE_PLUGINS
        Registry::removeListener(this);
#endif
    }

    UniqueComponentContainerImpl(const UniqueComponentContainerImpl &) = delete;
    void operator=(const UniqueComponentContainerImpl &) = delete;

    const_iterator begin() const
    {
        return Container::begin();
    }

    const_iterator end() const
    {
        return Container::end();
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
        static_cast<UniqueComponentContainerImpl<C, Registry, _Ty...> *>(listener)->updateComponents(info, add, vals);
    }

    void updateComponents(CollectorInfoBase *info, bool add, const std::vector<F> &vals)
    {
        if (add) {
            assert(this->size() == info->mBaseIndex);
            mSortedComponents.reserve(info->mBaseIndex + vals.size());
            if constexpr (is_instance_v<C, std::vector>) {
                this->reserve(info->mBaseIndex + vals.size());
            }
            for (const F &f : vals) {
                value_type p = TupleUnpacker::invokeFromTuple(f, mArg);
                mSortedComponents.push_back(p.get());
                traits::emplace(*this, Container::end(), std::move(p));
            }
        } else {
            size_t from = info->mBaseIndex;
            size_t to = info->mBaseIndex + info->mElementInfos.size();
            for (size_t i = from; i != to; ++i) {
                this->erase(std::find_if(Container::begin(), Container::end(), [&](const value_type &p) { return p.get() == mSortedComponents[i]; }));
            }
            mSortedComponents.erase(mSortedComponents.begin() + from, mSortedComponents.begin() + to);
        }
    }

private:
    std::tuple<_Ty...> mArg;

#endif
};

template <typename C, typename Registry, typename __Base, typename... _Ty>
using UniqueComponentContainer = UniqueComponentContainerImpl<typename replace<C>::template type<std::unique_ptr<typename Registry::Base>>, Registry, _Ty...>;

}
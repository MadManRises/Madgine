#pragma once

#include "uniquecomponentregistry.h"

#include "uniquecomponent.h"

#if ENABLE_PLUGINS
#    include "../generic/container/compoundatomicoperation.h"
#endif

namespace Engine {

template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct UniqueComponentContainer : C
#if ENABLE_PLUGINS
    ,
                                  ComponentRegistryListener
#endif
{

    typedef C container;

    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

    typedef typename container::value_type value_type;

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
            auto p = f(arg...);
            mSortedComponents.push_back(p.get());
            container_traits<container>::emplace(*this, container::end(), std::move(p));
        }
#if ENABLE_PLUGINS
        Registry::addListener({ this, &UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>::sUpdateComponents });
#endif
    }

    ~UniqueComponentContainer()
    {
#if ENABLE_PLUGINS
        Registry::removeListener(this);
#endif
    }

    UniqueComponentContainer(const UniqueComponentContainer &) = delete;
    void operator=(const UniqueComponentContainer &) = delete;

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
    static void sUpdateComponents(ComponentRegistryListener *listener, CollectorInfoBase *info, bool add, const std::vector<F> &vals, CompoundAtomicOperation &op)
    {
        static_cast<UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...> *>(listener)->updateComponents(info, add, vals, op);
    }

    void updateComponents(CollectorInfoBase *info, bool add, const std::vector<F> &vals, CompoundAtomicOperation &op)
    {
        if (add) {
            //assert(this->size() == info->mBaseIndex);
            mSortedComponents.reserve(info->mBaseIndex + vals.size());
            if constexpr (is_instance_v<container, std::vector>) {
                this->reserve(info->mBaseIndex + vals.size());
            }
            auto &insertOp = op.addMultiInsertOperation(*this);
            for (const F &f : vals) {
                auto p = TupleUnpacker::invokeFromTuple(f, mArg);
                mSortedComponents.push_back(p.get());
                container_traits<std::remove_reference_t<decltype(insertOp)>>::emplace(insertOp, container::end(), std::move(p));
            }
        } else {
            //TODO: Is MultiRemove necessary?
            size_t from = info->mBaseIndex;
            size_t to = info->mBaseIndex + info->mElementInfos.size();
            for (size_t i = from; i != to; ++i) {
                auto it = std::find_if(container::begin(), container::end(), [&](const value_type &p) { return p.get() == mSortedComponents[i]; });
                auto &&single_op = op.addRemoveOperation(*this, it);
                single_op.erase(it);
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
    typedef C type;
};

template <typename C, typename Registry, typename __dont_remove_Base, typename... _Ty>
struct container_traits<UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>> : container_traits<C> {
    typedef typename UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>::iterator iterator;
    typedef typename UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>::const_iterator const_iterator;
    typedef typename UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>::reverse_iterator reverse_iterator;
    typedef typename UniqueComponentContainer<C, Registry, __dont_remove_Base, _Ty...>::const_reverse_iterator const_reverse_iterator;
};

}
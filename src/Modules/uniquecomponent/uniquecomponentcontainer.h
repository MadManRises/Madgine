#pragma once

#include "uniquecomponentregistry.h"

#include "../generic/container_traits.h"

#include "../threading/slot.h"

#include "uniquecomponent.h"

#include "../keyvalue/typedscopeptr.h"

#include "../generic/templates.h"


namespace Engine {

template <typename C, class _Base, class... _Ty>
struct UniqueComponentContainerImpl : C {
    typedef UniqueComponentRegistry<_Base, _Ty...> Registry;
    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

	typedef typename C::value_type value_type;

    typedef C Container;

    struct traits : container_traits<Container> {
    };

    typedef typename traits::const_iterator const_iterator;

    UniqueComponentContainerImpl(_Ty... arg)
#if ENABLE_PLUGINS
        : mUpdateSlot(this)
        , mArg(arg...)
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
        Registry::update().connect(mUpdateSlot);
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

    struct typed_const_iterator {
        using It = typename std::vector<Base *>::const_iterator;
        using TypeIt = typename std::vector<const MetaTable *>::const_iterator;

        typed_const_iterator(It &&it, TypeIt &&type)
            : mIt(std::move(it))
            , mType(std::move(type))
        {
        }

        void operator++()
        {
            ++mIt;
            ++mType;
        }

        TypedScopePtr operator*() const
        {
            return {
                *mIt, *mType
            };
        }

        bool operator==(const typed_const_iterator &other) const
        {
            return mIt == other.mIt;
        }

        bool operator!=(const typed_const_iterator &other) const
        {
            return mIt != other.mIt;
        }

    private:
        It mIt;
        TypeIt mType;
    };

    typed_const_iterator typedBegin() const
    {
        return { mSortedComponents.begin(), Registry::sTables().begin() };
    }

    typed_const_iterator typedEnd() const
    {
        return { mSortedComponents.end(), Registry::sTables().end() };
    }

    template <class T>
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
    void updateComponents(CollectorInfo *info, bool add, const std::vector<F> &vals)
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
                this->erase(std::find_if(Container::begin(), Container::end(), [&](value_type &p) { return p.get() == mSortedComponents[i]; }));
            }
            mSortedComponents.erase(mSortedComponents.begin() + from, mSortedComponents.begin() + to);
        }
    }

private:
    //TODO Consider virtual calls instead
    Threading::Slot<&UniqueComponentContainerImpl<C, Base, _Ty...>::updateComponents> mUpdateSlot;
    std::tuple<_Ty...> mArg;

#endif
};


template <typename C, class _Base, class... _Ty>
using UniqueComponentContainer = UniqueComponentContainerImpl<typename replace<C>::template type<std::unique_ptr<_Base>>, _Base, _Ty...>;

}
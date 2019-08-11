#pragma once

#include "uniquecomponentregistry.h"

#include "../keyvalue/container_traits.h"

#include "../threading/slot.h"

#include "uniquecomponent.h"

#include "../keyvalue/typedscopeptr.h"

#include "../threading/defaulttaskqueue.h"

namespace Engine {

template <class _Base, template <class...> class C, class... _Ty>
class UniqueComponentContainer {
public:
    typedef UniqueComponentRegistry<_Base, _Ty...> Registry;
    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

    typedef C<std::unique_ptr<Base>> Container;

    typedef typename Container::const_iterator const_iterator;

    UniqueComponentContainer(_Ty... arg)
#if ENABLE_PLUGINS
        : mUpdateSlot(this)
        , mArg(arg...)
#endif
    {
        size_t count = Registry::sComponents().size();
        mSortedComponents.reserve(count);
        if constexpr (std::is_same_v<C<F>, std::vector<F>>) {
            mComponents.reserve(count);
        }
        for (auto f : Registry::sComponents()) {
            std::unique_ptr<Base> p = f(arg...);
            mSortedComponents.push_back(p.get());
            container_traits<C, std::unique_ptr<Base>>::emplace(mComponents, mComponents.end(), std::move(p));
        }
#if ENABLE_PLUGINS
        Registry::update().connect(mUpdateSlot);
#endif
    }

    UniqueComponentContainer(const UniqueComponentContainer &) = delete;
    void operator=(const UniqueComponentContainer &) = delete;

    const_iterator begin() const
    {
        return mComponents.begin();
    }

    const_iterator end() const
    {
        return mComponents.end();
    }

    struct typed_const_iterator {
        using It = typename std::vector<Base *>::const_iterator;
		using TypeIt = typename std::vector<const MetaTable*>::const_iterator;

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
            return
            {
                *mIt, *mType
            };
        }

        bool operator==(const typed_const_iterator &other) const
        {
            return mIt == other.mIt;
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

    size_t size()
    {
        return mComponents.size();
    }

    const Container &data() const
    {
        return mComponents;
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
    Container mComponents;
    std::vector<Base *> mSortedComponents;

#if ENABLE_PLUGINS

protected:
    void updateComponents(CollectorInfo *info, bool add, const std::vector<F> &vals)
    {
        if (add) {
            assert(mComponents.size() == info->mBaseIndex);
            mSortedComponents.reserve(info->mBaseIndex + vals.size());
            if constexpr (std::is_same_v<C<F>, std::vector<F>>) {
                mComponents.reserve(info->mBaseIndex + vals.size());
            }
            for (F f : vals) {
                std::unique_ptr<Base> p = TupleUnpacker::invokeFromTuple(f, mArg);
                mSortedComponents.push_back(p.get());
                container_traits<C, std::unique_ptr<Base>>::emplace(mComponents, mComponents.end(), std::move(p));
            }
        } else {
            size_t from = info->mBaseIndex;
            size_t to = info->mBaseIndex + info->mElementInfos.size();
            for (size_t i = from; i != to; ++i) {
                mComponents.erase(std::remove_if(mComponents.begin(), mComponents.end(), [&](std::unique_ptr<Base> &p) { return p.get() == mSortedComponents[i]; }));
            }
            mSortedComponents.erase(mSortedComponents.begin() + from, mSortedComponents.begin() + to);
        }
    }

private:
    //TODO Consider virtual calls instead
    Threading::Slot<&UniqueComponentContainer<Base, C, _Ty...>::updateComponents> mUpdateSlot;
    std::tuple<_Ty...> mArg;

#endif
};

}
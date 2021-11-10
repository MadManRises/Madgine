#pragma once

#include "uniquecomponentregistry.h"
#include "Generic/container/containerevent.h"

namespace Engine {

template <typename Observer, typename Registry, typename __Base, typename... _Ty>
struct UniqueComponentSelector : Observer
#if ENABLE_PLUGINS
    ,
                                 ComponentRegistryListener
#endif
{
    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

    UniqueComponentSelector(_Ty... arg, size_t index = 0)
        : mArg(arg...)
    {
        set(index);
#if ENABLE_PLUGINS
        Registry::addListener({ this, &UniqueComponentSelector<Observer, Registry, __Base, _Ty...>::sUpdateComponents });
#endif
    }

    ~UniqueComponentSelector()
    {
#if ENABLE_PLUGINS
        Registry::removeListener(this);
#endif
    }

    UniqueComponentSelector(const UniqueComponentSelector &)
        = delete;
    void operator=(const UniqueComponentSelector &) = delete;

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

    void set(IndexType<size_t> index)
    {
        if (index >= Registry::sComponents().size())
            index.reset();
        if (index != mIndex) {
            if (mIndex) {
                Observer::operator()(mValue.get(), BEFORE | ERASE);
                mValue.reset();
                Observer::operator()(mValue.get(), AFTER | ERASE);
            }
            mIndex = index;
            if (mIndex) {
                Observer::operator()(mValue.get(), BEFORE | EMPLACE);
                mValue = TupleUnpacker::invokeFromTuple(Registry::sComponents().at(index), mArg);
                Observer::operator()(mValue.get(), AFTER | EMPLACE);
            }
        }
    }

    void reset()
    {
        set(IndexType<size_t>::sInvalid);
    }

    std::unique_ptr<Base> release() {
        std::unique_ptr<Base> result = std::move(mValue);
        reset();
        return result;
    }

private:
    std::unique_ptr<Base> mValue;
    IndexType<size_t> mIndex;
    std::tuple<_Ty...> mArg;

#if ENABLE_PLUGINS

protected:
    static void sUpdateComponents(ComponentRegistryListener *listener, CollectorInfoBase *info, bool add, const std::vector<F> &vals, CompoundAtomicOperation &op)
    {
        static_cast<UniqueComponentSelector<Observer, Registry, __Base, _Ty...> *>(listener)->updateComponents(info, add, vals, op);
    }

    void updateComponents(CollectorInfoBase *info, bool add, const std::vector<F> &vals, CompoundAtomicOperation &op)
    {
        if (add) {
            if (!mIndex && vals.size() > 0) {
                set(info->mBaseIndex);
            }
        } else {
            if (mIndex) {
                size_t count = static_cast<typename Registry::CollectorInfo *>(info)->mComponents.size();
                if (mIndex >= info->mBaseIndex && mIndex < info->mBaseIndex + count) {
                    reset();
                } else if (mIndex >= info->mBaseIndex + count) {
                    mIndex -= count;
                }
            }
        }
    }

#endif
};

}
#pragma once

#include "uniquecomponentregistry.h"

namespace Engine {

template <typename Registry, typename __Base, typename... _Ty>
struct UniqueComponentSelector
#if ENABLE_PLUGINS
    : ComponentRegistryListener
#endif
{
    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

    static const constexpr size_t INVALID = std::numeric_limits<size_t>::max();

    UniqueComponentSelector(_Ty... arg, size_t index = 0)
        : mIndex(INVALID)
        , mArg(arg...)
    {
        set(index);
#if ENABLE_PLUGINS
        Registry::addListener({ this, &UniqueComponentSelector<Registry, __Base, _Ty...>::sUpdateComponents });
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

    void set(size_t index)
    {
        if (index >= Registry::sComponents().size())
            index = INVALID;
        if (index != mIndex) {
            if (index != INVALID) {
                mValue = TupleUnpacker::invokeFromTuple(Registry::sComponents().at(index), mArg);
            } else {
                mValue.reset();
            }
            mIndex = index;
        }
    }

    void reset()
    {
        set(INVALID);
    }

private:
    std::unique_ptr<Base> mValue;
    size_t mIndex;
    std::tuple<_Ty...> mArg;

#if ENABLE_PLUGINS

protected:
    static void sUpdateComponents(ComponentRegistryListener *listener, CollectorInfoBase *info, bool add, const std::vector<F> &vals)
    {
        static_cast<UniqueComponentSelector<Registry, __Base, _Ty...> *>(listener)->updateComponents(info, add, vals);
    }

    void updateComponents(CollectorInfoBase *info, bool add, const std::vector<F> &vals)
    {
        if (add) {
            if (mIndex == INVALID && vals.size() > 0) {
                mValue = TupleUnpacker::invokeFromTuple(vals[0], mArg);
                mIndex = info->mBaseIndex;
            }
        } else {
            if (mIndex != INVALID) {
                size_t count = static_cast<typename Registry::CollectorInfo *>(info)->mComponents.size();
                if (mIndex >= info->mBaseIndex && mIndex < info->mBaseIndex + count) {
                    mValue.reset();
                    mIndex = INVALID;
                } else if (mIndex >= info->mBaseIndex + count) {
                    mIndex -= count;
                }
            }
        }
    }

#endif
};

}
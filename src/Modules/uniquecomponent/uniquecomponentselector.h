#pragma once

#include "uniquecomponentregistry.h"

#include "../threading/slot.h"

namespace Engine {

template <class _Base, class... _Ty>
class UniqueComponentSelector {
public:
    typedef UniqueComponentRegistry<_Base, _Ty...> Registry;
    typedef typename Registry::F F;
    typedef typename Registry::Base Base;

    static const constexpr size_t INVALID = std::numeric_limits<size_t>::max();

    UniqueComponentSelector(_Ty... arg, size_t index = 0)
        : mIndex(index)
#if ENABLE_PLUGINS
        , mUpdateSlot(this)
        , mArg(arg...)
#endif
    {
        if (index != INVALID) {
            if (Registry::sComponents().size() > index)
                mValue = Registry::sComponents().at(index)(arg...);
            else
                mIndex = INVALID;
        }
#if ENABLE_PLUGINS
        Registry::update().connect(mUpdateSlot);
#endif
    }

    UniqueComponentSelector(const UniqueComponentSelector &) = delete;
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

    void assign(size_t index)
    {
        if (mIndex != index) {
            if (mIndex != INVALID) {
                mValue.reset();
            }
            mIndex = index;
            if (mIndex != INVALID) {
                if (Registry::sComponents().size() > index)
                    mValue = TupleUnpacker::invokeFromTuple(Registry::sComponents().at(index), mArg);
                else
                    mIndex = INVALID;
            }
        }
    }

    void reset()
    {
        assign(INVALID);
    }

private:
    std::unique_ptr<Base> mValue;
    size_t mIndex;

#if ENABLE_PLUGINS

protected:
    void updateComponents(CollectorInfo *info, bool add, const std::vector<F> &vals)
    {
        if (add) {
            if (mIndex == INVALID && vals.size() > 0) {
                mValue = TupleUnpacker::invokeFromTuple(vals[0], mArg);
                mIndex = info->mBaseIndex;
            }
        } else {
            if (mIndex != INVALID) {
                if (mIndex >= info->mBaseIndex && mIndex < info->mBaseIndex + info->mComponents.size()) {
                    mValue.reset();
                    mIndex = INVALID;
                } else if (mIndex >= info->mBaseIndex + info->mComponents.size()) {
                    mIndex -= info->mComponents.size();
                }
            }
        }
    }

private:
    Threading::Slot<&UniqueComponentSelector<Base, _Ty...>::updateComponents> mUpdateSlot;
    std::tuple<_Ty...> mArg;

#endif
};

}
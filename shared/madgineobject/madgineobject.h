#pragma once

#include "Modules/threading/task.h"

namespace Engine {

struct MadgineObjectState {

protected:
    ~MadgineObjectState()
    {
        if (mState && mState->valid()) {
            LOG_WARNING("Deleting still initialized Object: " << mTypeInfo->name());
        }
    }

    Threading::TaskFuture<bool> getState()
    {
        if (!mState)
            mState = std::make_shared<Threading::TaskPromiseSharedState<bool>>();
        return mState;
    }

protected:
    std::shared_ptr<Threading::TaskPromiseSharedState<bool>> mState;
    const std::type_info *mTypeInfo = nullptr;
};

template <typename T>
struct MadgineObject : MadgineObjectState {
    Threading::Task<bool> callInit()
    {
        assert(!mState || (!mState->is_ready() && mState->valid()));
        mTypeInfo = &typeid(static_cast<T &>(*this));
        LOG("Initializing: " << mTypeInfo->name() << "...");
        auto task = Threading::make_task(&T::init, static_cast<T *>(this));
        if (!mState)
            mState = std::make_shared<Threading::TaskPromiseSharedState<bool>>();
        task.get_future(mState);

        return task;
    }

    Threading::Task<void> callFinalize()
    {
        return [this]() -> Threading::Task<void> {
            assert(mState && mState->is_ready() && mState->get());
            mState.reset();

            if constexpr (InstanceOf<decltype(static_cast<T *>(this)->finalize()), Threading::Task>) {
                co_await static_cast<T *>(this)->finalize();
            } else {
                static_cast<T *>(this)->finalize();
            }
        }();
    }
};

}

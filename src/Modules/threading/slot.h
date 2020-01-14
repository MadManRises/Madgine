#pragma once

#include "connectionstore.h"

#include "../generic/onetimefunctor.h"

#include "../generic/callable_traits.h"

#include "task.h"

namespace Engine {
namespace Threading {
    struct MODULES_EXPORT SlotBase {
        SlotBase(TaskQueue *queue);

        void disconnectAll();

        ConnectionStore &connectionStore();

        TaskQueue &taskQueue() const;

        void queue(TaskHandle &&task, const std::vector<Threading::DataMutex *> &dependencies) const;

        template <typename F>
        void queue(F &&f) const
        {
            queue(std::forward<F>(f), mDataDependencies);
        }

    private:
        ConnectionStore mConnections;

        std::vector<Threading::DataMutex *> mDataDependencies;

        TaskQueue *mQueue;
    };

    template <auto f, typename R, typename T, typename... _Ty>
    struct SlotImpl : SlotBase {
        SlotImpl(T *item, TaskQueue *queue = nullptr)
            : SlotBase(queue)
            , mItem(item)
        {
        }

        SlotImpl(const SlotImpl<f, R, T, _Ty...> &) = delete;

        void operator()(_Ty... args) const
        {
            (mItem->*f)(std::forward<_Ty>(args)...);
        }

        void queue(_Ty... args) const
        {
            SlotBase::queue(oneTimeFunctor<T, _Ty...>(f, mItem, std::forward<_Ty>(args)...));
        }

    private:
        T *mItem;
    };

    template <auto f>
    using Slot = typename MemberFunctionCapture<SlotImpl, f>::type;

}
}

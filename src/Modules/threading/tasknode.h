#pragma once

#include "../signalslot/signal.h"
#include "../signalslot/slot.h"
#include "../generic/callable_traits.h"

namespace Engine 
{
	namespace Threading 
	{

		template <typename R, typename Y = void>
		struct TaskResult
		{
			TaskState mState;
			union 
			{
				R mResult;
				Y mYield;
			};
		};

		template <auto f, typename R, typename T, typename... Args>
		struct TaskNodeImpl
		{
			TaskNodeImpl(T *t, TaskQueue *queue = nullptr) :
				mT(t),
				mImpl(this, queue) {}

		private:
			SignalSlot::TaskState impl(Args&&... args)
			{
				R result = (mT->*f)(std::forward<Args>(args)...);
				mSignal.emit(result);
				return SUCCESS;
			}

			T *mT;

			SignalSlot::Slot<TaskNodeImpl<f, R, T, Args...>::impl> mImpl;
			SignalSlot::Signal<R> mSignal;
		};

		template <auto f, typename R, typename Y, typename T, typename... Args>
		struct TaskNodeImpl<f, TaskResult<R, Y>, T, Args..., std::optional<Y>>
		{
			TaskNodeImpl(T *t) :
				mT(t),
				mImpl(this) {}

		private:
			SignalSlot::TaskState impl(Args&&... args)
			{
				TaskResult<R, Y> result = (mT->*f)(std::forward<Args>(args)..., mYield);
				switch (result.mState)
				{
				case YIELD:
					mYield = result.mYield;
					break;
				case SUCCESS:
					mSignal.emit(result.mResult);
					break;
				}				
				
				return result.mState;
			}

			std::optional<Y> mYield;
			T *mT;

			SignalSlot::Slot<TaskNodeImpl<f, R, T, Args...>::impl> mImpl;
			SignalSlot::Signal<R> mSignal;
		};

		template <auto f>
		using TaskNode = MemberFunctionCapture<TaskNodeImpl, f>;

	}
}
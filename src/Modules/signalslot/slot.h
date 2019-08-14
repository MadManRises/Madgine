#pragma once

#include "connectionstore.h"

#include "../generic/onetimefunctor.h"

#include "../generic/callable_traits.h"

#include "taskqueue.h"

namespace Engine
{
	namespace SignalSlot
	{
		struct MODULES_EXPORT SlotBase
		{
			SlotBase(TaskQueue *queue);

			void disconnectAll();

			ConnectionStore& connectionStore();

			TaskQueue& taskQueue() const;

			template <typename F>
			void queue(F &&f) const
			{
				mQueue->queue(std::forward<F>(f), mDataDependencies);
			}

		private:
			ConnectionStore mConnections;

			std::vector<Threading::DataMutex*> mDataDependencies;

			TaskQueue* mQueue;
		};

		template <auto f, class R, class T, class... _Ty>
		class SlotImpl : public SlotBase
		{
		public:
			SlotImpl(T* item, TaskQueue *queue) :
				SlotBase(queue),
				mItem(item)
			{
			}

			SlotImpl(const SlotImpl<f, R, T, _Ty...>&) = delete;

			void operator()(_Ty ... args) const
			{
				(mItem ->* f)(std::forward<_Ty>(args)...);
			}

			void queue(_Ty ... args) const
			{
				SlotBase::queue(oneTimeFunctor<T, _Ty...>(f, mItem, std::forward<_Ty>(args)...));
			}
			
		private:
			T* mItem;
		};

		template <auto f>
		using Slot = typename MemberFunctionCapture<SlotImpl, f>::type;



	}
}

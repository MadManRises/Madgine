#pragma once

#include "connectionstore.h"

#include "../generic/onetimefunctor.h"

#include "../generic/callable_traits.h"

#include "taskqueue.h"

namespace Engine
{
	namespace SignalSlot
	{
		template <auto f, class R, class T, class... _Ty>
		class SlotImpl
		{
		public:
			SlotImpl(T* item, TaskQueue *queue = nullptr) :
				mQueue(queue ? queue : DefaultTaskQueue::getSingletonPtr()),
				mThread(std::this_thread::get_id()),
				mItem(item)
			{
			}

			SlotImpl(const SlotImpl<f, T, R, _Ty...>&) = delete;

			void operator()(_Ty ... args) const
			{
				assert(std::this_thread::get_id() == mThread);
				(mItem ->* f)(std::forward<_Ty>(args)...);
			}

			void queue(_Ty ... args) const
			{
				mQueue->queue(oneTimeFunctor<T, _Ty...>(f, mItem, std::forward<_Ty>(args)...));
			}

			void queue_direct(_Ty ... args) const
			{
				if (std::this_thread::get_id() == mThread)
				{
					(mItem ->* f)(std::forward<_Ty>(args)...);
				}
				else
				{
					queue(std::forward<_Ty>(args)...);
				}
			}

			void disconnectAll()
			{
				mConnections.clear();
			}

			ConnectionStore& connectionStore()
			{
				return mConnections;
			}

			TaskQueue& taskQueue() const
			{
				assert(mQueue);
				return *mQueue;
			}

		private:
			ConnectionStore mConnections;
			TaskQueue* mQueue;
			std::thread::id mThread;

			T* mItem;
		};

		template <auto f>
		using Slot = typename MemberFunctionCapture<SlotImpl, f>::type;

	}
}

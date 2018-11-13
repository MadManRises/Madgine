#pragma once

#include "connectionstore.h"

#include "../generic/onetimefunctor.h"

#include "../generic/callable_traits.h"

#include "connectionmanager.h"

namespace Engine
{
	namespace SignalSlot
	{
		template <auto f, class R, class T, class... _Ty>
		class SlotImpl
		{
		public:
			SlotImpl(T* item) :
				mManager(ConnectionManager::getSingletonPtr()),
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
				mManager->queue(oneTimeFunctor<T, _Ty...>(f, mItem, std::forward<_Ty>(args)...));
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

			std::thread::id thread() const
			{
				return mThread;
			}

			ConnectionManager& manager() const
			{
				assert(mManager);
				return *mManager;
			}

		private:
			ConnectionStore mConnections;
			ConnectionManager* mManager;
			std::thread::id mThread;

			T* mItem;
		};

		template <auto f>
		using Slot = typename MemberFunctionCapture<SlotImpl, f>::type;

	}
}

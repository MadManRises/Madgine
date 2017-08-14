#pragma once

#include "generic/templates.h"
#include "connectionstore.h"

namespace Engine {
	namespace SignalSlot {

		template <typename F, F f, class T, class R, class... _Ty>
		class SlotImpl{
		public:
			SlotImpl(T *item) :
				mManager(ConnectionManager::getSingletonPtr()),
				mThread(std::this_thread::get_id()),				
				mItem(item)
			{}

			SlotImpl(const SlotImpl<F, f, T, R, _Ty...> &) = delete;

			void operator()(_Ty... args) const {
				assert(std::this_thread::get_id() == mThread);
				(mItem->*f)(args...);
			}

			void queue(_Ty... args) const {
				mManager->queue(std::bind(f, mItem, args...));
			}

			void disconnectAll() {
				mConnections.clear();
			}

			ConnectionStore &connectionStore() {
				return mConnections;
			}

			std::thread::id thread() {
				return mThread;
			}

			ConnectionManager &manager() {
				assert(mManager);
				return *mManager;
			}

		private:
			ConnectionStore mConnections;
			ConnectionManager *mManager;
			std::thread::id mThread;

			T *mItem;
		};

		/*template <auto f>
		using Slot = MemberFunctionWrapper<SlotImpl, f>;*/

		template <typename F, F f>
		using Slot = typename MemberFunctionCapture<SlotImpl, F, f>::type;
	}
}
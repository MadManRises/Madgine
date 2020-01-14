#pragma once

#include "connection.h"


namespace Engine
{
	namespace Threading
	{
		extern MODULES_EXPORT std::mutex sSignalConnectMutex;

		template <class... _Ty>
		class SignalStub
		{
		public:
			SignalStub() = default;

			SignalStub(const SignalStub<_Ty...>& other)
			{
			}

			SignalStub(SignalStub<_Ty...>&&) noexcept
			{
			}

			~SignalStub() = default;

			SignalStub<_Ty...>& operator=(const SignalStub<_Ty...>& other) = delete;

			template <class T, class _ = std::enable_if_t<has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T& slot, DirectConnectionType = {})
			{
				std::weak_ptr<Connection<_Ty...>> conn = slot.connectionStore().template emplace_front<DirectConnection<T*, _Ty...>>(
					&slot);
				std::lock_guard<std::mutex> guard(sSignalConnectMutex);
				mConnectedSlots.emplace_back(conn);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T& slot, QueuedConnectionType)
			{
				std::weak_ptr<Connection<_Ty...>> conn = slot.connectionStore().template emplace_front<QueuedConnection<T*, _Ty...>>(
					&slot, slot.taskQueue());
				std::lock_guard<std::mutex> guard(sSignalConnectMutex);
				mConnectedSlots.emplace_back(conn);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<!has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T&& slot, DirectConnectionType = {})
			{
				std::weak_ptr<Connection<_Ty...>> conn = ConnectionStore::globalStore().emplace_front<DirectConnection<T, _Ty...>>(
					std::forward<T>(slot));
				std::lock_guard<std::mutex> guard(sSignalConnectMutex);
				mConnectedSlots.emplace_back(conn);
				return conn;
			}

			//TODO how to deal with DefaultTaskQueue in Threading
			/*template <class T, class _ = std::enable_if_t<!has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T&& slot, QueuedConnectionType)
			{
				std::weak_ptr<Connection<_Ty...>> conn = ConnectionStore::globalStore().emplace_front<QueuedConnection<T, _Ty...>>(
					std::forward<T>(slot), DefaultTaskQueue::getSingleton());
				std::lock_guard<std::mutex> guard(sSignalConnectMutex);
				mConnectedSlots.emplace_back(conn);
				return conn;
			}*/

		protected:

			void disconnectAll()
			{
				for (const std::weak_ptr<Connection<_Ty...>>& conn : mConnectedSlots)
				{
					if (std::shared_ptr<Connection<_Ty...>> ptr = conn.lock())
					{
						ptr->disconnect();
					}
				}
				mConnectedSlots.clear();
			}

			void copyConnections(const SignalStub<_Ty...>& other)
			{
				for (const std::weak_ptr<Connection<_Ty...>>& conn : other.mConnectedSlots)
				{
					if (std::shared_ptr<Connection<_Ty...>> ptr = conn.lock())
					{
						mConnectedSlots.emplace_back(ptr->clone());
					}
				}
			}

			std::vector<std::weak_ptr<Connection<_Ty...>>> mConnectedSlots;

		};

	}
}

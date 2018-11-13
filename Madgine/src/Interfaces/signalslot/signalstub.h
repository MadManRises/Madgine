#pragma once

#include "connection.h"


namespace Engine
{
	namespace SignalSlot
	{
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
				std::weak_ptr<Connection<_Ty...>> conn = slot.connectionStore().template emplace_front<
					DirectConnection<T*, _Ty...>>(&slot);
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T& slot, QueuedConnectionType)
			{
				std::weak_ptr<Connection<_Ty...>> conn = slot.connectionStore().template emplace_front<QueuedConnection<T*, _Ty...>>(
					&slot, slot.manager());
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<!has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T&& slot, DirectConnectionType = {})
			{
				std::weak_ptr<Connection<_Ty...>> conn = ConnectionStore::globalStore().emplace_front<DirectConnection<T, _Ty...>>(
					std::forward<T>(slot));
				mConnectedSlots.emplace_back(conn);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<!has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T&& slot, QueuedConnectionType)
			{
				std::weak_ptr<Connection<_Ty...>> conn = ConnectionStore::globalStore().emplace_front<QueuedConnection<T, _Ty...>>(
					std::forward<T>(slot), ConnectionManager::getSingleton());
				mConnectedSlots.emplace_back(conn);
				return conn;
			}

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

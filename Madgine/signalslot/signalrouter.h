#pragma once

#include "connection.h"

namespace Engine
{
	namespace SignalSlot
	{
		template <class... _Ty>
		class SignalRouter
		{
		public:
			SignalRouter() = default;

			SignalRouter(const SignalRouter<_Ty...>& other) = delete;

			SignalRouter(SignalRouter<_Ty...>&&) = default;

			~SignalRouter()
			{
				disconnectAll();
			}

			SignalRouter<_Ty...>& operator=(const SignalRouter<_Ty...>& other)
			{
				/*disconnectAll();
				copyConnections(other);*/
				return *this;
			}

			void operator()(_Ty ... args)
			{
				emit(args...);
			}

			void emit(_Ty ... args)
			{
				auto end = mConnectedSlots.end();
				auto it = mConnectedSlots.begin();
				if (end != it)
				{
					--end;
					bool keepGoing;
					do
					{
						keepGoing = it != end;
						if (std::shared_ptr<ConnectionInstance<_Ty...>> ptr = it->lock())
						{
							(*ptr)(args...);
							++it;
						}
						else
						{
							it = mConnectedSlots.erase(it);
						}
					}
					while (keepGoing);
				}
			}

			template <class T, class _ = std::enable_if_t<has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T& slot)
			{
				std::weak_ptr<ConnectionInstance<_Ty...>> conn = slot.connectionStore().template create<_Ty...>(&slot);
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<!has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(const T& slot)
			{
				std::weak_ptr<ConnectionInstance<_Ty...>> conn = ConnectionStore::globalStore().create<_Ty...>(slot);
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			void disconnectAll()
			{
				for (const std::weak_ptr<ConnectionInstance<_Ty...>>& conn : mConnectedSlots)
				{
					if (std::shared_ptr<ConnectionInstance<_Ty...>> ptr = conn.lock())
					{
						ptr->disconnect();
					}
				}
				mConnectedSlots.clear();
				mConnections.clear();
			}

			void copyConnections(const SignalRouter<_Ty...>& other)
			{
				for (const std::weak_ptr<ConnectionInstance<_Ty...>>& conn : other.mConnectedSlots)
				{
					if (std::shared_ptr<ConnectionInstance<_Ty...>> ptr = conn.lock())
					{
						mConnectedSlots.emplace_back(ptr->clone());
					}
				}
			}

			ConnectionStore& connectionStore()
			{
				return mConnections;
			}

		private:
			ConnectionStore mConnections;
			std::list<std::weak_ptr<ConnectionInstance<_Ty...>>> mConnectedSlots;
		};
	}
}

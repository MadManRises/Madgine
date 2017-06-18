#pragma once

#include "connection.h"

namespace Engine {
	namespace SignalSlot {


		template <class... _Ty>
		class Signal {
		public:
			Signal() = default;

			Signal(const Signal<_Ty...> &other) {
			}

			Signal(Signal<_Ty...> &&) = default;

			~Signal() {
				disconnectAll();
			}

			Signal<_Ty...> &operator=(const Signal<_Ty...> &other) = delete;

			void emit(_Ty... args) {
				auto end = mConnectedSlots.end();
				auto it = mConnectedSlots.begin();
				if (end != it) {
					--end;
					bool keepGoing;
					do{
						keepGoing = it != end;
						if (std::shared_ptr<ConnectionInstance<_Ty...>> ptr = it->lock()) {
							(*ptr)(*it, args...);						
							++it;
						}
						else {
							it = mConnectedSlots.erase(it);
						}
					} while (keepGoing);
				}
			}

			

			template <class T, class _ = std::enable_if_t<has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T &slot, DirectConnectionType = {}) {
				std::weak_ptr<ConnectionInstance<_Ty...>> conn = slot.connectionStore().template create<DirectConnection, _Ty...>(&slot);
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T &slot, QueuedConnectionType) {
				std::weak_ptr<ConnectionInstance<_Ty...>> conn = slot.connectionStore().template create<QueuedConnection, _Ty...>(&slot, slot.manager());
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<!has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T &&slot, DirectConnectionType = {}) {
				std::weak_ptr<ConnectionInstance<_Ty...>> conn = ConnectionStore::globalStore().template create<DirectConnection, _Ty...>(std::forward<T>(slot));
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			template <class T, class _ = std::enable_if_t<!has_store<T>::value>>
			std::weak_ptr<ConnectionBase> connect(T &&slot, QueuedConnectionType) {
				std::weak_ptr<ConnectionInstance<_Ty...>> conn = ConnectionStore::globalStore().template create<QueuedConnection, _Ty...>(std::forward<T>(slot), ConnectionManager::getSingleton());
				mConnectedSlots.emplace_back(
					conn
				);
				return conn;
			}

			void disconnectAll() {
				for (const std::weak_ptr<ConnectionInstance<_Ty...>> &conn : mConnectedSlots) {
					if (std::shared_ptr<ConnectionInstance<_Ty...>> ptr = conn.lock()) {
						ptr->disconnect();
					}
				}
				mConnectedSlots.clear();
			}

			void copyConnections(const Signal<_Ty...> &other) {
				for (const std::weak_ptr<ConnectionInstance<_Ty...>> &conn : other.mConnectedSlots) {
					if (std::shared_ptr<ConnectionInstance<_Ty...>> ptr = conn.lock()) {
						mConnectedSlots.emplace_back(ptr->clone());
					}
				}
			}

		private:
			std::list<std::weak_ptr<ConnectionInstance<_Ty...>>> mConnectedSlots;
		};

	}
}
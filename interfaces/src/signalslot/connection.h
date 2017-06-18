#pragma once

#include "templates.h"
#include "connectionstore.h"

namespace Engine {
	namespace SignalSlot {

		class DirectConnectionType {};
		class QueuedConnectionType {};
		static const constexpr QueuedConnectionType queuedConnection{};
		static const constexpr DirectConnectionType directConnection{};
		
		class INTERFACES_EXPORT ConnectionBase {
		public:
			virtual ~ConnectionBase() = default;
			ConnectionBase(ConnectionStore &store, const ConnectionStore::const_iterator &where);
			ConnectionBase(const ConnectionBase &other) = default;
			void disconnect();

		protected:
			const std::shared_ptr<ConnectionBase> &ptr();
			ConnectionStore &store();

		private:
			ConnectionStore::const_iterator mWhere;
			ConnectionStore &mStore;
		};

		


		template <class... _Ty>
		class Connection {
		public:
			template <class T, class = std::enable_if_t<!std::is_convertible<T, const Connection<_Ty...>>::value>>
			Connection(T &&impl)				
			{
				mImpl = [impl = std::forward<T>(impl)](_Ty... args) {
					TupleUnpacker<>::call(impl, std::forward_as_tuple(args...));
				};
			}

			template <class T>
			Connection(T *impl)
			{
				mImpl = [=](_Ty... args) {
					TupleUnpacker<>::call(*impl, std::forward_as_tuple(args...));
				};
			}

			Connection(const Connection<_Ty...> &) = default;

			void operator()(_Ty... args) const {
				mImpl(std::forward<_Ty>(args)...);
			}

		private:
			std::function<void(_Ty...)> mImpl;
		};

		template <class... _Ty>
		class ConnectionInstance : public ConnectionBase, public Connection<_Ty...> {
		public:
			template <class T>
			ConnectionInstance(T &&impl, ConnectionStore &store, const ConnectionStore::const_iterator &where) :
				ConnectionBase(store, where),
				Connection<_Ty...>(std::forward<T>(impl))
			{
			}

			virtual std::weak_ptr<ConnectionInstance<_Ty...>> clone() = 0;
			virtual void operator()(const std::weak_ptr<ConnectionInstance<_Ty...>> &ptr, _Ty... args) = 0;
		};
			
		template <class... _Ty>
		class DirectConnection : public ConnectionInstance<_Ty...>{
		public:
			template <class T>
			DirectConnection(ConnectionStore &store, const ConnectionStore::const_iterator &where, T &&impl) :
				ConnectionInstance<_Ty...>(std::forward<T>(impl), store, where)
			{

			}

			DirectConnection(const DirectConnection<_Ty...> &other, ConnectionStore &store, const ConnectionStore::const_iterator &where) :
				ConnectionInstance<_Ty...>(other, store, where)
			{

			}

			virtual std::weak_ptr<ConnectionInstance<_Ty...>> clone() override
			{
				return this->store().clone(*this);
			}

			virtual void operator()(const std::weak_ptr<ConnectionInstance<_Ty...>> &ptr, _Ty... args) override {
				Connection<_Ty...>::operator()(args...);
			}

		};

		template <class... _Ty>
		class QueuedConnection : public ConnectionInstance<_Ty...> {
		public:
			template <class T>
			QueuedConnection(ConnectionStore &store, const ConnectionStore::const_iterator &where, T &&impl, ConnectionManager &mgr) :
				ConnectionInstance<_Ty...>(std::forward<T>(impl), store, where),
				mManager(mgr)
			{

			}

			QueuedConnection(const QueuedConnection<_Ty...> &other, ConnectionStore &store, const ConnectionStore::const_iterator &where) :
				ConnectionInstance<_Ty...>(other, store, where),
				mManager(other.mManager)
			{

			}

			virtual std::weak_ptr<ConnectionInstance<_Ty...>> clone() override
			{
				return this->store().clone(*this);
			}

			virtual void operator()(const std::weak_ptr<ConnectionInstance<_Ty...>> &ptr, _Ty... args) override {
				if (mManager.thread() == std::this_thread::get_id()) {
					Connection<_Ty...>::operator()(args...);
				}
				else {
					mManager.queue([=]() {
						if (std::shared_ptr<ConnectionInstance<_Ty...>> innerPtr = ptr.lock()) {
							innerPtr->Connection<_Ty...>::operator()(args...);
						}
					});
				}
			}

		private:
			ConnectionManager &mManager;

		};


	}
}

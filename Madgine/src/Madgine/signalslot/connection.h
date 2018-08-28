#pragma once

#include "connectionstore.h"
#include "connectionmanager.h"

namespace Engine
{
	namespace SignalSlot
	{
		class DirectConnectionType
		{
		};

		class QueuedConnectionType
		{
		};

		static const constexpr QueuedConnectionType queuedConnection{};
		static const constexpr DirectConnectionType directConnection{};

		class INTERFACES_EXPORT ConnectionBase
		{
		public:
			virtual ~ConnectionBase() = default;
			ConnectionBase(std::shared_ptr<ConnectionBase> *prev);
			ConnectionBase(const ConnectionBase& other) = delete;
			void disconnect();

		protected:
			template <class Con, class... Args>
			std::weak_ptr<Con> cloneImpl(Args&&... args)
			{
				std::shared_ptr<Con> ptr = ConnectionStore::create<Con>(&mNext, std::forward<Args>(args)...);
				mNext = ptr;
				return ptr;
			}

		private:
			std::shared_ptr<ConnectionBase> mNext;
			std::shared_ptr<ConnectionBase> *mPrev;
		};


		template <class... _Ty>
		class Connection
		{
		public:
			template <class T, class = std::enable_if_t<!std::is_convertible<T, const Connection<_Ty...>>::value>>
			Connection(T&& impl)
			{
				mImpl = [impl = std::forward<T>(impl)](_Ty ... args)
				{
					TupleUnpacker::call(impl, std::forward_as_tuple(args...));
				};
			}

			template <class T>
			Connection(T* impl)
			{
				mImpl = [=](_Ty ... args)
				{
					TupleUnpacker::call(*impl, std::forward_as_tuple(args...));
				};
			}

			Connection(const Connection<_Ty...>&) = default;

			void operator()(_Ty ... args) const
			{
				mImpl(std::forward<_Ty>(args)...);
			}

		private:
			std::function<void(_Ty ...)> mImpl;
		};

		template <class... _Ty>
		class ConnectionInstance : public ConnectionBase, public Connection<_Ty...>
		{
		public:
			template <class T>
			ConnectionInstance(std::shared_ptr<ConnectionBase> *prev, T&& impl) :
				ConnectionBase(prev),
				Connection<_Ty...>(std::forward<T>(impl))
			{
			}

			virtual std::weak_ptr<ConnectionInstance<_Ty...>> clone() = 0;
			virtual void operator()(const std::weak_ptr<ConnectionInstance<_Ty...>>& ptr, _Ty ... args) = 0;
		};

		template <class... _Ty>
		class DirectConnection : public ConnectionInstance<_Ty...>
		{
		public:
			template <class T>
			DirectConnection(std::shared_ptr<ConnectionBase> *prev, T&& impl) :
				ConnectionInstance<_Ty...>(prev, std::forward<T>(impl))
			{
			}

			DirectConnection(std::shared_ptr<ConnectionBase> *prev, const DirectConnection<_Ty...>& other) :
				ConnectionInstance<_Ty...>(prev, other)
			{
			}

			std::weak_ptr<ConnectionInstance<_Ty...>> clone() override
			{
				return this->template cloneImpl<DirectConnection>(*this);
			}

			void operator()(const std::weak_ptr<ConnectionInstance<_Ty...>>& ptr, _Ty ... args) override
			{
				Connection<_Ty...>::operator()(args...);
			}

		
		};

		template <class... _Ty>
		class QueuedConnection : public ConnectionInstance<_Ty...>
		{
		public:
			template <class T>
			QueuedConnection(std::shared_ptr<ConnectionBase> *prev, T&& impl,
			                 ConnectionManager& mgr) :
				ConnectionInstance<_Ty...>(prev, std::forward<T>(impl)),
				mManager(mgr)
			{
			}

			QueuedConnection(std::shared_ptr<ConnectionBase> *prev, const QueuedConnection<_Ty...>& other) :
				ConnectionInstance<_Ty...>(prev, other),
				mManager(other.mManager)
			{
			}

			std::weak_ptr<ConnectionInstance<_Ty...>> clone() override
			{
				return this->template cloneImpl<QueuedConnection>(*this);
			}

			void operator()(const std::weak_ptr<ConnectionInstance<_Ty...>>& ptr, _Ty ... args) override
			{
				if (mManager.thread() == std::this_thread::get_id())
				{
					Connection<_Ty...>::operator()(args...);
				}
				else
				{
					mManager.queue([=]()
					{
						if (std::shared_ptr<ConnectionInstance<_Ty...>> innerPtr = ptr.lock())
						{
							innerPtr->Connection<_Ty...>::operator()(args...);
						}
					});
				}
			}

		private:
			ConnectionManager& mManager;
		};
	}
}

#pragma once

#include "connectionstore.h"
#include "connectionmanager.h"

namespace Engine
{
	namespace SignalSlot
	{
		struct DirectConnectionType	{};
		struct QueuedConnectionType	{};

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
		class Connection : public ConnectionBase
		{
		public:
			using ConnectionBase::ConnectionBase;			

			virtual void operator()(const std::weak_ptr<Connection<_Ty...>>& ptr, _Ty ... args) const = 0;
			virtual std::weak_ptr<Connection<_Ty...>> clone() = 0;

		};

		template <class T, class... _Ty>
		class ConnectionInstance : public Connection<_Ty...>
		{
		public:			
			ConnectionInstance(std::shared_ptr<ConnectionBase> *prev, T&& impl) :				
				Connection<_Ty...>(prev),
				mImpl(std::forward<T>(impl))
			{
			}

			ConnectionInstance(std::shared_ptr<ConnectionBase> *prev, const ConnectionInstance &other) :
				Connection<_Ty...>(prev),
				mImpl(other.mImpl)
			{
			}

			void operator()(_Ty ... args) const
			{
				TupleUnpacker::call(mImpl, std::forward_as_tuple(args...));
			}				

		private:
			std::remove_reference_t<T> mImpl;			
		};

		template <class T, class... _Ty>
		class ConnectionInstance<T*, _Ty...> : public Connection<_Ty...>
		{
		public:
			ConnectionInstance(std::shared_ptr<ConnectionBase> *prev, T *impl) :
				Connection<_Ty...>(prev),
				mImpl(impl)
			{
			}

			ConnectionInstance(std::shared_ptr<ConnectionBase> *prev, const ConnectionInstance &other) :
				Connection<_Ty...>(prev),
				mImpl(other.mImpl)
			{
			}

			void operator()(_Ty ... args) const
			{
				TupleUnpacker::call(*mImpl, std::forward_as_tuple(args...));
			}

		private:
			T *mImpl;
		};

		template <class T, class... _Ty>
		class DirectConnection : public ConnectionInstance<T, _Ty...>
		{
		public:
			DirectConnection(std::shared_ptr<ConnectionBase> *prev, T&& impl) :
				ConnectionInstance<T, _Ty...>(prev, std::forward<T>(impl))
			{
			}

			DirectConnection(std::shared_ptr<ConnectionBase> *prev, const DirectConnection<T, _Ty...>& other) :
				ConnectionInstance<T, _Ty...>(prev, other)
			{
			}

			std::weak_ptr<Connection<_Ty...>> clone() override
			{
				return this->template cloneImpl<DirectConnection>(*this);
			}

			void operator()(const std::weak_ptr<Connection<_Ty...>>& ptr, _Ty ... args) const override
			{
				ConnectionInstance<T, _Ty...>::operator()(args...);
			}

		
		};

		template <class T, class... _Ty>
		class QueuedConnection : public ConnectionInstance<T, _Ty...>
		{
		public:			
			QueuedConnection(std::shared_ptr<ConnectionBase> *prev, T&& impl,
			                 ConnectionManager& mgr) :
				ConnectionInstance<T, _Ty...>(prev, std::forward<T>(impl)),
				mManager(mgr)
			{
			}

			QueuedConnection(std::shared_ptr<ConnectionBase> *prev, const QueuedConnection<T, _Ty...>& other) :
				ConnectionInstance<T, _Ty...>(prev, other),
				mManager(other.mManager)
			{
			}

			std::weak_ptr<Connection<_Ty...>> clone() override
			{
				return this->template cloneImpl<QueuedConnection>(*this);
			}

			void operator()(const std::weak_ptr<Connection<_Ty...>>& ptr, _Ty ... args) override
			{
				if (mManager.thread() == std::this_thread::get_id())
				{
					ConnectionInstance<T, _Ty...>::operator()(args...);
				}
				else
				{
					mManager.queue([=]()
					{
						if (std::shared_ptr<ConnectionInstance<T, _Ty...>> innerPtr = ptr.lock())
						{
							static_cast<ConnectionInstance<T, _Ty...>*>(innerPtr.get())->operator()(args...);
						}
					});
				}
			}

		private:
			ConnectionManager& mManager;
		};
	}
}

#pragma once

#include "connectionstore.h"
#include "taskqueue.h"
#include "../generic/tupleunpacker.h"

namespace Engine
{
	namespace Threading
	{
		struct DirectConnectionType	{};
		struct QueuedConnectionType	{};

		static const constexpr QueuedConnectionType queuedConnection{};
		static const constexpr DirectConnectionType directConnection{};

		class MODULES_EXPORT ConnectionBase
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

			std::shared_ptr<ConnectionBase> mNext;
			std::shared_ptr<ConnectionBase> *mPrev;
		};


		template <class... _Ty>
		class Connection : public ConnectionBase
		{
		public:
			using ConnectionBase::ConnectionBase;			

			virtual void operator()(_Ty ... args) const = 0;
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
				TupleUnpacker::invoke(mImpl, std::forward<_Ty>(args)...);
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

			void operator()(_Ty ... args) const override
			{
				TupleUnpacker::invoke(*mImpl, std::forward<_Ty>(args)...);
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

			void operator()(_Ty ... args) const override final
			{
				ConnectionInstance<T, _Ty...>::operator()(args...);
			}
		
		};

		template <class T, class... _Ty>
		class QueuedConnection : public ConnectionInstance<T, _Ty...>
		{
		public:			
			QueuedConnection(std::shared_ptr<ConnectionBase> *prev, T&& impl,
			                 TaskQueue& queue) :
				ConnectionInstance<T, _Ty...>(prev, std::forward<T>(impl)),
				mQueue(queue)
			{
			}

			QueuedConnection(std::shared_ptr<ConnectionBase> *prev, const QueuedConnection<T, _Ty...>& other) :
				ConnectionInstance<T, _Ty...>(prev, other),
				mQueue(other.mQueue)
			{
			}

			std::weak_ptr<Connection<_Ty...>> clone() override
			{
				return this->template cloneImpl<QueuedConnection>(*this);
			}

			void operator()(_Ty ... args) const override final
			{
				std::weak_ptr<ConnectionInstance<T, _Ty...>> ptr = std::static_pointer_cast<ConnectionInstance<T, _Ty...>>(*this->mPrev);
				std::tuple<_Ty...> t = std::make_tuple(args...);
				mQueue.queue([=]()
				{
					if (std::shared_ptr<ConnectionInstance<T, _Ty...>> innerPtr = ptr.lock())
					{
						TupleUnpacker::invokeExpand([&](_Ty ... args) {innerPtr->ConnectionInstance<T, _Ty...>::operator()(args...); }, t);
					}
				});
			}

		private:
			TaskQueue& mQueue;
		};
	}
}

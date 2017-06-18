#pragma once

namespace Engine {
	namespace SignalSlot {



		class INTERFACES_EXPORT ConnectionStore {
		public:
			typedef typename std::list<std::shared_ptr<ConnectionBase>>::const_iterator const_iterator;

			template <template <class...> class Con, class... _Ty, class... Args>
			std::weak_ptr<ConnectionInstance<_Ty...>> create(Args&&... args) {
				mConnections.emplace_back();
				auto it = --mConnections.end();
				std::shared_ptr<ConnectionInstance<_Ty...>> conn =
					std::static_pointer_cast<ConnectionInstance<_Ty...>>(
						make_shared_connection(
							new Con<_Ty...>(*this, it, std::forward<Args>(args)...)
						)
					);
				*it = conn;
				return conn;
			}

			template <class T>
			std::weak_ptr<T> clone(const T &connection) {
				mConnections.emplace_back();
				auto it = --mConnections.end();
				std::shared_ptr<T> conn = std::static_pointer_cast<T>(make_shared_connection(new T(connection, *this, it)));
				*it = conn;
				return conn;
			}

			void disconnect(const const_iterator &where);
			void clear();

			static ConnectionStore &globalStore();

		private:
			static std::shared_ptr<ConnectionBase> make_shared_connection(ConnectionBase *conn);

		private:
			std::list<std::shared_ptr<ConnectionBase>> mConnections;
		};



		template <class T>
		class has_store {
		private:
			template<typename C> static std::is_same<decltype(&C::connectionStore), ConnectionStore &(C::*)()> Test(void*);
			template<typename> static std::false_type Test(...);

		public:
			static bool const constexpr value = decltype(Test<T>(0))::value;
		};		

		class INTERFACES_EXPORT ConnectionManager : public Singleton<ConnectionManager> {
		public:
			ConnectionManager();

			void queue(const std::function<void()> &f);

			void update();

			std::thread::id thread();

		private:		

			std::thread::id mThread;
			std::queue<std::function<void()>> mQueue;
			std::mutex mMutex;
		};

	}
}
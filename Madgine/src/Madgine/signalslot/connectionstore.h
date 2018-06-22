#pragma once

namespace Engine
{
	namespace SignalSlot
	{
		class INTERFACES_EXPORT ConnectionStore
		{
		public:
			typedef std::vector<std::shared_ptr<ConnectionBase>>::const_iterator const_iterator;

			template <template <class...> class Con, class... _Ty, class... Args>
			std::weak_ptr<ConnectionInstance<_Ty...>> create(Args&&... args)
			{
				mConnections.emplace_back();
				auto it = --mConnections.end();
				std::shared_ptr<ConnectionInstance<_Ty...>> conn =
					std::static_pointer_cast<ConnectionInstance<_Ty...>>(
						make_shared_connection(
							std::make_unique<Con<_Ty...>>(*this, it, std::forward<Args>(args)...)
						)
					);
				*it = conn;
				return conn;
			}

			template <class T>
			std::weak_ptr<T> clone(const T& connection)
			{
				mConnections.emplace_back();
				auto it = --mConnections.end();
				std::shared_ptr<T> conn = std::static_pointer_cast<T>(make_shared_connection(std::make_unique<T>(connection, *this, it)));
				*it = conn;
				return conn;
			}

			void disconnect(const const_iterator& where);
			void clear();

			static ConnectionStore& globalStore();

		private:
			static std::shared_ptr<ConnectionBase> make_shared_connection(std::unique_ptr<ConnectionBase> &&conn);

		private:
			std::vector<std::shared_ptr<ConnectionBase>> mConnections;
		};


		template <class T>
		class has_store
		{
		private:
			template <typename C>
			static std::is_same<decltype(&C::connectionStore), ConnectionStore &(C::*)()> test(void*);
			template <typename>
			static std::false_type test(...);

		public:
			static bool const constexpr value = decltype(test<T>(nullptr))::value;
		};


	}
}

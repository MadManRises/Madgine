#pragma once

#include "serializablecontainer.h"
#include "observablecontainer.h"
#include "creationhelper.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {


		template <class T>
		class BaseContainer<std::list<T>> : protected UnitHelper<T> {
		public:

			static constexpr const bool sorted = false;
		//protected:
			typedef std::list<typename UnitHelper<T>::Type> NativeContainerType;
			typedef typename NativeContainerType::iterator iterator;
			typedef typename NativeContainerType::const_iterator const_iterator;


			
			template <class... _Ty>
			iterator insert_where(const iterator &where, _Ty&&... args) {
				return mData.emplace(where, std::forward<_Ty>(args)...);
			}

			const_iterator erase(const const_iterator &it) {
				return mData.erase(it);
			}

		protected:
			NativeContainerType mData;

		};

		template <class T, class Creator>
		class SerializableListImpl : public SerializableContainer<std::list<T>, Creator> {
		public:
			using SerializableContainer<std::list<T>, Creator>::SerializableContainer;

			typedef typename SerializableContainer<std::list<T>, Creator>::iterator iterator;
			typedef typename SerializableContainer<std::list<T>, Creator>::const_iterator const_iterator;
		};

		template <class T, class Creator, const _ContainerPolicy &Config>
		class ObservableListImpl : public ObservableContainer<SerializableListImpl<T, Creator>, Config> {
		public:
			using ObservableContainer<SerializableListImpl<T, Creator>, Config>::ObservableContainer;

			typedef typename ObservableContainer<SerializableListImpl<T, Creator>, Config>::iterator iterator;
			typedef typename ObservableContainer<SerializableListImpl<T, Creator>, Config>::const_iterator const_iterator;

			template <class... _Ty>
			void emplace_back_safe(std::function<void(const iterator &)> callback, _Ty&&... args) {
				insert_where_safe(callback, this->end(), std::forward<_Ty>(args)...);
			}

		protected:
			
		};

		template <class C>
		class ListImpl : public C {

		public:

			typedef typename C::Type Type;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;

			using C::C;

			void remove(const Type &item) {
				for (const_iterator it = this->begin(); it != this->end();) {
					if (*it == item) {
						it = erase(it);
					}
					else {
						++it;
					}
				}

			}

			void push_back(const Type &item) {
				insert_where(this->end(), item);
			}

			template <class... _Ty>
			iterator emplace_back(_Ty&&... args) {
				return this->insert_where(this->end(), std::forward<_Ty>(args)...);
			}

			

			template <class... _Ty>
			iterator emplace_tuple_back(std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<>::call(this, &ListImpl::emplace_back<_Ty...>, std::forward<std::tuple<_Ty...>>(tuple));
			}

			size_t size() const {
				return this->mData.size();
			}

		

		};

		template <class T, class... Args>
		using SerializableList = ListImpl<SerializableListImpl<T, Creator<Args...>>>;

		template <class T, const _ContainerPolicy &Config, class... Args>
		using ObservableList = ListImpl<ObservableListImpl<T, Creator<Args...>, Config>>;


	}
}



#pragma once

#include "serializablecontainer.h"
#include "observablecontainer.h"
#include "creationhelper.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {


		template <class T>
		class BaseContainer<std::vector<T>> : protected UnitHelper<T> {
		public:
			static constexpr const bool sorted = false;
		protected:
			typedef std::vector<typename UnitHelper<T>::Type> NativeContainer;
			typedef typename NativeContainer::iterator iterator;
			typedef typename NativeContainer::const_iterator const_iterator;

			template <class... _Ty>
			iterator insert_where(const iterator &where, _Ty&&... args) {
				return mData.emplace(where, std::forward<_Ty>(args)...);
			}

			const_iterator erase(const const_iterator &it) {
				return mData.erase(it);
			}

		protected:
			NativeContainer mData;

		};

		template <class T, class Creator>
		class SerializableVectorImpl : public SerializableContainer<std::vector<T>, Creator> {
		public:
			using SerializableContainer<std::vector<T>,Creator>::SerializableContainer;

			typedef typename SerializableContainer<std::vector<T>, Creator>::iterator iterator;
			typedef typename SerializableContainer<std::vector<T>, Creator>::const_iterator const_iterator;

			void resize(size_t size) {
				this->mData.resize(size);
			}


		};

		template <class T, class Creator, _ContainerPolicy &Config>
		class ObservableVectorImpl : public ObservableContainer<SerializableVectorImpl<T, Creator>, Config> {
		public:
			using ObservableContainer<SerializableVectorImpl<T, Creator>, Config>::ObservableContainer;

			typedef typename ObservableContainer<SerializableVectorImpl<T, Creator>, Config>::iterator iterator;
			typedef typename ObservableContainer<SerializableVectorImpl<T, Creator>, Config>::const_iterator const_iterator;

			template <class... _Ty>
			void emplace_back_safe(std::function<void(const iterator &)> callback, _Ty&&... args) {
				insert_where_safe(callback, this->end(), std::forward<_Ty>(args)...);
			}

		protected:
			
		};

		template <class C>
		class VectorImpl : public C {

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
				return insert_where(this->end(), std::forward<_Ty>(args)...);
			}

			

			template <class... _Ty>
			iterator emplace_tuple_back(std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<>::call(this, &VectorImpl::emplace_back<_Ty...>, std::forward<std::tuple<_Ty...>>(tuple));
			}

			size_t size() const {
				return this->mData.size();
			}

			Type &at(size_t i) {
				return this->mData.at(i);
			}

			const Type &at(size_t i) const {
				return this->mData.at(i);
			}

		};

		template <class T, class... Args>
		using SerializableVector = VectorImpl<SerializableVectorImpl<T, Creator<Args...>>>;

		template <class T, const _ContainerPolicy &Config, class... Args>
		using ObservableVector = VectorImpl<ObservableVectorImpl<T, Creator<Args...>, Config>>;


	}
}



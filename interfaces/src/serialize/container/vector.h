#pragma once

#include "observablecontainer.h"
#include "creationhelper.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {


		template <class T>
		class BaseContainer<std::vector<T>> : protected UnitHelper<T> {
		public:
			static constexpr const bool sorted = false;
		//protected:
			typedef std::vector<typename UnitHelper<T>::Type> NativeContainerType;
			typedef typename NativeContainerType::iterator iterator;
			typedef typename NativeContainerType::const_iterator const_iterator;

			BaseContainer()
			{
			}

			BaseContainer(const NativeContainerType &c) :
				mData(c)
			{
			}

			template <class... _Ty>
			iterator insert(const const_iterator &where, _Ty&&... args) {
				iterator it = mData.emplace(where, std::forward<_Ty>(args)...);
				return it;
			}

			iterator erase(const const_iterator &it) {
				return mData.erase(it);
			}

		protected:
			NativeContainerType mData;

		};

		template <class T, class Creator>
		class SerializableVectorImpl : public SerializableContainer<std::vector<T>, Creator> {
		public:
			using SerializableContainer<std::vector<T>,Creator>::SerializableContainer;
			using SerializableContainer<std::vector<T>, Creator>::operator=;

			typedef typename SerializableContainer<std::vector<T>, Creator>::iterator iterator;
			typedef typename SerializableContainer<std::vector<T>, Creator>::const_iterator const_iterator;

			void resize(size_t size) {
				this->mData.resize(size);
			}


		};

		template <class T, class Creator, const _ContainerPolicy &Config>
		class ObservableVectorImpl : public ObservableContainer<SerializableVectorImpl<T, Creator>, Config> {
		public:
			using ObservableContainer<SerializableVectorImpl<T, Creator>, Config>::ObservableContainer;

			typedef typename ObservableContainer<SerializableVectorImpl<T, Creator>, Config>::iterator iterator;
			typedef typename ObservableContainer<SerializableVectorImpl<T, Creator>, Config>::const_iterator const_iterator;

			using ObservableContainer<SerializableVectorImpl<T, Creator>, Config>::operator=;

			template <class... _Ty>
			void emplace_back_safe(std::function<void(const iterator &)> callback, _Ty&&... args) {
				insert_safe(callback, this->end(), std::forward<_Ty>(args)...);
			}

		};

		template <class C>
		class VectorImpl : public C {

		public:

			typedef typename C::Type Type;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;

			using C::C;

			using C::operator=;

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
				insert(this->end(), item);
			}

			template <class... _Ty>
			iterator emplace_back(_Ty&&... args) {
				return insert(this->end(), std::forward<_Ty>(args)...);
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

			Type &operator[](size_t i) {
				return this->mData[i];
			}

			const Type &operator[](size_t i) const {
				return this->mData[i];
			}

		};

		template <class T, class Creator = DefaultCreator<>>
		using SerializableVector = VectorImpl<SerializableVectorImpl<T, Creator>>;

		template <class T, const _ContainerPolicy &Config, class Creator = DefaultCreator<>>
		using ObservableVector = VectorImpl<ObservableVectorImpl<T, Creator, Config>>;


	}
}



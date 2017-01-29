#pragma once

#include "basecontainer.h"
#include "observablecontainer.h"
#include "creationhelper.h"
#include "serializablecontainer.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {

		template <class T>
		class BaseContainer<std::map<std::string, T>> : protected UnitHelper<std::pair<const std::string, T>> {
		public:
			static constexpr const bool sorted = true;
		//protected:
			typedef std::map<std::string, typename UnitHelper<T>::Type> NativeContainerType;
			typedef typename NativeContainerType::iterator iterator;
			typedef typename NativeContainerType::const_iterator const_iterator;


			template <class K, class... _Ty>
			iterator insert_where(const iterator &where, K &&key, _Ty&&... args) {
				return mData.emplace_hint(where, std::piecewise_construct, std::forward_as_tuple(std::forward<K>(key)), std::forward_as_tuple(std::forward<_Ty>(args)...));
			}

			NativeContainerType mData;
		};


		template <class T, class Creator>
		class SerializableMapImpl : public SerializableContainer<std::map<std::string, T>, Creator> {
		public:
			using SerializableContainer<std::map<std::string, T>, Creator>::SerializableContainer;


			T &operator[](const std::string &key) {
				return this->mData[key];
			}
		};

		template <class T, class Creator, const _ContainerPolicy &Config>
		class ObservableMapImpl : public ObservableContainer<SerializableMapImpl<T, Creator>, Config> {
		public:
			using ObservableContainer<SerializableMapImpl<T, Creator>, Config>::ObservableContainer;

		};

		template <class C>
		class MapImpl : public C {
		public:

			using C::C;

			/*T &operator[](const std::string &key) {
				iterator it = mData.lower_bound(key);
				if (it == mData.end() || it->first != key) {
					iterator it = insert_where()
				}
				return mData[key];
			}*/

			bool contains(const std::string &key) const {
				return this->mData.find(key) != this->mData.end();
			}

			/*typename const C::Type::second_type &at(const std::string &key) const {
				return mData.at(key);
			}*/

			typename C::iterator find(const std::string &key) {
				return this->mData.find(key);
			}

			typename C::const_iterator find(const std::string &key) const {
				return this->mData.find(key);
			}

			template <class... _Ty>
			std::pair<typename C::iterator, bool> try_emplace(const std::string &key, _Ty&&... args) {
				auto it = this->mData.lower_bound(key);
				if (it != this->end() && it->first == key) {
					return{ it, false };
				}
				else {
					return{ this->insert_where(it, key, std::forward<_Ty>(args)...), true };
				}
			}

			size_t size() const {
				return this->mData.size();
			}

		};

		template <class T, class... Args>
		using SerializableMap = MapImpl<SerializableMapImpl<T, Creator<std::string, Args...>>>;

		template <class T, const _ContainerPolicy &Config, class... Args>
		using ObservableMap = MapImpl<ObservableMapImpl<T, Creator<std::string, Args...>, Config>>;


	}
}



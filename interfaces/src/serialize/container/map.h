#pragma once

#include "observablecontainer.h"
#include "creationhelper.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {

		template <class Key, class T>
		class BaseContainer<std::map<Key, T>> : protected UnitHelper<std::pair<const Key, T>> {
		public:
			static constexpr const bool sorted = true;
		//protected:
			typedef std::map<Key, typename UnitHelper<T>::Type> NativeContainerType;
			typedef typename NativeContainerType::iterator iterator;
			typedef typename NativeContainerType::const_iterator const_iterator;
			typedef Key KeyType;


			template <class K, class... _Ty>
			iterator insert(const const_iterator &where, K &&key, _Ty&&... args) {
				iterator it = mData.emplace_hint(where, std::piecewise_construct, std::forward_as_tuple(std::forward<K>(key)), std::forward_as_tuple(std::forward<_Ty>(args)...));
				this->postConstruct(*it);
				return it;
			}

			iterator erase(const const_iterator &where) {
				return mData.erase(where);
			}

			const KeyType &key(const const_iterator &where) const {
				return where->first;
			}

			bool contains(const Key &key) const {
				return mData.find(key) != mData.end();
			}


			iterator find(const Key &key) {
				return mData.find(key);
			}

			const_iterator find(const Key &key) const {
				return mData.find(key);
			}

			NativeContainerType mData;
		};


		template <class Key, class T, class Creator>
		class SerializableMapImpl : public SerializableContainer<std::map<Key, T>, Creator> {
		public:
			using SerializableContainer<std::map<Key, T>, Creator>::SerializableContainer;


			T &operator[](const Key &key) {
				return this->mData[key];
			}
		};

		template <class Key, class T, class Creator, const _ContainerPolicy &Config>
		class ObservableMapImpl : public ObservableContainer<SerializableMapImpl<Key, T, Creator>, Config> {
		public:
			using ObservableContainer<SerializableMapImpl<Key, T, Creator>, Config>::ObservableContainer;

		};

		template <class Key, class C>
		class MapImpl : public C {
		public:

			using C::C;

			/*T &operator[](const std::string &key) {
				iterator it = mData.lower_bound(key);
				if (it == mData.end() || it->first != key) {
					iterator it = insert()
				}
				return mData[key];
			}*/

			
			/*typename const C::Type::second_type &at(const std::string &key) const {
				return mData.at(key);
			}*/

		

			template <class... _Ty>
			std::pair<typename C::iterator, bool> try_emplace(const Key &key, _Ty&&... args) {
				auto it = this->mData.lower_bound(key);
				if (it != this->end() && it->first == key) {
					return{ it, false };
				}
				else {
					return{ this->insert(it, key, std::forward<_Ty>(args)...), true };
				}
			}

			size_t size() const {
				return this->mData.size();
			}

		};

		template <class Key, class T, class Creator = DefaultCreator<Key>>
		using SerializableMap = MapImpl<Key, SerializableMapImpl<Key, T, Creator>>;

		template <class Key, class T, const _ContainerPolicy &Config, class Creator = DefaultCreator<Key>>
		using ObservableMap = MapImpl<Key, ObservableMapImpl<Key, T, Creator, Config>>;


	}
}



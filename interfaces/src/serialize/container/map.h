#pragma once

#include "observablecontainer.h"
#include "creationhelper.h"
#include "unithelper.h"
#include "generic/container_traits.h"

namespace Engine {
	namespace Serialize {


		template <class Key, class C>
		class MapImpl : public C {
		public:

			using C::C;

			decltype(auto) operator[](const Key &key) {
				return this->mData[key];
			}

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
					return this->emplace(it, std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(args...));
				}
			}

		};


		template <class Key, class C>
		class ObservableMapImpl : public MapImpl<Key, C> {
		public:
			using MapImpl<Key, C>::MapImpl;

		};

		template <class Key, class T, class Creator = DefaultCreator<>, class KeyCreator = DefaultCreator<Key>>
		using SerializableMap = MapImpl<Key, SerializableContainer<container_traits<std::map, typename UnitHelper<std::pair<const Key, T>>::Type>, PairCreator<KeyCreator, Creator>>>;

		template <class Key, class T, const _ContainerPolicy &Config, class Creator = DefaultCreator<>, class KeyCreator = DefaultCreator<Key>>
		using ObservableMap = ObservableMapImpl<Key, ObservableContainer<container_traits<std::map, typename UnitHelper<std::pair<const Key, T>>::Type>, PairCreator<KeyCreator, Creator>, Config>>;




	}
}



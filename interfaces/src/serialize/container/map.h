#pragma once

#include "observablecontainer.h"
#include "creationhelper.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {

		template <class K, class T>
		struct container_traits<std::map, std::pair<const K, T>> {
			static constexpr const bool sorted = true;

			typedef std::map<K, T> container;
			typedef typename container::iterator iterator;
			typedef typename container::const_iterator const_iterator;
			typedef K key_type;
			typedef T value_type;
			typedef std::pair<const K, T> type;

			template <class... _Ty>
			static iterator insert(container &c, const const_iterator &where, _Ty&&... args) {
				return c.emplace_hint(where, std::forward<_Ty>(args)...);
			}

		};


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
					return{ this->insert(it, std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(args...)), true };
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



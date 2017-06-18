#pragma once

#include "apihelper.h"

namespace Engine {
	namespace Scripting {

		class KeyValueIterator {
		public:
			virtual ~KeyValueIterator() = default;
			virtual std::string key() = 0;
			virtual std::pair<bool, ValueType> value() = 0;
			virtual void operator++() = 0;
			virtual int push(lua_State *state) = 0;
			virtual bool ended() = 0;			
		};

		class KeyValueMapRef {
		public:
			virtual KeyValueIterator *iterator() = 0;
			virtual int resolve(lua_State *state, const std::string &key) = 0;
		};

		template <class T>
		struct Finder {
			static auto find(const T &c, const std::string &key) {
				return std::find_if(c.begin(), c.end(), [&](auto &v) {return kvKey(v) == key; });
			}
		};

		template <class V>
		struct Finder<std::map<std::string, V>> {
			static auto find(const std::map<std::string, V> &c, const std::string &key) {
				return c.find(key);
			}
		};

		template <class T>
		struct KeyValue {
			static const T &value(const T &v) {
				return v;
			}

			static const std::string &key(const T &v) {
				return v.key();
			}
		};

		template <class T>
		struct KeyValue<std::pair<const std::string, T>> {
			static const T &value(const std::pair<const std::string, T> &p) {
				return p.second;
			}

			static const std::string &key(const std::pair<const std::string, T> &p) {
				return p.first;
			}
		};

		template <class T>
		struct KeyValue<std::unique_ptr<T>> {
			static ScopeBase *value(const std::unique_ptr<T> &p) {
				return p.get();
			}

			static std::string key(const std::unique_ptr<T> &p) {
				return p->getName();
			}
		};

		template <class T>
		auto kvValue(const T &v) -> decltype(KeyValue<T>::value(v)) {
			return KeyValue<T>::value(v);
		}

		template <class T>
		auto kvKey(const T &v) -> decltype(KeyValue<T>::key(v)) {
			return KeyValue<T>::key(v);
		}

		template <class T, class _ = void_t<decltype(ValueType(std::declval<T>()))>>
		std::pair<bool, ValueType> toValueType(const T &v) {
			return { true, ValueType(v) };
		}

		std::pair<bool, ValueType> INTERFACES_EXPORT toValueType(...);

		template <class T>
		class KeyValueRef : public KeyValueMapRef {
		public:
			KeyValueRef(const T &map) :
				mMap(map)
			{

			}

			virtual KeyValueIterator *iterator() override {
				return new Iterator(mMap);
			}

			virtual int resolve(lua_State *state, const std::string &key) override {
				auto it = Finder<T>::find(mMap, key);
				if (it != mMap.end()) {
					return APIHelper::push(state, kvValue(*it));
				}
				return 0;
			}

		private:
			class Iterator : public KeyValueIterator {
			public:
				Iterator(const T &map) :
					mMap(map),
					mIt(map.begin())
				{
				}

				virtual std::string key() override {
					return kvKey(*mIt);
				}

				virtual std::pair<bool, ValueType> value() override {
					return toValueType(kvValue(*mIt));
				}

				virtual void operator++() override {
					++mIt;
				}

				virtual int push(lua_State *state) override {
					return APIHelper::push(state, kvValue(*mIt));
				}

				virtual bool ended() override {
					return mIt == mMap.end();
				}

			private:
				const T &mMap;
				decltype(std::declval<const T>().begin()) mIt;
			};

		private:
			const T &mMap;
		};

		class INTERFACES_EXPORT KeyValueMapList {
		public:
			//using vector::vector;
			template <class... Ty>
			KeyValueMapList(const Ty&... maps)
			{
				mMaps.reserve(sizeof...(Ty));
				using unpacker = bool[];
				unpacker{ (mMaps.emplace_back(new KeyValueRef<Ty>(maps)), true)... };
			}
			KeyValueMapList() = default;
			KeyValueMapList(const KeyValueMapList &) = delete;
			KeyValueMapList(KeyValueMapList &&) = default;

			int resolve(lua_State *state, const std::string &key);
			KeyValueIterator *iterator();

			KeyValueMapList &&merge(KeyValueMapList &&other) &&;

			template <class... Ty>
			KeyValueMapList &&merge(const Ty&... maps) && {
				return std::forward<KeyValueMapList>(*this).merge({ maps... });
			}

			std::vector<std::unique_ptr<KeyValueMapRef>>::const_iterator begin() const;
			std::vector<std::unique_ptr<KeyValueMapRef>>::const_iterator end() const;

			size_t size() const;

		private:
			std::vector<std::unique_ptr<KeyValueMapRef>> mMaps;
		};

		class KeyValueMapListIterator : public KeyValueIterator {
		public:
			KeyValueMapListIterator(const KeyValueMapList &list);
			virtual std::string key() override;
			virtual std::pair<bool, ValueType> value() override;
			virtual void operator++() override;
			virtual int push(lua_State *state) override;
			virtual bool ended() override;

		private:
			void validate();
		
		private:
			size_t mIndex;
			std::vector<std::unique_ptr<KeyValueIterator>> mIterators;
		};

	}
}

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
			virtual std::unique_ptr<KeyValueIterator> iterator() = 0;
			virtual int resolve(lua_State *state, ScopeBase *ref, const std::string &key) = 0;
			virtual bool contains(const std::string &key) = 0;
			virtual std::pair<bool, ValueType> at(const std::string &key) = 0;
		};


		template <class T, bool allowPair>
		struct KeyValue {
			template <class _T>
			static typename std::enable_if<!std::is_base_of<ScopeBase, _T>::value, const _T &>::type value(const _T &v) {
				return v;
			}

			template <class _T>
			static typename std::enable_if<std::is_base_of<ScopeBase, _T>::value, ScopeBase *>::type value(const _T &v) {
				return const_cast<_T*>(&v);
			}

			static const std::string &key(const T &v) {
				return v.key();
			}
		};

		template <class T>
		struct KeyValue<std::pair<const std::string, T>, true> {
			static auto value(const std::pair<const std::string, T> &p) {
				return KeyValue<T, false>::value(p.second);
			}

			static const std::string &key(const std::pair<const std::string, T> &p) {
				return p.first;
			}
		};

		template <class T, bool allowPair>
		struct KeyValue<std::unique_ptr<T>, allowPair> {
			static ScopeBase *value(const std::unique_ptr<T> &p) {
				return p.get();
			}

			static std::string key(const std::unique_ptr<T> &p) {
				return p->getName();
			}
		};

		template <class T>
		auto kvValue(const T &v) -> decltype(KeyValue<T,true>::value(v)) {
			return KeyValue<T,true>::value(v);
		}

		template <class T>
		auto kvKey(const T &v) -> decltype(KeyValue<T, true>::key(v)) {
			return KeyValue<T,true>::key(v);
		}

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

		template <class T, class _ = decltype(ValueType{ std::declval<T>() })>
		std::pair<bool, ValueType> toValueType(ScopeBase *ref, const T &v) {
			return { true, ValueType{v} };
		}

		std::pair<bool, ValueType> INTERFACES_EXPORT toValueType(ScopeBase *ref, const Mapper &mapper);

		template <class T>
		std::pair<bool, ValueType> toValueType(ScopeBase *ref, T &v) {
			return { false, ValueType{} };
		}

		template <class T>
		class KeyValueRef : public KeyValueMapRef {
		public:
			KeyValueRef(const T &map, ScopeBase *ref) :
				mMap(map),
				mRef(ref)
			{

			}

			virtual std::unique_ptr<KeyValueIterator> iterator() override {
				return std::unique_ptr<KeyValueIterator>(new Iterator(mMap, mRef));
			}

			virtual int resolve(lua_State *state, ScopeBase *ref, const std::string &key) override {
				auto it = Finder<T>::find(mMap, key);
				if (it != mMap.end()) {
					return APIHelper::push(state, ref, kvValue(*it));
				}
				return 0;
			}

			virtual bool contains(const std::string &key) override {
				auto it = Finder<T>::find(mMap, key);
				return it != mMap.end();
			}

			virtual std::pair<bool, ValueType> at(const std::string &key) override {
				auto it = Finder<T>::find(mMap, key);
				if (it == mMap.end())
					throw 0;
				return toValueType(mRef, kvValue(*it));
			}

		private:
			class Iterator : public KeyValueIterator {
			public:
				Iterator(const T &map, ScopeBase *ref) :
					mRef(ref),
					mMap(map),
					mIt(map.begin())
				{
				}

				virtual std::string key() override {
					return kvKey(*mIt);
				}

				virtual std::pair<bool, ValueType> value() override {
					return toValueType(mRef, kvValue(*mIt));
				}

				virtual void operator++() override {
					++mIt;
				}

				virtual int push(lua_State *state) override {
					return APIHelper::push(state, mRef, kvValue(*mIt));
				}

				virtual bool ended() override {
					return mIt == mMap.end();
				}

			private:
				const T &mMap;
				decltype(std::declval<const T>().begin()) mIt;
				ScopeBase *mRef;
			};

		private:
			const T &mMap;
			ScopeBase *mRef;
		};

		class INTERFACES_EXPORT KeyValueMapList {
		public:
			//using vector::vector;
			KeyValueMapList(ScopeBase *ref) :
				mRef(ref){
			}

			template <class... Ty>
			KeyValueMapList(ScopeBase *ref, const Ty&... maps) :
				mRef(ref)
			{
				mMaps.reserve(sizeof...(Ty));
				using unpacker = bool[];
				(void)unpacker{ (mMaps.emplace_back(new KeyValueRef<Ty>(maps, ref)), true)... };
			}
			KeyValueMapList() = default;
			KeyValueMapList(const KeyValueMapList &) = delete;
			KeyValueMapList(KeyValueMapList &&) = default;

			KeyValueMapList &operator=(const KeyValueMapList &) = delete;

			int resolve(lua_State *state, const std::string &key);
			std::unique_ptr<KeyValueIterator> iterator();

			KeyValueMapList merge(KeyValueMapList &&other) &&;

			template <class... Ty>
			KeyValueMapList merge(const Ty&... maps) && {
				return std::forward<KeyValueMapList>(*this).merge({ mRef, maps... });
			}

			std::vector<std::unique_ptr<KeyValueMapRef>>::const_iterator begin() const;
			std::vector<std::unique_ptr<KeyValueMapRef>>::const_iterator end() const;

			size_t size() const;

			bool contains(const std::string &key);
			std::pair<bool, Engine::ValueType> at(const std::string &key);

		private:
			std::vector<std::unique_ptr<KeyValueMapRef>> mMaps;
			ScopeBase *mRef;
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

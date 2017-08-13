#pragma once

#include "apihelper.h"

namespace Engine {
	namespace Scripting {

		struct Mapper;

		class KeyValueIterator {
		public:
			virtual ~KeyValueIterator() = default;
			virtual std::string key() = 0;
			virtual ValueType value() = 0;
			virtual void operator++() = 0;			
			virtual bool ended() = 0;		
		};

		class KeyValueRef {
		public:
			virtual std::shared_ptr<KeyValueIterator> iterator() = 0;
			virtual std::pair<bool, ValueType> get(const std::string &key) = 0;
			virtual bool contains(const std::string &key) = 0;
		};


		template <class T>
		struct KeyValue {
			static T & value(T &v) {
				return v;
			}

			static const std::string &key(const T &v) {
				return v.key();
			}
		};

		template <class T>
		struct KeyValue<T*> {			
			static T *value(T *v) {
				return v;
			}

			static const std::string &key(const T *v) {
				return v->key();
			}
		};

		template <class T>
		struct KeyValue<T* const> {
			static T *value(T *v) {
				return v;
			}

			static const std::string &key(const T *v) {
				return v->key();
			}
		};

		template <class T>
		struct KeyValue<std::pair<const std::string, T>> {
			static T& value(std::pair<const std::string, T> &p) {
				return p.second;
			}

			static const std::string &key(const std::pair<const std::string, T> &p) {
				return p.first;
			}
		};

		template <class T>
		struct KeyValue<const std::pair<const std::string, T>> {
			static const T& value(const std::pair<const std::string, T> &p) {
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
		struct KeyValue<const std::unique_ptr<T>> {
			static ScopeBase *value(const std::unique_ptr<T> &p) {
				return p.get();
			}

			static std::string key(const std::unique_ptr<T> &p) {
				return p->getName();
			}
		};

		template <class T>
		decltype(auto) kvValue(T &v) {
			return KeyValue<T>::value(v);
		}

		template <class T>
		decltype(auto) kvValue(const T &v) {
			return KeyValue<const T>::value(v);
		}

		template <class T>
		decltype(auto) kvKey(T &v) {
			return KeyValue<T>::key(v);
		}

		template <class T>
		decltype(auto) kvKey(const T &v) {
			return KeyValue<const T>::key(v);
		}

		template <class T>
		struct Finder {
			static auto find(T &c, const std::string &key) {
				return std::find_if(c.begin(), c.end(), [&](decltype(*c.begin()) v) {return kvKey(v) == key; });
			}
		};

		template <class V>
		struct Finder<std::map<std::string, V>> {
			static auto find(const std::map<std::string, V> &c, const std::string &key) {
				return c.find(key);
			}
		};

		template <class T, class _ = decltype(ValueType{ std::declval<T>() })>
		ValueType toValueType(ScopeBase *ref, const T &v) {
			return ValueType{v};
		}

		ValueType INTERFACES_EXPORT toValueType(ScopeBase *ref, const Mapper &mapper);

		template <class T, class _ = decltype(ValueType{ std::declval<T>().toValueType() })>
		ValueType toValueType(ScopeBase *ref, T &v) {
			return v.toValueType();
		}

		struct ToPointerConverter {
			template <class T>
			T *operator()(T &t) {
				return &t;
			}
		};

		template <class T, typename Converter>
		class TransformItContainer {
		private:
			TransformItContainer() = delete;
			TransformItContainer(const TransformItContainer<T, Converter> &) = delete;
			TransformItContainer(TransformItContainer<T, Converter> &&) = delete;
		
			template <class It>
			class TransformIterator {
			public:
				TransformIterator(It &&it) :
					mIt(std::forward<It>(it)) {}

				void operator++() {
					++mIt;
				}

				decltype(auto) operator*() const {
					return Converter{}(*mIt);
				}

				bool operator!=(const TransformIterator<It> &other) const {
					return mIt != other.mIt;
				}

				bool operator==(const TransformIterator<It> &other) const {
					return mIt == other.mIt;
				}


			private:
				It mIt;

			public:

				typedef typename It::difference_type difference_type;
				typedef typename std::remove_reference<decltype(*std::declval<TransformIterator<It>>())>::type value_type;
				typedef value_type* pointer;
				typedef decltype(*std::declval<TransformIterator<It>>()) reference;
				typedef typename It::iterator_category iterator_category;
			};

		public:			
			using iterator = TransformIterator<typename T::iterator>;
			using const_iterator = TransformIterator<typename T::const_iterator>;

			iterator begin() {
				return mList.begin();
			}

			const_iterator begin() const {
				return mList.begin();
			}

			iterator end() {
				return mList.end();
			}

			const_iterator end() const {
				return mList.end();
			}

		private:
			T mList;
		};

		template <typename Converter, class T>
		const TransformItContainer<T, Converter> &transformIt(const T &t) {
			return reinterpret_cast<const TransformItContainer<T, Converter>&>(t);
		}

		template <typename Converter, class T>
		TransformItContainer<T, Converter> &transformIt(T &t) {
			return reinterpret_cast<TransformItContainer<T, Converter>&>(t);
		}

		template <class T>
		class KeyValueMapRef : public KeyValueRef {
		public:
			KeyValueMapRef(T &map, ScopeBase *ref) :
				mMap(map),
				mRef(ref)
			{

			}

			virtual std::shared_ptr<KeyValueIterator> iterator() override {
				return std::shared_ptr<KeyValueIterator>(new Iterator(mMap, mRef));
			}

			virtual std::pair<bool, ValueType> get(const std::string &key) override {
				auto it = Finder<T>::find(mMap, key);
				if (it != mMap.end()) {
					return { true, toValueType(mRef, kvValue(*it)) };
				}
				return { false, ValueType{} };
			}

			virtual bool contains(const std::string &key) override {
				auto it = Finder<T>::find(mMap, key);
				return it != mMap.end();
			}

		private:
			class Iterator : public KeyValueIterator {
			public:
				Iterator(T &map, ScopeBase *ref) :
					mMap(map),
					mIt(map.begin()),
					mRef(ref)
				{
				}

				virtual std::string key() override {
					return kvKey(*mIt);
				}

				virtual ValueType value() override {
					return toValueType(mRef, kvValue(*mIt));
				}

				virtual void operator++() override {
					++mIt;
				}

				virtual bool ended() override {
					return mIt == mMap.end();
				}

			private:
				T &mMap;
				decltype(std::declval<T>().begin()) mIt;
				ScopeBase *mRef;
			};

		private:
			T &mMap;
			ScopeBase *mRef;
		};

		template <class T>
		class KeyValueItemRef : public KeyValueRef {
		public:
			KeyValueItemRef(T &item, ScopeBase *ref) :
				mItem(item),
				mRef(ref)
			{

			}

			virtual std::shared_ptr<KeyValueIterator> iterator() override {
				return std::shared_ptr<KeyValueIterator>(new Iterator(mItem, mRef));
			}

			virtual std::pair<bool, ValueType> get(const std::string &key) override {
				if (kvKey(mItem) == key) {
					return { true, toValueType(mRef, kvValue(mItem)) };
				}
				return { false, ValueType{} };
			}

			virtual bool contains(const std::string &key) override {
				return kvKey(mItem) == key;
			}

		private:
			class Iterator : public KeyValueIterator {
			public:
				Iterator(T &item, ScopeBase *ref) :
					mRef(ref),
					mItem(item),
					mEnded(false)
				{
				}

				virtual std::string key() override {
					return kvKey(mItem);
				}

				virtual ValueType value() override {
					return toValueType(mRef, kvValue(mItem));
				}

				virtual void operator++() override {
					mEnded = true;
				}

				virtual bool ended() override {
					return mEnded;
				}

			private:
				T &mItem;
				bool mEnded;
				ScopeBase *mRef;
			};

		private:
			T &mItem;
			ScopeBase *mRef;
		};

		template <class T>
		typename std::enable_if<is_iterable<T>::value, KeyValueMapRef<T>*>::type make_ref(ScopeBase *ref, T &t) {
			return new KeyValueMapRef<T>(t, ref);
		}

		template <class T>
		typename std::enable_if<!is_iterable<T>::value, KeyValueItemRef<T>*>::type make_ref(ScopeBase *ref, T &t) {
			return new KeyValueItemRef<T>(t, ref);
		}

		class INTERFACES_EXPORT KeyValueMapList {
		public:
			//using vector::vector;
			KeyValueMapList(ScopeBase *ref) :
				mRef(ref){
			}

			template <class... Ty>
			KeyValueMapList(ScopeBase *ref, Ty&... maps) :
				mRef(ref)
			{
				mRefs.reserve(sizeof...(Ty));
				using unpacker = bool[];
				(void)unpacker{ (mRefs.emplace_back(make_ref(ref, maps)), true)... };
			}
			KeyValueMapList() = delete;
			KeyValueMapList(const KeyValueMapList &) = delete;
			KeyValueMapList(KeyValueMapList &&) = default;

			KeyValueMapList &operator=(const KeyValueMapList &) = delete;

			std::pair<bool, ValueType> get(const std::string &key);
			std::unique_ptr<KeyValueIterator> iterator();

			KeyValueMapList merge(KeyValueMapList &&other) &&;

			template <class... Ty>
			KeyValueMapList merge(Ty&... maps) && {
				return std::forward<KeyValueMapList>(*this).merge({ mRef, maps... });
			}

			std::vector<std::unique_ptr<KeyValueRef>>::const_iterator begin() const;
			std::vector<std::unique_ptr<KeyValueRef>>::const_iterator end() const;

			size_t size() const;

			bool contains(const std::string &key);

		private:
			std::vector<std::unique_ptr<KeyValueRef>> mRefs;
			ScopeBase *mRef;
		};

		class KeyValueMapListIterator : public KeyValueIterator {
		public:
			KeyValueMapListIterator(const KeyValueMapList &list);
			virtual std::string key() override;
			virtual ValueType value() override;
			virtual void operator++() override;
			virtual bool ended() override;

		private:
			void validate();
		
		private:
			size_t mIndex;
			std::vector<std::shared_ptr<KeyValueIterator>> mIterators;
		};

	}
}
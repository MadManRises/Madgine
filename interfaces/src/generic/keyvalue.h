#pragma once

#include "valuetype.h"

namespace Engine {

	enum KeyValueValueFlags {
		NoFlags = 0x0,
		IsEditable = 0x1
	};

	class KeyValueIterator {
	public:
		virtual ~KeyValueIterator() = default;
		virtual std::string key() = 0;
		virtual ValueType value() = 0;
		virtual void operator++() = 0;
		virtual bool ended() = 0;
		virtual KeyValueValueFlags flags() = 0;
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

		static auto key(const T &v) -> decltype(v.key()) {
			return v.key();
		}
	};

	template <class T>
	struct KeyValue<T*> {
		static T *value(T *v) {
			return v;
		}

		static decltype(auto) key(const T *v) {
			return v->key();
		}
	};

	template <class T>
	struct KeyValue<T* const> {
		static T *value(T *v) {
			return v;
		}

		static decltype(auto) key(const T *v) {
			return v->key();
		}
	};

	template <class K, class T>
	struct KeyValue<std::pair<const K, T>> {
		static T& value(std::pair<const K, T> &p) {
			return p.second;
		}

		static const K &key(const std::pair<const K, T> &p) {
			return p.first;
		}
	};

	template <class K, class T>
	struct KeyValue<const std::pair<const K, T>> {
		static const T& value(const std::pair<const K, T> &p) {
			return p.second;
		}

		static const K &key(const std::pair<const K, T> &p) {
			return p.first;
		}
	};

	template <class T>
	struct KeyValue<std::unique_ptr<T>> {
		static Scripting::ScopeBase *value(const std::unique_ptr<T> &p) {
			return p.get();
		}

		static decltype(auto) key(const std::unique_ptr<T> &p) {
			return p->key();
		}
	};

	template <class T>
	struct KeyValue<const std::unique_ptr<T>> {
		static Scripting::ScopeBase *value(const std::unique_ptr<T> &p) {
			return p.get();
		}

		static decltype(auto) key(const std::unique_ptr<T> &p) {
			return p->key();
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

	template <class T, class K>
	struct Finder {
		static auto find(T &c, const K &key) {
			return std::find_if(c.begin(), c.end(), [&](decltype(*c.begin()) v) {return kvKey(v) == key; });
		}
	};

	template <class V, class K, class _K>
	struct Finder<std::map<K, V>, _K> {
		static auto find(std::map<K, V> &c, const _K &key) {
			return c.find(key);
		}
	};

	template <class V, class K, class _K>
	struct Finder<const std::map<K, V>, _K> {
		static auto find(const std::map<K, V> &c, const _K &key) {
			return c.find(key);
		}
	};

	template <class T, class K>
	decltype(auto) kvFind(T &c, const K &key) {
		return Finder<T, K>::find(c, key);
	}

	template <class T>
	struct KeyValueFlags {
		static KeyValueValueFlags flags(T &t) {
			return KeyValueValueFlags::NoFlags;
		}
	};

	template <class T>
	KeyValueValueFlags kvFlags(T &c) {
		return KeyValueFlags<T>::flags(c);
	}

	template <class T>
	KeyValueValueFlags kvFlags(const T &c) {
		return KeyValueFlags<const T>::flags(c);
	}

	template <class T>
	struct FixString {
		typedef T type;
	};

	template <>
	struct FixString<const char*> {
		typedef std::string type;
	};

	template <class T>
	struct KeyType {
		typedef typename FixString<typename std::remove_const<typename std::remove_reference<decltype(kvKey(std::declval<T>()))>::type>::type>::type type;
	};

	template <class T, class _ = decltype(ValueType{ std::declval<T>() })>
	ValueType toValueType(Scripting::ScopeBase *ref, const T &v) {
		return ValueType{ v };
	}

	template <class T, class _ = decltype(ValueType{ std::declval<T>().toValueType() })>
	ValueType toValueType(Scripting::ScopeBase *ref, T &v) {
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
		class TransformIterator : public std::iterator<typename std::iterator_traits<It>::iterator_category,
			typename std::remove_reference<decltype(std::declval<Converter>()(*std::declval<It>()))>::type> {
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
		KeyValueMapRef(T &map, Scripting::ScopeBase *ref) :
			mMap(map),
			mRef(ref)
		{

		}

		virtual std::shared_ptr<KeyValueIterator> iterator() override {
			return std::shared_ptr<KeyValueIterator>(new Iterator(mMap, mRef));
		}

		virtual std::pair<bool, ValueType> get(const std::string &key) override {
			auto it = Finder<T, std::string>::find(mMap, key);
			if (it != mMap.end()) {
				return { true, toValueType(mRef, kvValue(*it)) };
			}
			return { false, ValueType{} };
		}

		virtual bool contains(const std::string &key) override {
			auto it = Finder<T, std::string>::find(mMap, key);
			return it != mMap.end();
		}

	private:
		class Iterator : public KeyValueIterator {
		public:
			Iterator(T &map, Scripting::ScopeBase *ref) :
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

			virtual KeyValueValueFlags flags() override {
				return kvFlags(kvValue(*mIt));
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
			Scripting::ScopeBase *mRef;
		};

	private:
		T &mMap;
		Scripting::ScopeBase *mRef;
	};

	template <class T>
	class KeyValueItemRef : public KeyValueRef {
	public:
		KeyValueItemRef(T &item, Scripting::ScopeBase *ref) :
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
			Iterator(T &item, Scripting::ScopeBase *ref) :
				mItem(item),
				mEnded(false),
				mRef(ref)
			{
			}

			virtual std::string key() override {
				return kvKey(mItem);
			}

			virtual ValueType value() override {
				return toValueType(mRef, kvValue(mItem));
			}

			virtual KeyValueValueFlags flags() override {
				return kvFlags(mItem);
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
			Scripting::ScopeBase *mRef;
		};

	private:
		T &mItem;
		Scripting::ScopeBase *mRef;
	};

	template <class T>
	typename std::enable_if<is_iterable<T>::value, KeyValueMapRef<T>*>::type make_ref(Scripting::ScopeBase *ref, T &t) {
		return new KeyValueMapRef<T>(t, ref);
	}

	template <class T>
	typename std::enable_if<!is_iterable<T>::value, KeyValueItemRef<T>*>::type make_ref(Scripting::ScopeBase *ref, T &t) {
		return new KeyValueItemRef<T>(t, ref);
	}

	class INTERFACES_EXPORT KeyValueMapList {
	public:
		//using vector::vector;
		KeyValueMapList(Scripting::ScopeBase *ref) :
			mRef(ref) {
		}

		template <class... Ty>
		KeyValueMapList(Scripting::ScopeBase *ref, Ty&... maps) :
			mRef(ref)
		{
			mRefs.reserve(sizeof...(Ty));
			using unpacker = bool[];
			(void)unpacker {
				(mRefs.emplace_back(make_ref(ref, maps)), true)...
			};
		}
		KeyValueMapList() = delete;
		KeyValueMapList(const KeyValueMapList &) = delete;
		KeyValueMapList(KeyValueMapList &&) = default;

		KeyValueMapList &operator=(const KeyValueMapList &) = delete;

		std::pair<bool, ValueType> get(const std::string &key);
		std::unique_ptr<KeyValueIterator> iterator();

		KeyValueMapList merge(KeyValueMapList &&other) && ;

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
		Scripting::ScopeBase *mRef;
	};

	class KeyValueMapListIterator : public KeyValueIterator {
	public:
		KeyValueMapListIterator(const KeyValueMapList &list);
		virtual std::string key() override;
		virtual ValueType value() override;
		virtual void operator++() override;
		virtual bool ended() override;
		virtual KeyValueValueFlags flags() override;

	private:
		void validate();

	private:
		size_t mIndex;
		std::vector<std::shared_ptr<KeyValueIterator>> mIterators;
	};

}
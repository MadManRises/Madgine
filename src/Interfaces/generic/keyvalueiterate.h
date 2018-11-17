#pragma once

#include "container_traits.h"
#include "valuetype.h"
#include "keyvalue.h"

namespace Engine
{

	class KeyValueIterator
	{
	public:
		virtual ~KeyValueIterator() = default;
		virtual std::string key() const = 0;
		virtual ValueType value() const = 0;
		virtual KeyValueValueFlags flags() const = 0;
		virtual bool ended() const = 0;
		virtual void operator++() = 0;				
	};

	class KeyValueRef
	{
	public:
		virtual ~KeyValueRef() = default;
		virtual std::shared_ptr<KeyValueIterator> iterator() const = 0;
		virtual std::shared_ptr<KeyValueIterator> find(const std::string &key) const = 0;
		virtual std::optional<ValueType> get(const std::string& key) const = 0;
		virtual bool contains(const std::string& key) const = 0;
		virtual bool set(const std::string &key, const ValueType &value) = 0;		
	};


	template <class T, class _ = decltype(ValueType{ std::declval<T>() })>
	ValueType toValueType(Scripting::ScopeBase* ref, const T& v)
	{
		return ValueType{ v };
	}

	template <class T, class _ = decltype(ValueType{ std::declval<T>().toValueType() })>
	ValueType toValueType(Scripting::ScopeBase* ref, T& v)
	{
		return v.toValueType();
	}

	template <class T, class _ = decltype(std::declval<ValueType>.as<T>())>
	void fromValueType(Scripting::ScopeBase *ref, T& v, const ValueType &value)
	{
		v = value.as<T>();
	}

	template <class T>
	void fromValueType(Scripting::ScopeBase *ref, const T &v, const ValueType &value)
	{
		throw 0; //read-only
	}

	/*template <class T, class _ = decltype(std::declval<T>().fromValueType(std::declval<ValueType>()))>
	void fromValueType(Scripting::ScopeBase * ref, T &v, const ValueType &value)
	{
		v.fromValueType(value);
	}*/


	template <class T>
	class KeyValueMapRef : public KeyValueRef
	{
	public:
		KeyValueMapRef(T& map, Scripting::ScopeBase* ref) :
			mMap(map),
			mRef(ref)
		{
		}

		std::shared_ptr<KeyValueIterator> iterator() const override
		{
			return std::make_shared<Iterator>(mMap, mRef);
		}

		std::shared_ptr<KeyValueIterator> find(const std::string &key) const override
		{			
			return std::make_shared<Iterator>(mMap, mRef, kvFind(mMap, key));
		}

		std::optional<ValueType> get(const std::string& key) const override
		{
			auto it = kvFind(mMap, key);
			if (it != mMap.end())
			{
				return toValueType(mRef, kvValue(*it));
			}
			return {};
		}

		bool set(const std::string &key, const ValueType &value) override
		{
			auto it = kvFind(mMap, key);
			if (it != mMap.end())
			{
				fromValueType(mRef, kvValue(*it), value);
				return true;
			}
			return false;
		}

		bool contains(const std::string& key) const override
		{
			return kvFind(mMap, key) != mMap.end();
		}

	private:
		class Iterator : public KeyValueIterator
		{
		public:
			using InternIt = decltype(std::declval<T>().begin());

			Iterator(T& map, Scripting::ScopeBase* ref) :
				mMap(map),
				mIt(map.begin()),
				mRef(ref)
			{
			}

			Iterator(T& map, Scripting::ScopeBase* ref, const InternIt &it) :
				mMap(map),
				mIt(it),
				mRef(ref)
			{
			}


			std::string key() const override
			{
				return kvKey(*mIt);
			}

			ValueType value() const override
			{
				return toValueType(mRef, kvValue(*mIt));
			}

			KeyValueValueFlags flags() const override
			{
				return kvFlags(kvValue(*mIt));
			}

			void operator++() override
			{
				++mIt;
			}

			bool ended() const override
			{
				return mIt == mMap.end();
			}

		private:
			T & mMap;
			InternIt mIt;
			Scripting::ScopeBase* mRef;
		};

	private:
		T & mMap;
		Scripting::ScopeBase* mRef;
	};

	template <class T>
	class KeyValueItemRef : public KeyValueRef
	{
	public:
		KeyValueItemRef(T&& item, Scripting::ScopeBase* ref) :
			mItem(std::forward<T>(item)),
			mRef(ref)
		{
		}

		std::shared_ptr<KeyValueIterator> iterator() const override
		{
			return std::make_shared<Iterator>(mItem, mRef);
		}

		std::shared_ptr<KeyValueIterator> find(const std::string &key) const override
		{
			if (kvKey(mItem) == key)
				return std::make_shared<Iterator>(mItem, mRef);
			else
				return std::make_shared<Iterator>(mItem);
		}

		std::optional<ValueType> get(const std::string& key) const override
		{
			if (kvKey(mItem) == key)
			{
				return toValueType(mRef, kvValue(mItem));
			}
			return {};
		}

		bool set(const std::string &key, const ValueType &value) override
		{
			if (kvKey(mItem) == key)
			{
				fromValueType(mRef, kvValue(mItem), value);
				return true;
			}
			return false;
		}

		bool contains(const std::string& key) const override
		{
			return kvKey(mItem) == key;
		}

	private:
		class Iterator : public KeyValueIterator
		{
		public:
			Iterator(const T& item, Scripting::ScopeBase* ref) :
				mItem(item),
				mEnded(false),
				mRef(ref)
			{
			}

			Iterator(const T& item) :
			mItem(item),
			mEnded(true),
			mRef(nullptr)
			{
				
			}

			std::string key() const override
			{
				return kvKey(mItem);
			}

			ValueType value() const override
			{
				return toValueType(mRef, kvValue(mItem));
			}

			KeyValueValueFlags flags() const override
			{
				return kvFlags(mItem);
			}

			void operator++() override
			{
				mEnded = true;
			}

			bool ended() const override
			{
				return mEnded;
			}

		private:
			T mItem;
			bool mEnded;
			Scripting::ScopeBase* mRef;
		};

	private:
		T mItem;
		Scripting::ScopeBase* mRef;
	};

	template <class T>
	typename std::enable_if<is_iterable<T>::value, std::unique_ptr<KeyValueMapRef<T>>>::type make_ref(Scripting::ScopeBase* ref, T& t)
	{
		return std::make_unique<KeyValueMapRef<T>>(t, ref);
	}

	template <class T>
	typename std::enable_if<!is_iterable<T>::value, std::unique_ptr<KeyValueItemRef<T>>>::type make_ref(Scripting::ScopeBase* ref, T&& t)
	{
		return std::make_unique<KeyValueItemRef<T>>(std::forward<T>(t), ref);
	}

	class INTERFACES_EXPORT KeyValueMapList
	{
	public:
		//using vector::vector;
		explicit KeyValueMapList(Scripting::ScopeBase* ref) :
			mRef(ref)
		{
		}

		template <class... Ty>
		explicit KeyValueMapList(Scripting::ScopeBase* ref, Ty&&... maps) :
			mRef(ref)
		{
			mRefs.reserve(sizeof...(Ty));
			(mRefs.emplace_back(make_ref(ref, std::forward<Ty>(maps))) , ...);
		}

		KeyValueMapList() = delete;
		KeyValueMapList(const KeyValueMapList&) = delete;
		KeyValueMapList(KeyValueMapList&&) = default;

		KeyValueMapList& operator=(const KeyValueMapList&) = delete;

		std::unique_ptr<KeyValueIterator> find(const std::string &key);
		std::optional<ValueType> get(const std::string& key);
		bool set(const std::string &key, const ValueType &value);
		std::unique_ptr<KeyValueIterator> iterator();

		KeyValueMapList merge(KeyValueMapList&& other) && ;

		template <class... Ty>
		KeyValueMapList merge(Ty&&... maps) &&
		{
			return std::forward<KeyValueMapList>(*this).merge(KeyValueMapList{ mRef, std::forward<Ty>(maps)... });
		}

		std::vector<std::unique_ptr<KeyValueRef>>::const_iterator begin() const;
		std::vector<std::unique_ptr<KeyValueRef>>::const_iterator end() const;

		size_t size() const;

		bool contains(const std::string& key);

	private:
		std::vector<std::unique_ptr<KeyValueRef>> mRefs;
		Scripting::ScopeBase* mRef;
	};

	class KeyValueMapListIterator : public KeyValueIterator
	{
	public:
		KeyValueMapListIterator(const KeyValueMapList& list);
		KeyValueMapListIterator(std::vector<std::shared_ptr<KeyValueIterator>> &&elements = {});
		std::string key() const override;
		ValueType value() const override;
		KeyValueValueFlags flags() const override;
		bool ended() const override;
		void operator++() override;
		
		

	private:
		void validate();

	private:
		size_t mIndex;
		std::vector<std::shared_ptr<KeyValueIterator>> mIterators;
	};

}
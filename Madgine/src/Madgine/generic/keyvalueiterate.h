#pragma once

#include "valuetype.h"
#include "keyvalue.h"

namespace Engine
{

	enum KeyValueValueFlags;

	class KeyValueIterator
	{
	public:
		virtual ~KeyValueIterator() = default;
		virtual std::string key() = 0;
		virtual ValueType value() = 0;
		virtual void operator++() = 0;
		virtual bool ended() = 0;
		virtual KeyValueValueFlags flags() = 0;
	};

	class KeyValueRef
	{
	public:
		virtual ~KeyValueRef() = default;
		virtual std::shared_ptr<KeyValueIterator> iterator() = 0;
		virtual std::optional<ValueType> get(const std::string& key) = 0;
		virtual bool contains(const std::string& key) = 0;
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


	template <class T>
	class KeyValueMapRef : public KeyValueRef
	{
	public:
		KeyValueMapRef(T& map, Scripting::ScopeBase* ref) :
			mMap(map),
			mRef(ref)
		{
		}

		std::shared_ptr<KeyValueIterator> iterator() override
		{
			return std::static_pointer_cast<KeyValueIterator>(std::make_shared<Iterator>(mMap, mRef));
		}

		std::optional<ValueType> get(const std::string& key) override
		{
			auto it = Finder<T, std::string>::find(mMap, key);
			if (it != mMap.end())
			{
				return toValueType(mRef, kvValue(*it));
			}
			return {};
		}

		bool contains(const std::string& key) override
		{
			auto it = Finder<T, std::string>::find(mMap, key);
			return it != mMap.end();
		}

	private:
		class Iterator : public KeyValueIterator
		{
		public:
			Iterator(T& map, Scripting::ScopeBase* ref) :
				mMap(map),
				mIt(map.begin()),
				mRef(ref)
			{
			}

			std::string key() override
			{
				return kvKey(*mIt);
			}

			ValueType value() override
			{
				return toValueType(mRef, kvValue(*mIt));
			}

			KeyValueValueFlags flags() override
			{
				return kvFlags(kvValue(*mIt));
			}

			void operator++() override
			{
				++mIt;
			}

			bool ended() override
			{
				return mIt == mMap.end();
			}

		private:
			T & mMap;
			decltype(std::declval<T>().begin()) mIt;
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

		std::shared_ptr<KeyValueIterator> iterator() override
		{
			return std::static_pointer_cast<KeyValueIterator>(std::make_shared<Iterator>(mItem, mRef));
		}

		std::optional<ValueType> get(const std::string& key) override
		{
			if (kvKey(mItem) == key)
			{
				return toValueType(mRef, kvValue(mItem));
			}
			return {};
		}

		bool contains(const std::string& key) override
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

			std::string key() override
			{
				return kvKey(mItem);
			}

			ValueType value() override
			{
				return toValueType(mRef, kvValue(mItem));
			}

			KeyValueValueFlags flags() override
			{
				return kvFlags(mItem);
			}

			void operator++() override
			{
				mEnded = true;
			}

			bool ended() override
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
			using unpacker = bool[];
			(void)unpacker {
				(mRefs.emplace_back(make_ref(ref, std::forward<Ty>(maps))), true)...
			};
		}

		KeyValueMapList() = delete;
		KeyValueMapList(const KeyValueMapList&) = delete;
		KeyValueMapList(KeyValueMapList&&) = default;

		KeyValueMapList& operator=(const KeyValueMapList&) = delete;

		std::optional<ValueType> get(const std::string& key);
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
		std::string key() override;
		ValueType value() override;
		void operator++() override;
		bool ended() override;
		KeyValueValueFlags flags() override;

	private:
		void validate();

	private:
		size_t mIndex;
		std::vector<std::shared_ptr<KeyValueIterator>> mIterators;
	};

}
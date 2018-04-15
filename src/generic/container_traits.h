#pragma once

#include "keyvalue.h"

namespace Engine
{
	template <template <class...> class C, class T>
	struct container_traits
	{
		static constexpr const bool sorted = false;

		typedef C<T> container;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::value_type value_type;
		typedef void key_type;
		typedef T type;

		template <class... _Ty>
		static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
		{
			return c.emplace(where, std::forward<_Ty>(args)...);
		}

		static iterator keepIterator(const iterator& begin, const iterator& it)
		{
			return it;
		}

		static iterator revalidateIteratorInsert(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}

		static iterator revalidateIteratorRemove(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}
	};

	template <class T>
	struct container_traits<std::list, T>
	{
		static constexpr const bool sorted = false;

		typedef std::list<T> container;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::value_type value_type;
		typedef void key_type;
		typedef T type;

		template <class... _Ty>
		static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
		{
			return std::make_pair(c.emplace(where, std::forward<_Ty>(args)...), true);
		}

		static iterator keepIterator(const iterator& begin, const iterator& it)
		{
			return it;
		}

		static iterator revalidateIteratorInsert(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}

		static iterator revalidateIteratorRemove(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}
	};

	template <class T>
	struct container_traits<std::vector, T>
	{
		static constexpr const bool sorted = false;

		typedef std::vector<T> container;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::value_type value_type;
		typedef void key_type;
		typedef T type;

		template <class... _Ty>
		static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
		{
			return std::make_pair(c.emplace(where, std::forward<_Ty>(args)...), true);
		}

		static size_t keepIterator(const iterator& begin, const iterator& it)
		{
			return std::distance(begin, it);
		}

		static iterator revalidateIteratorInsert(const iterator& begin, size_t dist, const iterator& it)
		{
			size_t item = std::distance(begin, it);
			if (item <= dist)
				++dist;
			iterator newIt = begin;
			std::advance(newIt, dist);
			return newIt;
		}

		static iterator revalidateIteratorRemove(const iterator& begin, size_t dist, const iterator& it)
		{
			size_t item = std::distance(begin, it);
			if (item == dist)
				throw 0;
			if (item < dist)
				--dist;
			iterator newIt = begin;
			std::advance(newIt, dist);
			return newIt;
		}
	};


	template <class _Ty = void>
	struct KeyCompare
	{
		// functor for operator<
		typedef _Ty first_argument_type;
		typedef _Ty second_argument_type;
		typedef bool result_type;

		constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{
			// apply operator< to operands
			return (kvKey(_Left) < kvKey(_Right));
		}
	};

	template <class _Ty = void>
	struct KeyHash
	{	
		typedef _Ty argument_type;
		typedef size_t result_type;

		constexpr size_t operator()(const _Ty& _Arg) const 
		{
			return std::hash<typename KeyType<_Ty>::type>{}(kvKey(_Arg));
		}
	};

	template <class T>
	class SetConstIterator;

	template <class T>
	class SetIterator : public std::iterator<typename std::iterator_traits<typename std::set<T>::iterator>::
	                                         iterator_category, T>
	{
	public:
		typedef typename std::set<T>::iterator It;

		SetIterator()
		{
		}

		SetIterator(It&& it) :
			mIterator(std::forward<It>(it))
		{
		}

		SetIterator(const It& it) :
			mIterator(it)
		{
		}

		T& operator *() const
		{
			return const_cast<T&>(*mIterator);
		}

		T* operator ->() const
		{
			return &const_cast<T&>(*mIterator);
		}

		bool operator !=(const SetIterator<T>& other) const
		{
			return mIterator != other.mIterator;
		}

		bool operator ==(const SetIterator<T>& other) const
		{
			return mIterator == other.mIterator;
		}

		void operator++()
		{
			++mIterator;
		}

		void operator--()
		{
			--mIterator;
		}

		operator const It &() const
		{
			return mIterator;
		}

	private:
		friend class SetConstIterator<T>;

		It mIterator;
	};

	template <class T>
	class SetConstIterator : public std::iterator<typename std::iterator_traits<typename std::set<T>::iterator>::
	                                              iterator_category, T>
	{
	public:
		typedef typename std::set<T>::const_iterator It;

		SetConstIterator(It&& it) :
			mIterator(std::forward<It>(it))
		{
		}

		SetConstIterator(const It& it) :
			mIterator(it)
		{
		}

		SetConstIterator(const SetIterator<T>& it) :
			mIterator(it.mIterator)
		{
		}

		const T& operator *() const
		{
			return *mIterator;
		}

		const T* operator ->() const
		{
			return &*mIterator;
		}

		bool operator !=(const SetConstIterator<T>& other) const
		{
			return mIterator != other.mIterator;
		}

		bool operator ==(const SetConstIterator<T>& other) const
		{
			return mIterator == other.mIterator;
		}

		void operator++()
		{
			++mIterator;
		}

		void operator--()
		{
			--mIterator;
		}

		operator const It &() const
		{
			return mIterator;
		}

	private:

		It mIterator;
	};


	template <class T>
	struct container_traits<std::set, T>
	{
		static constexpr const bool sorted = true;

		typedef std::set<T, KeyCompare<T>> container;
		typedef SetIterator<T> iterator;
		typedef SetConstIterator<T> const_iterator;
		typedef typename KeyType<T>::type key_type;
		typedef typename container::value_type value_type;
		typedef T type;

		template <class... _Ty>
		static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
		{
			return c.emplace(std::forward<_Ty>(args)...);
		}

		static iterator keepIterator(const iterator& begin, const iterator& it)
		{
			return it;
		}

		static iterator revalidateIteratorInsert(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}

		static iterator revalidateIteratorRemove(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}
	};


	template <class T>
	class UnorderedSetConstIterator;

	template <class T>
	class UnorderedSetIterator : public std::iterator<typename std::iterator_traits<typename std::unordered_set<T>::iterator>::
		iterator_category, T>
	{
	public:
		typedef typename std::unordered_set<T>::iterator It;

		UnorderedSetIterator()
		{
		}

		UnorderedSetIterator(It&& it) :
			mIterator(std::forward<It>(it))
		{
		}

		UnorderedSetIterator(const It& it) :
			mIterator(it)
		{
		}

		T& operator *() const
		{
			return const_cast<T&>(*mIterator);
		}

		T* operator ->() const
		{
			return &const_cast<T&>(*mIterator);
		}

		bool operator !=(const UnorderedSetIterator<T>& other) const
		{
			return mIterator != other.mIterator;
		}

		bool operator ==(const UnorderedSetIterator<T>& other) const
		{
			return mIterator == other.mIterator;
		}

		void operator++()
		{
			++mIterator;
		}

		operator const It &() const
		{
			return mIterator;
		}

	private:
		friend class UnorderedSetConstIterator<T>;

		It mIterator;
	};

	template <class T>
	class UnorderedSetConstIterator : public std::iterator<typename std::iterator_traits<typename std::unordered_set<T>::iterator>::
		iterator_category, T>
	{
	public:
		typedef typename std::unordered_set<T>::const_iterator It;

		UnorderedSetConstIterator(It&& it) :
			mIterator(std::forward<It>(it))
		{
		}

		UnorderedSetConstIterator(const It& it) :
			mIterator(it)
		{
		}

		UnorderedSetConstIterator(const UnorderedSetIterator<T>& it) :
			mIterator(it.mIterator)
		{
		}

		const T& operator *() const
		{
			return *mIterator;
		}

		const T* operator ->() const
		{
			return &*mIterator;
		}

		bool operator !=(const UnorderedSetConstIterator<T>& other) const
		{
			return mIterator != other.mIterator;
		}

		bool operator ==(const UnorderedSetConstIterator<T>& other) const
		{
			return mIterator == other.mIterator;
		}

		void operator++()
		{
			++mIterator;
		}

		operator const It &() const
		{
			return mIterator;
		}

	private:

		It mIterator;
	};


	template <class T>
	struct container_traits<std::unordered_set, T>
	{
		static constexpr const bool sorted = true;

		typedef std::unordered_set<T, KeyHash<T>, KeyCompare<T>> container;
		typedef UnorderedSetIterator<T> iterator;
		typedef UnorderedSetConstIterator<T> const_iterator;
		typedef typename KeyType<T>::type key_type;
		typedef typename container::value_type value_type;
		typedef T type;

		template <class... _Ty>
		static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
		{
			return c.emplace(std::forward<_Ty>(args)...);
		}

		static iterator keepIterator(const iterator& begin, const iterator& it)
		{
			return it;
		}

		static iterator revalidateIteratorInsert(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}

		static iterator revalidateIteratorRemove(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}
	};


	template <class K, class T>
	struct container_traits<std::map, std::pair<const K, T>>
	{
		static constexpr const bool sorted = true;

		typedef std::map<K, T> container;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		typedef K key_type;
		typedef T value_type;
		typedef std::pair<const K, T> type;

		template <class... _Ty>
		static std::pair<iterator, bool> emplace(container& c, const const_iterator& where, _Ty&&... args)
		{
			return c.emplace(std::forward<_Ty>(args)...);
		}

		static iterator keepIterator(const iterator& begin, const iterator& it)
		{
			return it;
		}

		static iterator revalidateIteratorInsert(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}

		static iterator revalidateIteratorRemove(const iterator& begin, const iterator& dist, const iterator& it)
		{
			return dist;
		}
	};
}

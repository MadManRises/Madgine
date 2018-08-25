#pragma once

#include "keyvalue.h"

namespace Engine
{

	template <class T, typename = void>
	struct is_iterable : std::false_type {};

	template <class T>
	struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {};

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
			return std::next(begin, dist);
		}

		static iterator revalidateIteratorRemove(const iterator& begin, size_t dist, const iterator& it)
		{
			size_t item = std::distance(begin, it);
			assert (item != dist);
			if (item < dist)
				--dist;
			return std::next(begin, dist);
		}
	};

	template <class T>
	class SetConstIterator;

	template <class T>
	class SetIterator
	{
	public:
		typedef typename std::set<T>::iterator It;

		using iterator_category = typename It::iterator_category;
		using value_type = typename It::value_type;
		using difference_type = typename It::difference_type;
		using pointer = std::remove_const_t<typename It::value_type>*;
		using reference = std::remove_const_t<typename It::value_type>&;

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
	class SetConstIterator
	{
	public:
		typedef typename std::set<T>::const_iterator It;

		using iterator_category = typename It::iterator_category;
		using value_type = typename It::value_type;
		using difference_type = typename It::difference_type;
		using pointer = typename It::pointer;
		using reference = typename It::reference;

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

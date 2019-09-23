#pragma once

#include "../keyvalue/keyvalue.h"

#include "sortedcontainerapi.h"

namespace Engine {

template <class T, typename = void>
struct is_iterable : std::false_type {
};

template <class T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : std::true_type {
};

template <typename C>
struct container_traits : C::traits {
};

template <typename T>
struct container_traits<std::list<T>> {
    static constexpr const bool sorted = false;

    typedef std::list<T> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::value_type value_type;
    typedef void key_type;
    typedef T type;
		
	template <template <typename> typename M>
	using rebind = container_traits<std::list<M<T>>>;

    template <typename C>
    struct api : C {

			using C::C;

            void remove(const type &item)
            {
                for (iterator it = this->begin(); it != this->end();) {
                    if (*it == item) {
                        it = this->erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            void push_back(const type &item)
            {
                this->emplace(this->end(), item);
            }

            template <class... _Ty>
            std::pair<iterator, bool> emplace_back(_Ty &&... args)
            {
                return this->emplace(this->end(), std::forward<_Ty>(args)...);
            }

            const type &back() const
            {
                return C::back();
            }

            type &back()
            {
                return C::back();
            }
    };

    template <typename... _Ty>
    static std::pair<iterator, bool> emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return std::make_pair(c.emplace(where, std::forward<_Ty>(args)...), true);
    }

    static iterator keepIterator(const iterator &begin, const iterator &it)
    {
        return it;
    }

    static iterator revalidateIteratorInsert(const iterator &begin, const iterator &dist, const iterator &it)
    {
        return dist;
    }

    static iterator revalidateIteratorRemove(const iterator &begin, const iterator &dist, const iterator &it, size_t count = 1)
    {
        return dist;
    }
};

template <typename T>
struct container_traits<std::vector<T>> {
    static constexpr const bool sorted = false;

    typedef std::vector<T> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::value_type value_type;
    typedef void key_type;
    typedef T type;
	
	template <template <typename> typename M>
	using rebind = container_traits<std::vector<M<T>>>;

    template <typename C>
        struct api : C {
            
            using C::C;

            using C::operator=;

            /*void resize(size_t size)
            {
                C::resize(size);
            }*/

            void remove(const type &item)
            {
                for (const_iterator it = this->begin(); it != this->end();) {
                    if (*it == item) {
                        it = erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            void push_back(const type &item)
            {
                emplace(this->end(), item);
            }

            template <typename... _Ty>
            type &emplace_back(_Ty &&... args)
            {
                return *this->emplace(this->end(), std::forward<_Ty>(args)...).first;
            }

            type &at(size_t i)
            {
                return C::at(i);
            }

            const type &at(size_t i) const
            {
                return C::at(i);
            }

            type &operator[](size_t i)
            {
                return C::operator[](i);
            }

            const type &operator[](size_t i) const
            {
                return C::operator[](i);
            }
        };

    template <class... _Ty>
    static std::pair<iterator, bool> emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return std::make_pair(c.emplace(where, std::forward<_Ty>(args)...), true);
    }

    static size_t keepIterator(const iterator &begin, const iterator &it)
    {
        return std::distance(begin, it);
    }

    static iterator revalidateIteratorInsert(const iterator &begin, size_t dist, const iterator &it)
    {
        size_t item = std::distance(begin, it);
        if (item <= dist)
            ++dist;
        return std::next(begin, dist);
    }

    static iterator revalidateIteratorRemove(const iterator &begin, size_t dist, const iterator &it, size_t count = 1)
    {
        size_t pivot = std::distance(begin, it);
        assert(dist < pivot || dist >= pivot + count);
        if (dist > pivot)
            dist -= count;
        return std::next(begin, dist);
    }
};

template <class T>
class SetConstIterator;

template <class T>
class SetIterator {
public:
    typedef typename std::set<T>::iterator It;

    using iterator_category = typename It::iterator_category;
    using value_type = typename It::value_type;
    using difference_type = typename It::difference_type;
    using pointer = std::remove_const_t<typename It::value_type> *;
    using reference = std::remove_const_t<typename It::value_type> &;

    SetIterator()
    {
    }

    SetIterator(It &&it)
        : mIterator(std::forward<It>(it))
    {
    }

    SetIterator(const It &it)
        : mIterator(it)
    {
    }

    T &operator*() const
    {
        return const_cast<T &>(*mIterator);
    }

    T *operator->() const
    {
        return &const_cast<T &>(*mIterator);
    }

    bool operator!=(const SetIterator<T> &other) const
    {
        return mIterator != other.mIterator;
    }

    bool operator==(const SetIterator<T> &other) const
    {
        return mIterator == other.mIterator;
    }

    SetIterator<T> &operator++()
    {
        ++mIterator;
        return *this;
    }

    SetIterator<T> &operator--()
    {
        --mIterator;
        return *this;
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
class SetConstIterator {
public:
    typedef typename std::set<T>::const_iterator It;

    using iterator_category = typename It::iterator_category;
    using value_type = typename It::value_type;
    using difference_type = typename It::difference_type;
    using pointer = typename It::pointer;
    using reference = typename It::reference;

    SetConstIterator(It &&it)
        : mIterator(std::forward<It>(it))
    {
    }

    SetConstIterator(const It &it)
        : mIterator(it)
    {
    }

    SetConstIterator(const SetIterator<T> &it)
        : mIterator(it.mIterator)
    {
    }

    const T &operator*() const
    {
        return *mIterator;
    }

    const T *operator->() const
    {
        return &*mIterator;
    }

    bool operator!=(const SetConstIterator<T> &other) const
    {
        return mIterator != other.mIterator;
    }

    bool operator==(const SetConstIterator<T> &other) const
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

template <typename T>
struct container_traits<std::set<T>> {
    static constexpr const bool sorted = true;

    typedef std::set<T, KeyCompare<T>> container;
    typedef SetIterator<T> iterator;
    typedef SetConstIterator<T> const_iterator;
    typedef typename KeyType<T>::type key_type;
    typedef typename container::value_type value_type;
    typedef T type;

	template <template <typename> typename M>
	using rebind = container_traits<std::set<M<T>>>;

   template <typename C>
        struct api : SortedContainerApi<C> {
            using Base = SortedContainerApi<C>;

            using Base::Base;

            template <class... _Ty>
            std::pair<iterator, bool> emplace(_Ty &&... args)
            {
                return this->Base::emplace(this->end(), std::forward<_Ty>(args)...);
            }

        };

    template <class... _Ty>
    static std::pair<iterator, bool> emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return c.emplace(std::forward<_Ty>(args)...);
    }

    static iterator keepIterator(const iterator &begin, const iterator &it)
    {
        return it;
    }

    static iterator revalidateIteratorInsert(const iterator &begin, const iterator &dist, const iterator &it)
    {
        return dist;
    }

    static iterator revalidateIteratorRemove(const iterator &begin, const iterator &dist, const iterator &it, size_t count = 1)
    {
        return dist;
    }
};

template <typename K, typename T>
struct container_traits<std::map<K, T>> {
    static constexpr const bool sorted = true;

    typedef std::map<K, T> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef K key_type;
    typedef T value_type;
    typedef std::pair<const K, T> type;

	template <template <typename> typename M>
	using rebind = container_traits<std::map<M<K>, M<T>>>;

    template <typename C>
        struct api : C {
            using C::C;

            T &operator[](const K &key) {
				iterator it = C::lower_bound(key);
				if (it == this->end() || it->first != key) {
                    auto pib = try_emplace(key);
                                    assert(pib.second);
                    it = pib.first;
				}
				return it->second;
			}

            const T &at(const std::string &key) const {
				return C::at(key);
			}

            template <class... _Ty>
            std::pair<iterator, bool> try_emplace(const K &key, _Ty &&... args)
            {
                auto it = C::lower_bound(key);
                if (it != this->end() && it->first == key) {
                    return { it, false };
                }
                return C::emplace(it, std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(args...));
            }
        };

    template <class... _Ty>
    static std::pair<iterator, bool> emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return c.emplace(std::forward<_Ty>(args)...);
    }

    static iterator keepIterator(const iterator &begin, const iterator &it)
    {
        return it;
    }

    static iterator revalidateIteratorInsert(const iterator &begin, const iterator &dist, const iterator &it)
    {
        return dist;
    }

    static iterator revalidateIteratorRemove(const iterator &begin, const iterator &dist, const iterator &it, size_t count = 1)
    {
        return dist;
    }
};

}

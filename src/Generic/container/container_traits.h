#pragma once

#include "underlying_container.h"
#include "../indextype.h"

namespace Engine {

template <typename It>
struct Pib {

    Pib() = default;

    template <typename It2>
    Pib(std::pair<It2, bool> &&pib)
        : first(std::move(pib.first))
        , second(pib.second)
    {
    }

    template <typename It2>
    Pib(It2 &&it)
        : first(std::forward<It2>(it))
    {
    }

    decltype(auto) operator*() const
    {
        return *first;
    }

    operator It &()
    {
        return first;
    }

    operator const It &() const
    {
        return first;
    }

    bool success() const
    {
        return second;
    }

    It first;
    bool second = true;
};

template <typename C, typename = void>
struct container_traits : container_traits<typename underlying_container<C>::type> {
    typedef C container;
};

template <typename T>
struct container_traits<std::list<T>, void> {
    static constexpr const bool sorted = false;
    static constexpr const bool has_dependent_handle = false;
    static constexpr const bool remove_invalidates_handles = false;
    static constexpr const bool is_fixed_size = false;

    typedef std::list<T> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;
    typedef T *handle;
    typedef const T *const_handle;
    typedef iterator position_handle;
    typedef const_iterator const_position_handle;
    typedef typename container::value_type value_type;

    typedef iterator emplace_return;

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return c.emplace(where, std::forward<_Ty>(args)...);
    }

    static bool was_emplace_successful(const emplace_return &)
    {
        return true;
    }

    static position_handle toPositionHandle(container &c, const iterator &it)
    {
        return it;
    }

    static handle toHandle(container &c, const iterator &it)
    {
        if (it == c.end())
            return nullptr;
        return &*it;
    }

    static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
    {
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const iterator &it, bool wasIn, size_t count = 1)
    {
        if (wasIn)
            handle = it;
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return handle;
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return handle;
    }

    static position_handle next(container &c, const position_handle &handle)
    {
        return std::next(handle);
    }

    static position_handle prev(container &c, const position_handle &handle)
    {
        return std::prev(handle);
    }
};

template <typename T>
struct container_traits<std::vector<T>, void> {
    static constexpr const bool sorted = false;
    static constexpr const bool has_dependent_handle = true;
    static constexpr const bool remove_invalidates_handles = true;
    static constexpr const bool is_fixed_size = false;

    typedef std::vector<T> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;    

    typedef IndexType<size_t> handle;
    typedef IndexType<size_t> const_handle;
    typedef IndexType<size_t> position_handle;
    typedef IndexType<size_t> const_position_handle;
    typedef typename container::value_type value_type;

    typedef iterator emplace_return;

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return c.emplace(where, std::forward<_Ty>(args)...);
    }

    static bool was_emplace_successful(const emplace_return &)
    {
        return true;
    }

    static position_handle toPositionHandle(container &c, const iterator &it)
    {
        return std::distance(c.begin(), it);
    }

    static handle toHandle(container &c, const position_handle &handle)
    {
        return handle;
    }

    static handle toHandle(container &c, const iterator &it)
    {
        if (it == c.end())
            return {};
        return std::distance(c.begin(), it);
    }

    static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
    {
        size_t item = std::distance(c.begin(), it);
        if (item <= handle)
            ++handle;
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const const_iterator &it, bool wasIn, size_t count = 1)
    {
        size_t pivot = std::distance(c.begin(), it);
        if (wasIn) {
            handle = pivot;
        } else {
            assert(handle < pivot || handle >= pivot + count);
            if (handle > pivot)
                handle -= count;
        }
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return c.begin() + handle;
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return c.begin() + handle;
    }

    static position_handle next(container &c, const position_handle &handle)
    {
        return handle + 1;
    }

    static position_handle prev(container &c, const position_handle &handle)
    {
        return handle - 1;
    }
};

template <typename T, typename Cmp>
struct container_traits<std::set<T, Cmp>, void> {
    static constexpr const bool sorted = true;
    static constexpr const bool has_dependent_handle = false;
    static constexpr const bool remove_invalidates_handles = false;
    static constexpr const bool is_fixed_size = false;

    typedef std::set<T, Cmp> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;
    typedef const T *handle;
    typedef const T *const_handle;
    typedef iterator position_handle;
    typedef const_iterator const_position_handle;
    typedef Cmp cmp_type;
    typedef const typename container::value_type value_type;

    typedef Pib<iterator> emplace_return;

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return c.emplace(std::forward<_Ty>(args)...);
    }

    static bool was_emplace_successful(const emplace_return &pib)
    {
        return pib.success();
    }

    static position_handle toPositionHandle(container &c, const iterator &it)
    {
        return it;
    }

    static handle toHandle(container &c, const iterator &it)
    {
        if (it == c.end())
            return nullptr;
        return &*it;
    }

    static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
    {
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const iterator &it, bool wasIn, size_t count = 1)
    {
        if (wasIn)
            handle = it;
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return handle;
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return handle;
    }

    static position_handle next(container &c, const position_handle &handle)
    {
        return std::next(handle);
    }

    static position_handle prev(container &c, const position_handle &handle)
    {
        return std::prev(handle);
    }
};

template <typename K, typename T, typename Cmp>
struct container_traits<std::map<K, T, Cmp>, void> {
    static constexpr const bool sorted = true;
    static constexpr const bool has_dependent_handle = false;
    static constexpr const bool remove_invalidates_handles = false;
    static constexpr const bool is_fixed_size = false;

    typedef std::map<K, T, Cmp> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;
    typedef std::pair<const K, T> *handle;
    typedef const std::pair<const K, T> *const_handle;
    typedef iterator position_handle;
    typedef const_iterator const_position_handle;
    typedef Cmp cmp_type;
    typedef typename container::value_type value_type;

    typedef Pib<iterator> emplace_return;

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return c.emplace(std::forward<_Ty>(args)...);
    }

    static bool was_emplace_successful(const emplace_return &pib)
    {
        return pib.success();
    }

    static position_handle toPositionHandle(container &c, const iterator &it)
    {
        return it;
    }

    static handle toHandle(container &c, const iterator &it)
    {
        if (it == c.end())
            return nullptr;
        return &*it;
    }

    static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
    {
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const iterator &it, bool wasIn, size_t count = 1)
    {
        if (wasIn)
            handle = it;
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return handle;
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return handle;
    }

    static position_handle next(container &c, const position_handle &handle)
    {
        return std::next(handle);
    }

    static position_handle prev(container &c, const position_handle &handle)
    {
        return std::prev(handle);
    }
};

template <typename T, size_t Size>
struct container_traits<std::array<T, Size>, void> {
    static constexpr const bool sorted = false;
    static constexpr const bool has_dependent_handle = true;
    static constexpr const bool remove_invalidates_handles = false;
    static constexpr const bool is_fixed_size = true;

    typedef std::array<T, Size> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;

    struct emplace_return {
    };

    struct handle_t {
        handle_t(size_t index = std::numeric_limits<size_t>::max())
            : mIndex(index)
        {
        }

        operator size_t() const { return mIndex; }

        void operator++() { ++mIndex; }
        void operator--() { --mIndex; }
        handle_t &operator-=(size_t s)
        {
            mIndex -= s;
            return *this;
        }

        size_t mIndex;
    };

    typedef handle_t handle;
    typedef handle_t const_handle;
    typedef handle_t position_handle;
    typedef handle_t const_position_handle;
    typedef typename container::value_type value_type;

    template <typename C>
    struct api : C {

        using C::C;

        using C::operator=;

        /*void resize(size_t size)
            {
                C::resize(size);
            }*/

        value_type &at(size_t i)
        {
            return C::at(i);
        }

        const value_type &at(size_t i) const
        {
            return C::at(i);
        }

        value_type &operator[](size_t i)
        {
            return C::operator[](i);
        }

        const value_type &operator[](size_t i) const
        {
            return C::operator[](i);
        }
    };

    static position_handle toPositionHandle(container &c, const iterator &it)
    {
        return std::distance(c.begin(), it);
    }

    static handle toHandle(container &c, const position_handle &handle)
    {
        return handle;
    }

    static handle toHandle(container &c, const iterator &it)
    {
        if (it == c.end())
            return {};
        return std::distance(c.begin(), it);
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return c.begin() + handle;
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return c.begin() + handle;
    }

    static position_handle next(container &c, const position_handle &handle)
    {
        return handle + 1;
    }

    static position_handle prev(container &c, const position_handle &handle)
    {
        return handle - 1;
    }
};

}

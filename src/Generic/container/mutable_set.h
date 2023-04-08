#pragma once

namespace Engine {

template <typename T, typename Cmp>
struct mutable_set : std::set<T, Cmp> {

    using Base = std::set<T, Cmp>;

    using _traits = container_traits<Base>;

    template <typename It, bool isConst>
    struct iterator_prototype {
        using iterator_category = typename It::iterator_category;
        using value_type = const_if<isConst, std::remove_const_t<typename It::value_type>>;
        using difference_type = typename It::difference_type;
        using pointer = value_type *;
        using reference = value_type &;

        iterator_prototype()
        {
        }

        /* iterator_prototype(const iterator_prototype &) = default;
        iterator_prototype(iterator_prototype &&) = default;*/

        iterator_prototype(It &&it)
            : mIterator(std::forward<It>(it))
        {
        }

        iterator_prototype(const It &it)
            : mIterator(it)
        {
        }

        template <typename It2, bool isConst2>
        requires (!isConst2 || isConst)
        iterator_prototype(const iterator_prototype<It2, isConst2> &other)
            : mIterator(static_cast<const It2 &>(other))
        {
        }

        /* iterator_prototype &operator=(const iterator_prototype &) = default;
        iterator_prototype &operator=(iterator_prototype &&) = default;*/

        T &operator*() const
        {
            return const_cast<T &>(*mIterator);
        }

        T *operator->() const
        {
            return &const_cast<T &>(*mIterator);
        }

        template <bool isConst2>
        bool operator!=(const iterator_prototype<It, isConst2> &other) const
        {
            return mIterator != other.mIterator;
        }

        template <bool isConst2>
        bool operator==(const iterator_prototype<It, isConst2> &other) const
        {
            return mIterator == other.mIterator;
        }

        iterator_prototype &operator++()
        {
            ++mIterator;
            return *this;
        }

        iterator_prototype operator++(int)
        {
            iterator_prototype copy = *this;
            ++*this;
            return copy;
        }

        iterator_prototype &operator--()
        {
            --mIterator;
            return *this;
        }

        operator const It &() const
        {
            return mIterator;
        }

    private:
        It mIterator;
    };

    typedef iterator_prototype<typename Base::iterator, false> iterator;
    typedef iterator_prototype<typename Base::const_iterator, true> const_iterator;
    typedef iterator_prototype<typename Base::reverse_iterator, false> reverse_iterator;
    typedef iterator_prototype<typename Base::const_reverse_iterator, true> const_reverse_iterator;

    template <typename Arg>
    iterator find(Arg &&arg)
    {
        return Base::find(std::forward<Arg>(arg));
    }

    template <typename Arg>
    const_iterator find(Arg &&arg) const
    {
        return Base::find(std::forward<Arg>(arg));
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args &&...args) {
        return Base::emplace(std::forward<Args>(args)...);
    }

    iterator begin()
    {
        return Base::begin();
    }

    const_iterator begin() const
    {
        return Base::begin();
    }

    iterator end()
    {
        return Base::end();
    }

    const_iterator end() const
    {
        return Base::end();
    }

    reverse_iterator rbegin()
    {
        return Base::rbegin();
    }

    const_reverse_iterator rbegin() const
    {
        return Base::rbegin();
    }

    reverse_iterator rend()
    {
        return Base::rend();
    }

    const_reverse_iterator rend() const
    {
        return Base::rend();
    }
};

template <typename T, typename Cmp>
struct underlying_container<mutable_set<T, Cmp>> {
    typedef std::set<T, Cmp> type;
};

template <typename T, typename Cmp>
struct container_traits<mutable_set<T, Cmp>> : container_traits<std::set<T, Cmp>> {

    typedef mutable_set<T, Cmp> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;

    typedef T *handle;
    typedef const T *const_handle;
    typedef iterator position_handle;
    typedef const_iterator const_position_handle;
    typedef Cmp cmp_type;
    typedef T value_type;

    //static_assert(sizeof(position_handle) <= sizeof(void *));

    typedef Pib<iterator> emplace_return;

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&...args)
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

}
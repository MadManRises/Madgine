#pragma once

namespace Engine {

template <typename T, typename Cmp, typename It>
struct NonConstSetConstIterator;

template <typename T, typename Cmp, typename It>
struct NonConstSetIterator {
    using iterator_category = typename It::iterator_category;
    using value_type = typename It::value_type;
    using difference_type = typename It::difference_type;
    using pointer = std::remove_const_t<typename It::value_type> *;
    using reference = std::remove_const_t<typename It::value_type> &;

    NonConstSetIterator()
    {
    }

    NonConstSetIterator(It &&it)
        : mIterator(std::forward<It>(it))
    {
    }

    NonConstSetIterator(const It &it)
        : mIterator(it)
    {
    }

    template <typename It2>
    NonConstSetIterator(const NonConstSetIterator<T, Cmp, It2> &other)
        : mIterator(static_cast<const It2 &>(other))
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

    bool operator!=(const NonConstSetIterator<T, Cmp, It> &other) const
    {
        return mIterator != other.mIterator;
    }

    bool operator==(const NonConstSetIterator<T, Cmp, It> &other) const
    {
        return mIterator == other.mIterator;
    }

    bool operator==(const NonConstSetConstIterator<T, Cmp, It> &other) const
    {
        return mIterator == other.mIterator;
    }

    NonConstSetIterator<T, Cmp, It> &
    operator++()
    {
        ++mIterator;
        return *this;
    }

    NonConstSetIterator<T, Cmp, It> &operator--()
    {
        --mIterator;
        return *this;
    }

    operator const It &() const
    {
        return mIterator;
    }

private:
    friend struct NonConstSetConstIterator<T, Cmp, It>;

    It mIterator;
};

template <typename T, typename Cmp, typename It>
struct NonConstSetConstIterator {
    using iterator_category = typename It::iterator_category;
    using value_type = typename It::value_type;
    using difference_type = typename It::difference_type;
    using pointer = typename It::pointer;
    using reference = typename It::reference;

    NonConstSetConstIterator(It &&it)
        : mIterator(std::forward<It>(it))
    {
    }

    NonConstSetConstIterator(const It &it)
        : mIterator(it)
    {
    }

    NonConstSetConstIterator(const NonConstSetIterator<T, Cmp, It> &it)
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

    bool operator!=(const NonConstSetConstIterator<T, Cmp, It> &other) const
    {
        return mIterator != other.mIterator;
    }

    bool operator==(const NonConstSetConstIterator<T, Cmp, It> &other) const
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
    friend struct NonConstSetIterator<T, Cmp, It>;

    It mIterator;
};

template <typename T, typename Cmp>
struct NonConstSet : std::set<T, Cmp> {

    using Base = std::set<T, Cmp>;

    using _traits = container_traits<Base>;

    typedef NonConstSetIterator<T, Cmp, typename std::set<T, Cmp>::iterator> iterator;
    typedef NonConstSetConstIterator<T, Cmp, typename std::set<T, Cmp>::const_iterator> const_iterator;
    typedef NonConstSetIterator<T, Cmp, typename std::set<T, Cmp>::reverse_iterator> reverse_iterator;
    typedef NonConstSetConstIterator<T, Cmp, typename std::set<T, Cmp>::const_reverse_iterator> const_reverse_iterator;

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
struct underlying_container<NonConstSet<T, Cmp>> {
    typedef std::set<T, Cmp> type;
};

template <typename T, typename Cmp>
struct container_traits<NonConstSet<T, Cmp>> : container_traits<std::set<T, Cmp>> {

    typedef NonConstSet<T, Cmp> container;
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

}
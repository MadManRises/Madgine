#pragma once

#include "keyvalue.h"

namespace Engine {

template <typename T, typename Cmp, typename It>
class SetConstIterator;

template <typename T, typename Cmp, typename It>
class SetIterator {
public:
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

    template <typename It2>
    SetIterator(const SetIterator<T, Cmp, It2> &other)
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

    bool operator!=(const SetIterator<T, Cmp, It> &other) const
    {
        return mIterator != other.mIterator;
    }

    bool operator==(const SetIterator<T, Cmp, It> &other) const
    {
        return mIterator == other.mIterator;
    }

    SetIterator<T, Cmp, It> &
    operator++()
    {
        ++mIterator;
        return *this;
    }

    SetIterator<T, Cmp, It> &operator--()
    {
        --mIterator;
        return *this;
    }

    operator const It &() const
    {
        return mIterator;
    }

private:
    friend class SetConstIterator<T, Cmp, It>;

    It mIterator;
};

template <class T, typename Cmp, typename It>
class SetConstIterator {
public:
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

    SetConstIterator(const SetIterator<T, Cmp, It> &it)
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

    bool operator!=(const SetConstIterator<T, Cmp, It> &other) const
    {
        return mIterator != other.mIterator;
    }

    bool operator==(const SetConstIterator<T, Cmp, It> &other) const
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
    friend class SetIterator<T, Cmp, It>;

    It mIterator;
};

template <typename T, typename Cmp = KeyCompare<T>>
struct KeyValueSet : std::set<T, Cmp> {

    using _traits = container_traits<std::set<T, Cmp>>;

    typedef KeyValueSet<T, Cmp> container;
    typedef SetIterator<T, Cmp, typename std::set<T, Cmp>::iterator> iterator;
    typedef SetConstIterator<T, Cmp, typename std::set<T, Cmp>::const_iterator> const_iterator;
    typedef SetIterator<T, Cmp, typename std::set<T, Cmp>::reverse_iterator> reverse_iterator;
    typedef SetConstIterator<T, Cmp, typename std::set<T, Cmp>::const_reverse_iterator> const_reverse_iterator;

    struct traits : _traits {

        typedef typename container::iterator iterator;
        typedef typename container::const_iterator const_iterator;
        typedef typename container::reverse_iterator reverse_iterator;
        typedef typename container::const_reverse_iterator const_reverse_iterator;

        typedef iterator position_handle;
        typedef const_iterator const_position_handle;
        typedef cmp_type_t<T, Cmp> key_type;
        typedef typename container::value_type value_type;

        template <template <typename> typename M>
        using rebind = container_traits<KeyValueSet<M<T>, Cmp>>;

        template <class... _Ty>
        static std::pair<iterator, bool> emplace(container &c, const const_iterator &where, _Ty &&... args)
        {
            return c.emplace(std::forward<_Ty>(args)...);
        }

        static position_handle toPositionHandle(container &c, const iterator &it)
        {
            return it;
        }

        static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
        {
        }

        static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const const_iterator &it, size_t count = 1)
        {
        }

        static iterator toIterator(container &c, const position_handle &handle)
        {
            return handle;
        }

        static const_iterator toIterator(const container &c, const const_position_handle &handle)
        {
            return handle;
        }

        static position_handle next(const position_handle &handle)
        {
            return std::next(handle);
        }
    };
};

}
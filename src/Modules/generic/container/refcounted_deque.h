#pragma once

namespace Engine {

struct MODULES_EXPORT ControlBlockBase {
    ControlBlockBase();

    void incRef();

    void decRef();

    bool dead() const;

protected:
    bool mDeadFlag : 1;
    uint32_t mRefCount : 31;
};

template <typename T>
struct ControlBlock : ControlBlockBase {

    template <typename... Args>
    ControlBlock(Args &&... args)
    {
        new (&mData) T(std::forward<Args>(args)...);
    }

    ControlBlock(ControlBlock<T> &&other)
    {
        throw 0;
    }

    ControlBlock &operator=(ControlBlock &&other)
    {
        throw 0;
    }

    ~ControlBlock()
    {
        if (!mDeadFlag)
            destroy();
    }

    void destroy()
    {
        assert(!mDeadFlag);
        mDeadFlag = true;
        mData.~T();
    }

    T *get()
    {
        return &mData;
    }

    const T *get() const
    {
        return &mData;
    }

    operator T &()
    {
        return *get();
    }

    operator const T &() const
    {
        return *get();
    }

private:
    union {
        T mData;
    };
    
};

template <typename T>
struct refcounted_deque {

    using value_type = T;

    template <typename It, typename Val>
    struct IteratorImpl {

        using iterator_category = typename It::iterator_category;

        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = Val *;
        using reference = Val &;

        IteratorImpl() = default;

        IteratorImpl(It it, const std::deque<ControlBlock<T>> &cont)
            : mIt(std::move(it))
            , mContainer(&cont)
        {
        }

        template <typename It2, typename Val2>
        friend struct IteratorImpl;

        template <typename It2, typename Val2>
        IteratorImpl(const IteratorImpl<It2, Val2> &other)
            : mIt(other.mIt)
            , mContainer(other.mContainer)
        {
        }

        reference operator*() const
        {
            update();
            return *mIt;
        }

        pointer operator->() const
        {
            update();
            return &*mIt;
        }

        decltype(auto) get_refcounted() const
        {
            return *mIt;
        }

        difference_type operator-(const IteratorImpl &other) const
        {
            return mIt - other.mIt;
        }

        IteratorImpl operator+(difference_type diff) const
        {
            return { mIt + diff, *mContainer };
        }

        IteratorImpl &operator+=(difference_type diff)
        {
            mIt += diff;
            return *this;
        }

        bool operator!=(const IteratorImpl &other) const
        {
            assert(mContainer == other.mContainer);
            return mIt != other.mIt;
        }

        bool operator==(const IteratorImpl &other) const
        {
            assert(mContainer == other.mContainer);
            return mIt == other.mIt;
        }

        void operator++()
        {
            ++mIt;            
        }

        void operator--()
        {
            --mIt;            
            updateBack();
        }

        void update() const
        {
            while (mIt != mContainer->end() && mIt->dead()) {
                ++mIt;
            }
        }

        void updateBack()
        {
            while (mIt->dead()) {
                --mIt;
            }
        }

        const It &it() const
        {
            return mIt;
        }

    private:
        mutable It mIt;
        const std::deque<ControlBlock<T>> *mContainer = nullptr;
    };

    using iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::iterator, T>;
    using reverse_iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::reverse_iterator, T>;
    using const_iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::const_iterator, const T>;
    using const_reverse_iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::const_reverse_iterator, const T>;

    iterator begin()
    {
        return { mData.begin(), mData };
    }

    const_iterator begin() const
    {
        return { mData.begin(), mData };
    }

    iterator end()
    {
        return { mData.end(), mData };
    }

    const_iterator end() const
    {
        return { mData.end(), mData };
    }

    using refcounted_iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::iterator, ControlBlock<T>>;
    using refcounted_reverse_iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::reverse_iterator, ControlBlock<T>>;
    using refcounted_const_iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::const_iterator, const ControlBlock<T>>;
    using refcounted_const_reverse_iterator = IteratorImpl<typename std::deque<ControlBlock<T>>::const_reverse_iterator, const ControlBlock<T>>;

    refcounted_iterator refcounted_begin()
    {
        return { mData.begin(), mData };
    }

    refcounted_const_iterator refcounted_begin() const
    {
        return { mData.begin(), mData };
    }

    refcounted_iterator refcounted_end()
    {
        return { mData.end(), mData };
    }

    refcounted_const_iterator refcounted_end() const
    {
        return { mData.end(), mData };
    }

    void clear()
    {
        for (ControlBlock<T> &ref : refcounted()) {
            ref.destroy();
        }
        mSize = 0;
    }

    template <typename... Args>
    iterator emplace(const const_iterator &where, Args &&... args)
    {
        ++mSize;
        auto it = mData.emplace(where.it(), std::forward<Args>(args)...);
        return { it, mData };
    }

    iterator erase(const iterator &where)
    {
        --mSize;
        where.get_refcounted().destroy();
        return { where.it(), mData };
    }

    template <typename... Args>
    T &emplace_back(Args &&... args)
    {
        return *emplace(end(), std::forward<Args>(args)...);
    }

    size_t size() const
    {
        return mSize;
    }

    size_t internal_size() const
    {
        return mData.size();
    }

    struct RefcountedView {
        refcounted_deque<T> &mData;
        refcounted_iterator begin() const
        {
            return mData.refcounted_begin();
        }
        refcounted_iterator end() const
        {
            return mData.refcounted_end();
        }
        size_t size() const
        {
            return mData.size();
        }
    };

    RefcountedView refcounted()
    {
        return { *this };
    }

private:
    std::deque<ControlBlock<T>> mData;
    size_t mSize = 0;
};

template <typename T>
struct container_traits<refcounted_deque<T>, void> {
    static constexpr const bool sorted = false;
    static constexpr const bool has_dependent_handle = false;
    static constexpr const bool remove_invalidates_handles = false;
    static constexpr const bool is_fixed_size = false;

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

    typedef refcounted_deque<T> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;
    typedef T *handle;
    typedef const T *const_handle;
    typedef handle_t position_handle;
    typedef handle_t const_position_handle;
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

    static handle toHandle(container &c, const iterator &it)
    {
        if (it == c.end())
            return {};
        return std::distance(c.begin(), it);
    }

    static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
    {
        if (std::next(it) == c.end() && handle == c.internal_size() - 1) {
            ++handle;
        }
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const const_iterator &it, bool wasIn, size_t count = 1)
    {
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return c.begin() + handle;
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return c.begin() + handle;
    }

    static position_handle next(const position_handle &handle)
    {
        return handle + 1;
    }
};

template <typename C, typename T>
struct container_api_impl<C, refcounted_deque<T>> : C {

    using C::C;
};

}
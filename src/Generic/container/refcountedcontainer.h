#pragma once

namespace Engine {

template <typename C>
struct RefcountedContainer {

    using value_type = typename C::value_type;

    struct ControlBlock;

    using internal_container_type = typename container_traits<C>::template rebind_t<ControlBlock>;
    using internal_traits = container_traits<internal_container_type>;
    using position_handle = typename internal_traits::position_handle;

    struct ControlBlock {

        template <typename... Args>
        ControlBlock(Args &&...args)
        {
            new (&mData) value_type(std::forward<Args>(args)...);
        }

        ControlBlock(ControlBlock &&other)
        {
            throw 0;
        }

        ControlBlock &operator=(ControlBlock &&other)
        {
            throw 0;
        }

        ~ControlBlock()
        {
            if (!mDeadFlag) {
                mData.~value_type();
                mDeadFlag = true;
            }
        }

        value_type *get()
        {
            return &mData;
        }

        const value_type *get() const
        {
            return &mData;
        }

        operator value_type &()
        {
            return *get();
        }

        operator const value_type &() const
        {
            return *get();
        }

        static ControlBlock *fromPtr(value_type *ptr)
        {
            if (!ptr)
                return nullptr;
            return reinterpret_cast<ControlBlock *>(ptr);
        }

        void incRef();
        void decRef();

        bool dead() const
        {
            return mDeadFlag;
        }

    private:
        union {
            value_type mData;
            //struct {
            void *mHead; //position_handle *
            //position_handle mHandle;
            //}
        };
        bool mDeadFlag : 1 = false;
        uint32_t mRefCount : 31 = 0;

        friend struct RefcountedContainer<C>;
    };

    static_assert(sizeof(position_handle) + sizeof(void *) <= sizeof(value_type));

    private:
        static position_handle &handle(ControlBlock &block)
        {
            assert(block.mDeadFlag);
            return *reinterpret_cast<position_handle *>(&block.mHead + 1);
        }

        template <typename... Args>
        static position_handle emplace(ControlBlock &block, Args &&...args)
        {
            assert(block.mDeadFlag && block.mRefCount == 0);
            position_handle next = handle(block);
            handle(block).~position_handle();
            block.mDeadFlag = false;
            new (&block.mData) value_type(std::forward<Args>(args)...);
            return next;
        }

        static void destroy(ControlBlock &block, position_handle *head, const position_handle &selfIt)
        {
            assert(!block.mDeadFlag);
            block.mData.~value_type();
            block.mDeadFlag = true;
            block.mHead = head;
            if (block.mRefCount == 0) {
                new (&handle(block)) position_handle(*head);
                *head = selfIt;
            } else {
                new (&handle(block)) position_handle(selfIt);
            }
        }

        public:

    template <typename It, typename Val, bool refcounted>
    struct IteratorImpl {

        using iterator_category = typename It::iterator_category;

        using value_type = value_type;
        using difference_type = ptrdiff_t;
        using pointer = Val *;
        using reference = Val &;

        IteratorImpl() = default;

        IteratorImpl(It it, const internal_container_type &cont)
            : mIt(std::move(it))
            , mContainer(&cont)
        {
            if constexpr (!refcounted)
                update();
        }

        template <typename It2, typename Val2, bool>
        friend struct IteratorImpl;

        template <typename It2, typename Val2>
        IteratorImpl(const IteratorImpl<It2, Val2, refcounted> &other)
            : mIt(other.mIt)
            , mContainer(other.mContainer)
        {
        }

        template <typename It2, typename Val2>
        IteratorImpl(const Pib<IteratorImpl<It2, Val2, refcounted>> &other)
            : IteratorImpl(other.first)
        {
        }

        reference operator*() const
        {
            return *mIt;
        }

        pointer operator->() const
        {
            return &*mIt;
        }

        decltype(auto) get_block() const
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

        IteratorImpl &operator++()
        {
            ++mIt;
            if constexpr (!refcounted)
                update();
            return *this;
        }

        IteratorImpl &operator--()
        {
            --mIt;
            if constexpr (!refcounted)
                updateBack();
            return *this;
        }

        void update()
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
        It mIt;
        const internal_container_type *mContainer = nullptr;
    };

    using iterator = IteratorImpl<typename internal_traits::iterator, value_type, false>;
    using reverse_iterator = IteratorImpl<typename internal_traits::reverse_iterator, value_type, false>;
    using const_iterator = IteratorImpl<typename internal_traits::const_iterator, const value_type, false>;
    using const_reverse_iterator = IteratorImpl<typename internal_traits::const_reverse_iterator, const value_type, false>;

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

    using refcounted_iterator = IteratorImpl<typename internal_container_type::iterator, ControlBlock, true>;
    using refcounted_reverse_iterator = IteratorImpl<typename internal_container_type::reverse_iterator, ControlBlock, true>;
    using refcounted_const_iterator = IteratorImpl<typename internal_container_type::const_iterator, const ControlBlock, true>;
    using refcounted_const_reverse_iterator = IteratorImpl<typename internal_container_type::const_reverse_iterator, const ControlBlock, true>;

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

    using blocks_iterator = IteratorImpl<typename internal_container_type::iterator, ControlBlock, false>;
    using blocks_reverse_iterator = IteratorImpl<typename internal_container_type::reverse_iterator, ControlBlock, false>;
    using blocks_const_iterator = IteratorImpl<typename internal_container_type::const_iterator, const ControlBlock, false>;
    using blocks_const_reverse_iterator = IteratorImpl<typename internal_container_type::const_reverse_iterator, const ControlBlock, false>;

    blocks_iterator blocks_begin()
    {
        return { mData.begin(), mData };
    }

    blocks_const_iterator blocks_begin() const
    {
        return { mData.begin(), mData };
    }

    blocks_iterator blocks_end()
    {
        return { mData.end(), mData };
    }

    blocks_const_iterator blocks_end() const
    {
        return { mData.end(), mData };
    }

    void clear()
    {
        for (auto it = blocks_begin(); it != blocks().end(); ++it) {
            destroy(*it, &mFreeListHead, internal_traits::toPositionHandle(mData, it.it()));
        }
        mSize = 0;
    }

    template <typename... Args>
    Pib<iterator> emplace(const const_iterator &where, Args &&...args)
    {
        typename internal_traits::iterator freeListIterator = internal_traits::toIterator(mData, mFreeListHead);

        assert(where.it() == mData.end() || where.it() == freeListIterator);
        ++mSize;
        typename internal_traits::emplace_return it;
        if (freeListIterator == mData.end()) {
            it = internal_traits::emplace(mData, where.it(), std::forward<Args>(args)...);
            mFreeListHead = internal_traits::toPositionHandle(mData, std::next(it));
        } else {
            it = freeListIterator;
            mFreeListHead = emplace(*freeListIterator, std::forward<Args>(args)...);
        }
        return { { it, mData }, internal_traits::was_emplace_successful(it) };
    }

    iterator erase(const iterator &where)
    {
        --mSize;
        destroy(where.get_block(), &mFreeListHead, internal_traits::toPositionHandle(mData, where.it()));

        return { where.it(), mData };
    }

    template <typename... Args>
    value_type &emplace_back(Args &&...args)
    {
        return *emplace(end(), std::forward<Args>(args)...);
    }

    size_t size() const
    {
        return mSize;
    }

    size_t blocks_size() const
    {
        return mData.size();
    }

    struct RefcountedView {
        RefcountedContainer<C> &mData;
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
        constexpr bool isReference() const
        {
            return true;
        }
    };

    RefcountedView refcounted()
    {
        return { *this };
    }

    struct BlocksView {
        RefcountedContainer<C> &mData;
        blocks_iterator begin() const
        {
            return mData.blocks_begin();
        }
        blocks_iterator end() const
        {
            return mData.blocks_end();
        }
        size_t size() const
        {
            return mData.size();
        }
        constexpr bool isReference() const
        {
            return true;
        }
    };

    BlocksView blocks()
    {
        return { *this };
    }

private:
    friend struct container_traits<RefcountedContainer<C>, void>;

    internal_container_type mData;
    typename internal_traits::position_handle mFreeListHead = internal_traits::toPositionHandle(mData, mData.end());
    size_t mSize = 0;
};

template <typename C>
    void RefcountedContainer<C>::ControlBlock::incRef()
{
    ++mRefCount;
}

    template <typename C>
    void RefcountedContainer<C>::ControlBlock::decRef()
    {
            --mRefCount;
            if (mRefCount == 0 && mDeadFlag) {
                std::swap(*static_cast<position_handle*>(mHead), handle(*this));
            }
        }

template <typename C>
struct container_traits<RefcountedContainer<C>, void> : RefcountedContainer<C>::internal_traits {

    typedef typename C::value_type value_type;

    typedef RefcountedContainer<C> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;

    typedef Pib<iterator> emplace_return;

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&...args)
    {
        return c.emplace(where, std::forward<_Ty>(args)...);
    }

    static bool was_emplace_successful(const emplace_return &ret)
    {
        return ret.success();
    }

    static typename RefcountedContainer<C>::internal_traits::position_handle toPositionHandle(container &c, const iterator &it)
    {
        return RefcountedContainer<C>::internal_traits::toPositionHandle(c.mData, it.it());
    }

    static typename RefcountedContainer<C>::internal_traits::handle toHandle(container &c, const iterator &it)
    {
        return RefcountedContainer<C>::internal_traits::toHandle(c.mData, it.it());
    }

    static void revalidateHandleAfterInsert(typename RefcountedContainer<C>::internal_traits::position_handle &handle, const container &c, const const_iterator &it)
    {
        if (toIterator(c, handle) == it)
            RefcountedContainer<C>::internal_traits::revalidateHandleAfterInsert(handle, c.mData, it.it());
    }

    static void revalidateHandleAfterRemove(typename RefcountedContainer<C>::internal_traits::position_handle &handle, const container &c, const const_iterator &it, bool wasIn, size_t count = 1)
    {
        /*size_t pivot = std::distance(c.begin(), it);
        if (pivot != handle)
            RefcountedContainer<C>::internal_traits::revalidateHandleAfterRemove(handle, c.mData, it.it(), wasIn, count);*/
    }

    static iterator toIterator(container &c, const typename RefcountedContainer<C>::internal_traits::position_handle &handle)
    {
        return { RefcountedContainer<C>::internal_traits::toIterator(c.mData, handle), c.mData };
    }

    static const_iterator toIterator(const container &c, const typename RefcountedContainer<C>::internal_traits::const_position_handle &handle)
    {
        return { RefcountedContainer<C>::internal_traits::toIterator(c.mData, handle), c.mData };
    }

    static typename RefcountedContainer<C>::internal_traits::position_handle next(container &c, const typename RefcountedContainer<C>::internal_traits::position_handle &handle)
    {
        return toPositionHandle(c, ++toIterator(c, handle));
    }

    static typename RefcountedContainer<C>::internal_traits::position_handle prev(container &c, const typename RefcountedContainer<C>::internal_traits::position_handle &handle)
    {
        return toPositionHandle(c, --toIterator(c, handle));
    }
};

template <typename C, typename Inner>
struct container_api_impl<C, RefcountedContainer<Inner>> : C {

    using C::C;
};

}
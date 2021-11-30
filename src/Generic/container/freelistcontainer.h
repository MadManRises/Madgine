#pragma once

namespace Engine {

DERIVE_FUNCTION(construct);
DERIVE_FUNCTION(destruct);

template <typename C, typename DataTraits = typename C::value_type>
struct FreeListContainer {

    using value_type = typename C::value_type;

    using internal_traits = container_traits<C>;
    using position_handle = typename internal_traits::position_handle;

    using reference = typename C::reference;
    using const_reference = typename C::const_reference;

    template <typename It, typename type, bool skipping>
    struct IteratorImpl {

        using iterator_category = typename It::iterator_category;

        using value_type = value_type;
        using difference_type = ptrdiff_t;
        using pointer = typename It::pointer;
        using reference = typename It::reference;

        IteratorImpl() = default;

        IteratorImpl(It it, const C &cont)
            : mIt(std::move(it))
            , mContainer(&cont)
        {
            if constexpr (skipping)
                update();
        }

        template <typename It2, typename type2, bool>
        friend struct IteratorImpl;

        template <typename It2, typename type2, bool skipping2>
        IteratorImpl(const IteratorImpl<It2, type2, skipping2> &other)
            : mIt(other.mIt)
            , mContainer(other.mContainer)
        {
        }

        template <typename It2, typename type2>
        IteratorImpl(const Pib<IteratorImpl<It2, type2, skipping>> &other)
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

        template <typename It2, typename type2>
        difference_type operator-(const IteratorImpl<It2, type2, skipping> &other) const
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
            if constexpr (skipping)
                update();
            return *this;
        }

        IteratorImpl &operator--()
        {
            --mIt;
            if constexpr (skipping)
                updateBack();
            return *this;
        }

        void update()
        {
            while (mIt != mContainer->end() && DataTraits::isFree(*mIt)) {
                ++mIt;
            }
        }

        void updateBack()
        {
            while (DataTraits::isFree(*mIt)) {
                --mIt;
            }
        }

        const It &it() const
        {
            return mIt;
        }

        const C& container() const {
            return *mContainer;
        }

    private:
        It mIt;
        const C *mContainer = nullptr;
    };

    using iterator = IteratorImpl<typename internal_traits::iterator, value_type, true>;
    using reverse_iterator = IteratorImpl<typename internal_traits::reverse_iterator, value_type, true>;
    using const_iterator = IteratorImpl<typename internal_traits::const_iterator, const value_type, true>;
    using const_reverse_iterator = IteratorImpl<typename internal_traits::const_reverse_iterator, const value_type, true>;

    iterator begin()
    {
        return { mContainer.begin(), mContainer };
    }

    const_iterator begin() const
    {
        return { mContainer.begin(), mContainer };
    }

    iterator end()
    {
        return { mContainer.end(), mContainer };
    }

    const_iterator end() const
    {
        return { mContainer.end(), mContainer };
    }

    using nodes_iterator = IteratorImpl<typename internal_traits::iterator, value_type, false>;
    using nodes_reverse_iterator = IteratorImpl<typename internal_traits::reverse_iterator, value_type, false>;
    using nodes_const_iterator = IteratorImpl<typename internal_traits::const_iterator, const value_type, false>;
    using nodes_const_reverse_iterator = IteratorImpl<typename internal_traits::const_reverse_iterator, const value_type, false>;

    nodes_iterator nodes_begin()
    {
        return { mContainer.begin(), mContainer };
    }

    nodes_const_iterator nodes_begin() const
    {
        return { mContainer.begin(), mContainer };
    }

    nodes_iterator nodes_end()
    {
        return { mContainer.end(), mContainer };
    }

    nodes_const_iterator nodes_end() const
    {
        return { mContainer.end(), mContainer };
    }

private:
    static position_handle &handle(reference block)
    {
        assert(DataTraits::isFree(block));
        return *reinterpret_cast<position_handle *>(DataTraits::getLocation(block));
    }

    template <typename... Args>
    position_handle emplace(reference block, Args &&...args)
    {
        assert(DataTraits::isFree(block));
        position_handle next = handle(block);
        handle(block).~position_handle();
        if constexpr (has_function_construct_v<C, reference, Args &&...>)
            mContainer.construct(block, std::forward<Args>(args)...);
        else
            new (&block) value_type(std::forward<Args>(args)...);
        return next;
    }

    void destroy(reference block, const position_handle &head)
    {
        assert(!DataTraits::isFree(block));
        if constexpr (has_function_destruct_v<C, reference>)
            mContainer.destruct(block);
        else
            block.~value_type();
        new (&handle(block)) position_handle(head);
    }

public:
    template <typename... Args>
    Pib<iterator> emplace(const const_iterator &where, Args &&...args)
    {
        typename internal_traits::iterator freeListIterator = internal_traits::toIterator(mContainer, mFreeListHead);

        assert(where.it() == mContainer.end() || where.it() == freeListIterator);
        ++mSize;
        typename internal_traits::emplace_return it;
        if (freeListIterator == mContainer.end()) {
            it = internal_traits::emplace(mContainer, where.it(), std::forward<Args>(args)...);
            mFreeListHead = internal_traits::toPositionHandle(mContainer, mContainer.end());
        } else {
            it = freeListIterator;
            mFreeListHead = emplace(*freeListIterator, std::forward<Args>(args)...);
        }
        return { { it, mContainer }, internal_traits::was_emplace_successful(it) };
    }

    void clear()
    {
        for (auto it = begin(); it != end(); ++it) {
            destroy(*it, mFreeListHead);
            mFreeListHead = internal_traits::toPositionHandle(mContainer, it.it());
        }
        mSize = 0;
    }

    iterator erase(const iterator &where)
    {
        --mSize;
        destroy(*where, mFreeListHead);
        mFreeListHead = internal_traits::toPositionHandle(mContainer, where.it());

        return { where.it(), mContainer };
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

    size_t nodes_size() const
    {
        return mContainer.size();
    }

    reference at(size_t i)
    {
        return mContainer.at(i);
    }

    const_reference at(size_t i) const
    {
        return mContainer.at(i);
    }

    bool empty() const
    {
        return mSize == 0;
    }

private:
    friend struct container_traits<FreeListContainer<C, DataTraits>, void>;

    C mContainer;
    position_handle mFreeListHead = internal_traits::toPositionHandle(mContainer, mContainer.end());
    size_t mSize = 0;
};

template <typename C, typename DataTraits>
struct underlying_container<FreeListContainer<C, DataTraits>> {
    typedef C type;
};

template <typename C, typename DataTraits>
struct container_traits<FreeListContainer<C, DataTraits>, void> : FreeListContainer<C, DataTraits>::internal_traits {

    typedef typename C::value_type value_type;

    typedef FreeListContainer<C, DataTraits> container;
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

    static typename container::internal_traits::position_handle toPositionHandle(container &c, const iterator &it)
    {
        return container::internal_traits::toPositionHandle(c.mContainer, it.it());
    }

    static typename container::internal_traits::handle toHandle(container &c, const iterator &it)
    {
        return container::internal_traits::toHandle(c.mContainer, it.it());
    }

    static void revalidateHandleAfterInsert(typename container::internal_traits::position_handle &handle, const container &c, const const_iterator &it)
    {
        if (toIterator(c, handle) == it)
            container::internal_traits::revalidateHandleAfterInsert(handle, c.mContainer, it.it());
    }

    static void revalidateHandleAfterRemove(typename container::internal_traits::position_handle &handle, const container &c, const const_iterator &it, bool wasIn, size_t count = 1)
    {
        /*size_t pivot = std::distance(c.begin(), it);
        if (pivot != handle)
            RefcountedContainer<C>::internal_traits::revalidateHandleAfterRemove(handle, c.mData, it.it(), wasIn, count);*/
    }

    static iterator toIterator(container &c, const typename container::internal_traits::position_handle &handle)
    {
        return { container::internal_traits::toIterator(c.mContainer, handle), c.mContainer };
    }

    static const_iterator toIterator(const container &c, const typename container::internal_traits::const_position_handle &handle)
    {
        return { container::internal_traits::toIterator(c.mContainer, handle), c.mContainer };
    }

    static typename container::internal_traits::position_handle next(container &c, const typename container::internal_traits::position_handle &handle)
    {
        return toPositionHandle(c, ++toIterator(c, handle));
    }

    static typename container::internal_traits::position_handle prev(container &c, const typename container::internal_traits::position_handle &handle)
    {
        return toPositionHandle(c, --toIterator(c, handle));
    }
};

}
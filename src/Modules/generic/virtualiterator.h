#pragma once

#include "proxy.h"

#include "iterator_traits.h"

namespace Engine {

struct VirtualIteratorEnd {
};

template <typename RefT>
struct VirtualIteratorBase {
    virtual ~VirtualIteratorBase() = default;
    virtual bool ended() const = 0;
    virtual void increment() = 0;
    virtual RefT get() const = 0;
    virtual std::unique_ptr<VirtualIteratorBase<RefT>> clone() const = 0;
    virtual bool equals(const VirtualIteratorBase<RefT> &) const = 0;
};

template <typename RefT, typename It>
struct NonOwningVirtualIteratorImpl : VirtualIteratorBase<RefT> {

    NonOwningVirtualIteratorImpl(It begin, It end)
        : mIt(std::move(begin))
        , mEnd(std::move(end))
    {
    }

    virtual bool ended() const override
    {
        return mIt == mEnd;
    }

    virtual void increment() override
    {
        ++mIt;
    }

    virtual RefT get() const override
    {
        return RefT { *mIt };
    }

    virtual std::unique_ptr<VirtualIteratorBase<RefT>> clone() const override
    {
        return std::make_unique<NonOwningVirtualIteratorImpl<RefT, It>>(mIt, mEnd);
    }

    virtual bool equals(const VirtualIteratorBase<RefT> &other) const override
    {
        const NonOwningVirtualIteratorImpl<RefT, It> *otherP = dynamic_cast<const NonOwningVirtualIteratorImpl<RefT, It> *>(&other);
        return otherP && mIt == otherP->mIt;
    }

private:
    It mIt, mEnd;
};

template <typename RefT, typename It, typename T>
struct OwningVirtualIteratorImpl : VirtualIteratorBase<RefT> {

    OwningVirtualIteratorImpl(T &&t)
        : mT(std::move(t))
        , mIt(mT.begin())
        , mEnd(mT.end())
    {
    }

    OwningVirtualIteratorImpl(const T &t, const It &it, const It &end)
        : mT(t)
        , mIt(std::next(mT.begin(), std::distance(It { const_cast<T&>(t).begin() }, it)))
        , mEnd(std::next(mT.begin(), std::distance(It { const_cast<T &>(t).begin() }, end)))
    {
    }

    virtual bool ended() const override
    {
        return mIt == mEnd;
    }

    virtual void increment() override
    {
        ++mIt;
    }

    virtual RefT get() const override
    {
        return RefT { *mIt };
    }

    virtual std::unique_ptr<VirtualIteratorBase<RefT>> clone() const override
    {
        return std::make_unique<OwningVirtualIteratorImpl<RefT, It, T>>(mT, mIt, mEnd);
    }

    virtual bool equals(const VirtualIteratorBase<RefT> &other) const override
    {
        const OwningVirtualIteratorImpl<RefT, It, T> *otherP = dynamic_cast<const OwningVirtualIteratorImpl<RefT, It, T> *>(&other);
        return otherP && mIt == otherP->mIt;
    }

private:
    T mT;
    It mIt, mEnd;
};

template <typename RefT>
struct VirtualIterator {
    VirtualIterator() = default;

    template <typename It>
    VirtualIterator(It &&begin, It &&end)
        : mImpl(std::make_unique<NonOwningVirtualIteratorImpl<RefT, std::remove_reference_t<It>>>(std::forward<It>(begin), std::forward<It>(end)))
    {
    }

    template <typename It, typename T>
    static std::unique_ptr<VirtualIteratorBase<RefT>> implHelper(T &&t)
    {
        if constexpr (std::is_reference_v<T>) {
            return std::make_unique<NonOwningVirtualIteratorImpl<RefT, It>>(t.begin(), t.end());
        } else {
            return std::make_unique<OwningVirtualIteratorImpl<RefT, It, T>>(std::move(t));
        }
    }

    template <typename T, typename = std::enable_if_t<is_iterable_v<T>>, typename It = typename derive_iterator<T>::iterator>
    explicit VirtualIterator(T &&t, type_holder_t<It> = {})
        : mImpl(implHelper<It>(std::forward<T>(t)))
    {
    }

    VirtualIterator(const VirtualIterator<RefT> &other)
        : mImpl(other.mImpl->clone())
    {
    }

    VirtualIterator(VirtualIterator<RefT> &&other)
        : mImpl(std::move(other.mImpl))
    {
    }

    void operator=(const VirtualIterator<RefT> &other)
    {
        mImpl = other.mImpl->clone();
    }

    void operator=(VirtualIterator<RefT> &&other)
    {
        mImpl = std::move(other.mImpl);
    }

    void operator++()
    {
        mImpl->increment();
    }

    RefT operator*() const
    {
        return mImpl->get();
    }

    Proxy<RefT, false> operator->() const
    {
        return mImpl->get();
    }

    bool operator!=(const VirtualIteratorEnd &) const
    {
        return !mImpl->ended();
    }

    bool operator==(const VirtualIterator<RefT> &other) const
    {
        return mImpl->equals(*other.mImpl);
    }

private:
    std::unique_ptr<VirtualIteratorBase<RefT>> mImpl;
};

template <typename RefT>
struct VirtualRange {

    template <typename C>
    VirtualRange(C &c)
        : mRange(c.begin(), c.end())
    {
    }

    VirtualIterator<RefT> begin()
    {
        return mRange;
    }

    VirtualIteratorEnd end()
    {
        return {};
    }

private:
    VirtualIterator<RefT> mRange;
};
}
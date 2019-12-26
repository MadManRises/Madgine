#pragma once

namespace Engine {

/*template <typename It>
using IteratorValueType = decltype(*std::declval<It>());*/

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
struct VirtualIteratorImpl : VirtualIteratorBase<RefT> {

    VirtualIteratorImpl(It begin, It end)
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
        return RefT{*mIt};
    }

    virtual std::unique_ptr<VirtualIteratorBase<RefT>> clone() const override
    {
        return std::make_unique<VirtualIteratorImpl<RefT, It>>(mIt, mEnd);
    }

	virtual bool equals(const VirtualIteratorBase<RefT> &other) const override
	{
            const VirtualIteratorImpl<RefT, It> *otherP = dynamic_cast<const VirtualIteratorImpl<RefT, It> *>(&other);
                return otherP && mIt == otherP->mIt;
	}

private:
    It mIt, mEnd;
};

template <typename RefT>
struct VirtualIterator {
    VirtualIterator() = default;

    template <typename It>
    VirtualIterator(It &&begin, It &&end)
        : mImpl(std::make_unique<VirtualIteratorImpl<RefT, std::remove_reference_t<It>>>(std::forward<It>(begin), std::forward<It>(end)))
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
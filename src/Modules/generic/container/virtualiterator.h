#pragma once

#include "../proxy.h"

#include "../defaultassign.h"

namespace Engine {

template <typename RefT>
struct VirtualRangeBase;

template <typename RefT>
struct VirtualIteratorBase {
    virtual ~VirtualIteratorBase() = default;
    virtual void increment() = 0;
    virtual void get(RefT &ref) const = 0;
    virtual std::unique_ptr<VirtualIteratorBase<RefT>> clone() const = 0;
    virtual bool equals(const VirtualIteratorBase<RefT> &) const = 0;

    const std::shared_ptr<VirtualRangeBase<RefT>> &container() const
    {
        return mContainer;
    }

protected:
    VirtualIteratorBase(const std::shared_ptr<VirtualRangeBase<RefT>> &container)
        : mContainer(container)
    {
    }

    std::shared_ptr<VirtualRangeBase<RefT>> mContainer;
};

template <typename RefT, typename It, typename Assign>
struct VirtualIteratorImpl : VirtualIteratorBase<RefT> {

    template <typename It2>
    VirtualIteratorImpl(It2 &&it, const std::shared_ptr<VirtualRangeBase<RefT>> &container)
        : VirtualIteratorBase<RefT>(container),
        mIt(std::forward<It2>(it))        
    {
    }

    virtual void increment() override
    {
        ++mIt;
    }

    virtual void get(RefT &ref) const override
    {
        Assign {}(ref, *mIt);
    }

    virtual std::unique_ptr<VirtualIteratorBase<RefT>> clone() const override
    {
        return std::make_unique<VirtualIteratorImpl<RefT, It, Assign>>(mIt, this->mContainer);
    }

    virtual bool equals(const VirtualIteratorBase<RefT> &other) const override
    {
        assert(this->mContainer == other.container());
        const VirtualIteratorImpl<RefT, It, Assign> *otherP = static_cast<const VirtualIteratorImpl<RefT, It, Assign> *>(&other);
        return mIt == otherP->mIt;
    }

private:
    It mIt;
};

template <typename RefT>
struct VirtualIterator {

    using iterator_category = std::input_iterator_tag;
    using value_type = RefT;
    using difference_type = ptrdiff_t;
    using pointer = void;
    using reference = RefT;

    template <typename It, typename Assign = DefaultAssign>
    VirtualIterator(It &&it, const std::shared_ptr<VirtualRangeBase<RefT>> &container, type_holder_t<Assign> = {})
        : mImpl(std::make_unique<VirtualIteratorImpl<RefT, std::remove_reference_t<It>, Assign>>(std::forward<It>(it), container))
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
        RefT result;
        mImpl->get(result);
        return result;
    }

    Proxy<RefT, false> operator->() const
    {
        RefT result;
        mImpl->get(result);
        return result;
    }

    bool operator==(const VirtualIterator<RefT> &other) const
    {
        return mImpl->equals(*other.mImpl);
    }

    bool operator!=(const VirtualIterator<RefT> &other) const
    {
        return !mImpl->equals(*other.mImpl);
    }

private:
    std::unique_ptr<VirtualIteratorBase<RefT>> mImpl;
};

template <typename RefT>
struct VirtualRangeBase {
    virtual ~VirtualRangeBase() = default;
    virtual VirtualIterator<RefT> begin(const std::shared_ptr<VirtualRangeBase<RefT>> &self) = 0;
    virtual VirtualIterator<RefT> end(const std::shared_ptr<VirtualRangeBase<RefT>> &self) = 0;
};

template <typename RefT, typename C, typename Assign>
struct NonOwningVirtualRangeImpl : VirtualRangeBase<RefT> {

    NonOwningVirtualRangeImpl(C &c)
        : mContainer(c)
    {
    }

    virtual VirtualIterator<RefT> begin(const std::shared_ptr<VirtualRangeBase<RefT>> &self) override
    {
        return { mContainer.begin(), self, type_holder<Assign> };
    }

    virtual VirtualIterator<RefT> end(const std::shared_ptr<VirtualRangeBase<RefT>> &self) override
    {
        return { mContainer.end(), self, type_holder<Assign> };
    }

private:
    C &mContainer;
};

template <typename RefT, typename C, typename Assign>
struct OwningVirtualRangeImpl : VirtualRangeBase<RefT> {

    OwningVirtualRangeImpl(C &&c)
        : mContainer(std::move(c))
    {
    }

    virtual VirtualIterator<RefT> begin(const std::shared_ptr<VirtualRangeBase<RefT>> &self) override
    {
        return { mContainer.begin(), self, type_holder<Assign> };
    }

    virtual VirtualIterator<RefT> end(const std::shared_ptr<VirtualRangeBase<RefT>> &self) override
    {
        return { mContainer.end(), self, type_holder<Assign> };
    }

private:
    C mContainer;
};

template <typename RefT>
struct VirtualRange {

    template <typename C, typename Assign = DefaultAssign>
    explicit VirtualRange(C &&c, type_holder_t<Assign> = {})
        : mRange(implHelper<Assign>(std::forward<C>(c)))
    {
    }

    template <typename Assign, typename C>
    static std::shared_ptr<VirtualRangeBase<RefT>> implHelper(C &&c)
    {
        if constexpr (std::is_reference_v<C>) {
            return std::make_shared<NonOwningVirtualRangeImpl<RefT, C, Assign>>(c);
        } else {
            return std::make_shared<OwningVirtualRangeImpl<RefT, C, Assign>>(std::move(c));
        }
    }

    VirtualIterator<RefT> begin() const
    {
        return mRange->begin(mRange);
    }

    VirtualIterator<RefT> end() const
    {
        return mRange->end(mRange);
    }

    bool operator==(const VirtualRange<RefT> &other) const
    {
        throw "TODO";
        return false;
        //return std::equal(begin(), end(), other.begin(), other.end());
    }

private:
    std::shared_ptr<VirtualRangeBase<RefT>> mRange;
};
}
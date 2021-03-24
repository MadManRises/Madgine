#pragma once

#include "../proxy.h"

#include "../defaultassign.h"

#include "../heapobject.h"

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
        : VirtualIteratorBase<RefT>(container)
        , mIt(std::forward<It2>(it))
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
    virtual bool isReference() const = 0;
};

template <typename RefT, typename C>
struct VirtualRangeSecondBase : VirtualRangeBase<RefT> {
    virtual C &get() = 0;
};

template <typename RefT, typename C, typename Assign>
struct VirtualRangeImpl : VirtualRangeSecondBase<RefT, std::remove_reference_t<C>> {

    VirtualRangeImpl(C &&c)
        : mContainer(std::forward<C>(c))
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

    virtual C &get() override
    {
        return mContainer;
    }

    virtual bool isReference() const override
    {
        if constexpr (std::is_reference_v<C>)
            return true;
        else if constexpr (has_function_isReference_v<C>)
            return mContainer.isReference();
        else
            return false;
    }

private:
    C mContainer;
};

template <typename RefT, typename AssignDefault>
struct VirtualRange {

    template <typename C, typename Assign = AssignDefault, typename = std::enable_if_t<!std::is_same_v<std::decay_t<C>, VirtualRange<RefT, AssignDefault>>>>
    explicit VirtualRange(C &&c, type_holder_t<Assign> = {})
        : mRange(std::make_shared<VirtualRangeImpl<RefT, C, Assign>>(std::forward<C>(c)))
    {
    }

    VirtualRange(const VirtualRange &) = default;
    VirtualRange(VirtualRange &&) = default;

    template <typename Assign, typename C>
    void assign(C&& c) {
        mRange = std::make_shared<VirtualRangeImpl<RefT, C, Assign>>(std::forward<C>(c));
    }

    template <typename C, typename = std::enable_if_t<!std::is_same_v<std::decay_t<C>, VirtualRange<RefT, AssignDefault>>>>
    VirtualRange& operator=(C&& c) {
        assign<AssignDefault>(std::forward<C>(c));
        return *this;
    }

    VirtualRange &operator=(const VirtualRange &) = default;
    VirtualRange &operator=(VirtualRange &&) = default;

    VirtualIterator<RefT> begin() const
    {
        return mRange->begin(mRange);
    }

    VirtualIterator<RefT> end() const
    {
        return mRange->end(mRange);
    }

    template <typename C>
    HeapObject<C, std::shared_ptr<C>> safe_cast()
    {
        VirtualRangeSecondBase<RefT, C> *typed = dynamic_cast<VirtualRangeSecondBase<RefT, C> *>(mRange.get());
        if (typed) {
            if (isReference()) {
                return std::shared_ptr<C> { &typed->get(), [](C *) {} };
            } else {
                return std::shared_ptr<C> { mRange, &typed->get() };
            }
        } else {
            std::shared_ptr<C> result = std::make_shared<C>();
            throw 0;
            return result;
        }
    }

    template <typename C>
    HeapObject<const C, std::shared_ptr<const C>> safe_cast() const
    {
        VirtualRangeSecondBase<RefT, C> *typed = dynamic_cast<VirtualRangeSecondBase<RefT, C> *>(mRange.get());
        if (typed) {
            if (isReference()) {
                return std::shared_ptr<const C> { &typed->get(), [](C *) {} };
            } else {
                return std::shared_ptr<const C> { mRange, &typed->get() };
            }
        } else {
            std::shared_ptr<C> result = std::make_shared<C>();
            throw 0;
            return std::shared_ptr<const C> { result };
        }
    }

    template <typename C>
    C &unsafe_cast()
    {
        return dynamic_cast<VirtualRangeSecondBase<RefT, C> *>(mRange.get())->get();
    }

    template <typename C>
    const C &unsafe_cast() const
    {
        return dynamic_cast<VirtualRangeSecondBase<RefT, C> *>(mRange.get())->get();
    }

    bool isReference() const
    {
        return mRange->isReference();
    }

    bool operator==(const VirtualRange &other) const
    {
        throw "TODO";
        return false;
        //return std::equal(begin(), end(), other.begin(), other.end());
    }

private:
    std::shared_ptr<VirtualRangeBase<RefT>> mRange;
};
}
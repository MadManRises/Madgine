#pragma once

namespace Engine {

template <typename T>
struct LockFreeUnmanagedNode {
    LockFreeUnmanagedNode() = default;
    LockFreeUnmanagedNode(const LockFreeUnmanagedNode &) = delete;
    LockFreeUnmanagedNode(LockFreeUnmanagedNode &&) = delete;
    LockFreeUnmanagedNode &operator=(LockFreeUnmanagedNode &&) = delete;
    LockFreeUnmanagedNode &operator=(const LockFreeUnmanagedNode &) = delete;

    std::atomic<T *> mNext = nullptr;
};

template <typename T>
struct LockFreeUnmanagedQueue {
    static_assert(std::derived_from<T, LockFreeUnmanagedNode<T>>);

    bool push(T *item)
    {
        std::unique_lock lock { mMutex };
        T *previous = mTail;
        while (!mTail.compare_exchange_weak(previous, item))
            ;
        bool wasFirst = previous == nullptr;
        if (previous) {
            previous->mNext = item;
        } else {
            T *expected = nullptr;
            bool result = mHead.compare_exchange_strong(expected, item);
            assert(result);
        }
        return wasFirst;
    }

    T *pop()
    {
        std::unique_lock lock { mMutex };
        T *head = nullptr;
        T *newHead = nullptr;
        while (!mHead.compare_exchange_weak(head, newHead)) {
            if (head) {
                if (head != mTail)
                    while (!head->mNext)
                        ;
                newHead = head->mNext;
            } else {
                newHead = nullptr;
            }
        }
        if (head == mTail)
            mTail = nullptr;
        return head;
    }

    bool empty() const
    {
        std::unique_lock lock { mMutex };
        return !mHead;
    }

private:
    std::atomic<T *> mHead = nullptr;
    std::atomic<T *> mTail = nullptr;
#warning "Make it properly lock-free"
    mutable std::mutex mMutex; //TODO: get rid
};

}
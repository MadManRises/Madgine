#pragma once

namespace Engine {

template <typename PtrT>
struct DoublyLinkedNodeBase {

    void link(const PtrT &ptr)
    {
        assert(!ptr->mNode.mPrev);
        assert(!ptr->mNode.mNext);
        ptr->mNode.mPrev = &mNext;
        ptr->mNode.mNext = mNext;
        if (mNext)
            mNext->mNode.mPrev = &ptr->mNode.mNext;
        mNext = ptr;
    }


protected:
    PtrT mNext = nullptr;
};

template <typename PtrT>
struct DoublyLinkedStartNode : DoublyLinkedNodeBase<PtrT> {

    void reset() {
        mNext = nullptr;
    }

};

template <typename PtrT>
struct DoublyLinkedNode : DoublyLinkedNodeBase<PtrT> {

    void unlink() {
        if (mNext)
            mNext->mNode.mPrev = mPrev;        
        *mPrev = mNext;
        mPrev = nullptr;
        mNext = nullptr;
    }

private:
    PtrT *mPrev = nullptr;

    friend struct DoublyLinkedNodeBase<PtrT>;
};

}
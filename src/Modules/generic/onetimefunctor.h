#pragma once

#include "tupleunpacker.h"

namespace Engine {

//TODO Maybe mutable lambda is enough
template <class T, class... Ty>
struct OneTimeFunctor {
    OneTimeFunctor(void (T::*f)(Ty...), T *t, Ty &&... args)
        : mT(t)
        , mF(f)
        , mData(std::forward<Ty>(args)...)
        , mCalled(false)
    {
    }

    OneTimeFunctor(OneTimeFunctor<T, Ty...> &&other) noexcept
        : mT(other.mT)
        , mF(other.mF)
        , mData(std::forward<std::tuple<std::remove_reference_t<Ty>...>>(other.mData))
        , mCalled(other.mCalled)
    {
        other.mCalled = true;
    }

    OneTimeFunctor(const OneTimeFunctor<T, Ty...> &other)
        : mT(other.mT)
        , mF(other.mF)
        , mData(std::forward<std::tuple<std::remove_reference_t<Ty>...>>(other.mData))
        , mCalled(other.mCalled)
    {
        other.mCalled = true;
    }

    ~OneTimeFunctor()
    {
    }

    void operator()()
    {
        assert(!mCalled);
        mCalled = true;
        TupleUnpacker::invokeExpand(mF, mT, std::move(mData));
    }

private:
    T *mT;
    void (T::*mF)(Ty...);
    mutable std::tuple<std::remove_reference_t<Ty>...> mData;
    mutable bool mCalled;
};

template <class T, class... Ty>
auto oneTimeFunctor(void (T::*f)(Ty...), T *t, Ty &&... args)
{
    return OneTimeFunctor<T, Ty...> { f, t, std::forward<Ty>(args)... };
}

}
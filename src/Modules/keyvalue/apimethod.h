#pragma once

#include "typedscopeptr.h"

namespace Engine {

struct ApiMethod {
    typedef ValueType (*FPtr)(TypedScopePtr, const ArgumentList &);

    constexpr ApiMethod()
        : mFunction(nullptr)
        , mArgumentsCount(0)
        , mIsVariadic(false)
    {
    }

    constexpr ApiMethod(FPtr f, size_t argumentsCount, bool isVariadic)
        : mFunction(f)
        , mArgumentsCount(argumentsCount)
        , mIsVariadic(isVariadic)
    {
    }

    bool operator==(const ApiMethod &other) const
    {
        return mFunction == other.mFunction;
    }

    operator bool() const
    {
        return mFunction != nullptr;
    }

    operator FPtr() const
    {
        return mFunction;
    }

    size_t arguments_count() const
    {
        return mArgumentsCount;
    }

private:
    FPtr mFunction;
    size_t mArgumentsCount;
    bool mIsVariadic;
};

}
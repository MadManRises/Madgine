#pragma once

namespace Engine {

struct ApiMethod {
    typedef void (*FPtr)(ValueType &, TypedScopePtr, const ArgumentList &);

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

    FPtr mFunction = nullptr;
    uint32_t mArgumentsCount = 0;
    bool mIsVariadic = false;
};

}
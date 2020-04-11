#pragma once

namespace Engine {

struct ApiFunction {
    typedef void (*FPtr)(ValueType &, TypedScopePtr, const ArgumentList &);

    bool operator==(const ApiFunction &other) const
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

    size_t argumentsCount() const;

    FPtr mFunction = nullptr;
    const FunctionTable *mTable = nullptr;
};

}
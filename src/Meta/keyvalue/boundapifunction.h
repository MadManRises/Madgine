#pragma once

#include "apifunction.h"

namespace Engine {

struct META_EXPORT BoundApiFunction {

    constexpr BoundApiFunction() = default;

    BoundApiFunction(const ApiFunction &f, const TypedScopePtr &scope);

    bool operator==(const BoundApiFunction &other) const
    {
        return mFunction == other.mFunction && mScope == other.mScope;
    }

    uint32_t argumentsCount(bool excludeThis = false) const
    {
        return mFunction.argumentsCount(excludeThis);
    }

    bool isMemberFunction() const
    {
        return mFunction.isMemberFunction();
    }

    TypedScopePtr scope() const;

    void operator()(ValueType &retVal, const ArgumentList &args) const;

    ApiFunction mFunction;
    void *mScope = nullptr;
};

}
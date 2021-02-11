#pragma once

#include "apifunction.h"
#include "typedscopeptr.h"

namespace Engine {

struct META_EXPORT BoundApiFunction {

    bool operator==(const BoundApiFunction &other) const
    {
        return mMethod == other.mMethod && mScope == other.mScope;
    }

    uint32_t argumentsCount(bool excludeThis = false) const
    {
        return mMethod.argumentsCount(excludeThis);
    }

    bool isMemberFunction() const
    {
        return mMethod.isMemberFunction();
    }

    void operator()(ValueType &retVal, const ArgumentList &args) const;

	ApiFunction mMethod;
    TypedScopePtr mScope;
};

}
#pragma once

#include "apifunction.h"
#include "typedscopeptr.h"

namespace Engine {

struct MODULES_EXPORT BoundApiFunction {

    bool operator==(const BoundApiFunction &other) const
    {
        return mMethod == other.mMethod && mScope == other.mScope;
    }

    uint32_t argumentsCount() const
    {
        return mMethod.argumentsCount();
    }

    void operator()(ValueType &retVal, const ArgumentList &args) const
    {
        return mMethod(retVal, mScope, args);
    }


	ApiFunction mMethod;
    TypedScopePtr mScope;
};

}
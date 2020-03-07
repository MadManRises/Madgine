#pragma once

#include "apimethod.h"
#include "typedscopeptr.h"

namespace Engine {

struct MODULES_EXPORT BoundApiMethod {

    bool operator==(const BoundApiMethod &other) const
    {
        return mMethod == other.mMethod && mScope == other.mScope;
    }

    uint32_t argumentsCount() const
    {
        return mMethod.mArgumentsCount;
    }

    void operator()(ValueType &retVal, const ArgumentList &args) const;


	ApiMethod mMethod;
    TypedScopePtr mScope;
};

}
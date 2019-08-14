#pragma once

#include "apimethod.h"
#include "typedscopeptr.h"

namespace Engine {

struct MODULES_EXPORT BoundApiMethod {

    BoundApiMethod() = default;

    constexpr BoundApiMethod(ApiMethod m, TypedScopePtr s)
        : mMethod(m)
        , mScope(s)
    {
    }

    bool operator==(const BoundApiMethod &other) const
    {
        return mMethod == other.mMethod && mScope == other.mScope;
    }

    size_t argumentsCount() const
    {
        return mMethod.argumentsCount();
    }

    ValueType operator()(const ArgumentList &args) const;

    const TypedScopePtr &scope() const
    {
        return mScope;
    }

    const ApiMethod &method() const
    {
        return mMethod;
	}

private:
    ApiMethod mMethod;
    TypedScopePtr mScope;
};

}
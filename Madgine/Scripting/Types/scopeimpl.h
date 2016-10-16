#pragma once

#include "scopefactoryimpl.h"
#include "api.h"

namespace Engine {
namespace Scripting {

template <class T, class Base = Scope>
class ScopeImpl : public ScopeFactoryImpl<T, Base>, protected API<T> {

public:
	using ScopeFactoryImpl::ScopeFactoryImpl;

    ValueType methodCall(const std::string &name, const ArgumentList &args = {})
    {
        ValueType result;		
        if (hasMethod(name)) {
			result = execMethod(name, args);
        } else {
            result = Scope::methodCall(name, args);
        }
        return result;

    }

};

}
}




#pragma once

#include "scopefactoryimpl.h"
#include "api.h"

namespace Engine {
namespace Scripting {

template <class T, class Base = Scope>
class ScopeImpl : public ScopeFactoryImpl<T, Base>, public API<T> {

public:
	using ScopeFactoryImpl<T, Base>::ScopeFactoryImpl;

    ValueType methodCall(const std::string &name, const ArgumentList &args = {})
    {
        ValueType result;		
        if (this->hasMethod(name)) {
			result = this->execMethod(name, args);
        } else {
            result = Scope::methodCall(name, args);
        }
        return result;

    }

};

}
}




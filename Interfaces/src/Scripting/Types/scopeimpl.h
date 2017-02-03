#pragma once

#include "api.h"
#include "scope.h"
#include "Serialize/serializableunit.h"

namespace Engine {
namespace Scripting {


template <class T, class Base = Scope>
class ScopeImpl : public Base, public API<T> {

public:
	using Base::Base;

    ValueType methodCall(const std::string &name, const ArgumentList &args = {})
    {
        ValueType result;		
        if (this->hasMethod(name)) {
			result = this->execMethod(name, args);
        } else {
            result = Base::methodCall(name, args);
        }
        return result;

    }

	virtual std::string getIdentifier() override {
		return typeid(T).name();
	}

};

}
}




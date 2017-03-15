#pragma once

#include "Scripting/Types/api.h"
#include "GlobalScope.h"

namespace Engine {
namespace Scripting {


template <class T>
class GlobalAPI : public API<T> {
public:
	GlobalAPI() :
		mGlobalScope(&GlobalScope::getSingleton()) {
		mGlobalScope->addAPI(this);
	}
	virtual ~GlobalAPI() {
		mGlobalScope->removeAPI(this);
	}

	std::pair<bool, ValueType> callGlobalMethod(const std::string &name, const ArgumentList &args = {}) {
		return mGlobalScope->callMethodCatch(name, args);
	}

	std::pair<bool, ValueType> callGlobalMethodIfAvailable(const std::string &name, const ArgumentList &args = {}) {
		return mGlobalScope->callMethodIfAvailable(name, args);
	}

protected:
	GlobalScope *globalScope() {
		return mGlobalScope;
	}

private:
	GlobalScope *mGlobalScope;
};




} // namespace Scripting
} // namespace Core
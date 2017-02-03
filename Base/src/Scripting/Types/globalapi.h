#pragma once

#include "Scripting/Types/api.h"
#include "globalscopeimpl.h"

namespace Engine {
namespace Scripting {


template <class T>
class GlobalAPI : public API<T> {
public:
	GlobalAPI() :
		mGlobalScope(&GlobalScopeImpl::getSingleton()) {
		mGlobalScope->addAPI(this);
	}
	virtual ~GlobalAPI() {
		mGlobalScope->removeAPI(this);
	}

	bool callGlobalMethod(const std::string &name, const ArgumentList &args = {}) {
		return mGlobalScope->callMethodCatch(name, args);
	}

	bool callGlobalMethodIfAvailable(const std::string &name, const ArgumentList &args = {}) {
		return mGlobalScope->callMethodIfAvailable(name, args);
	}

protected:
	GlobalScopeImpl *globalScope() {
		return mGlobalScope;
	}

private:
	GlobalScopeImpl *mGlobalScope;
};




} // namespace Scripting
} // namespace Core
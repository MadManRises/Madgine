#pragma once



#include "scripting/scriptingexception.h"
#include "scopebase.h"

namespace Engine {
namespace Scripting {

template <class T>
T *scope_cast(ScopeBase *s) {
	T *t = dynamic_cast<T*>(s);
	if (!t)
		MADGINE_THROW_NO_TRACE(ScriptingException(Exceptions::unexpectedScopeType(s->getIdentifier(), typeid(T).name())));
	return t;
}


}
}



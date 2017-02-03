#pragma once

#include "Util\UtilMethods.h"
#include "exceptionmessages.h"
#include "Scripting\scriptingexception.h"
#include "scope.h"

namespace Engine {
namespace Scripting {

template <class T>
T *scope_cast(Scope *s) {
	T *t = dynamic_cast<T*>(s);
	if (!t)
		MADGINE_THROW_NO_TRACE(ScriptingException(Exceptions::unexpectedScopeType(s->getIdentifier(), typeid(T).name())));
	return t;
}


}
}



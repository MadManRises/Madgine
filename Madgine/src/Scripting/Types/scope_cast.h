#pragma once

#include "Util\UtilMethods.h"
#include "Database\exceptionmessages.h"
#include "Serialize\serializablebase.h"
#include "Scripting\scriptingexception.h"

namespace Engine {
namespace Scripting {

template <class T>
T *scope_cast(Scope *s) {
	T *t = dynamic_cast<T*>(s);
	if (!t)
		MADGINE_THROW_NO_TRACE(ScriptingException(Database::Exceptions::unexpectedScopeType(s->getIdentifier(), typeid(T).name())));
	return t;
}


}
}



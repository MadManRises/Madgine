#pragma once


#include "../scriptingexception.h"
#include "scopebase.h"

namespace Engine
{
	namespace Scripting
	{
		template <class T>
		T* scope_cast(ScopeBase* s)
		{
			T* t = dynamic_cast<T*>(s);
			if (!t)
				throw ScriptingException(Database::Exceptions::unexpectedScopeType(s->getIdentifier(), typeid(T).name()));
			return t;
		}
	}
}

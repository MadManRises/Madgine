#pragma once


#include "typedscopeptr.h"
#include "scopebase.h"
#include "valuetypeexception.h"

namespace Engine
{
		template <class T>
		T* scope_cast(TypedScopePtr s)
		{
                    if (!s.mType->isInstance<T>())
                        throw ValueTypeException(Database::Exceptions::unexpectedScopeType(typeid(*s).name(), typeid(T).name()));
			return static_cast<T*>(s.mScope);
		}
}

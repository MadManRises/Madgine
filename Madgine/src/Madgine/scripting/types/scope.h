#pragma once

#include "../../reflection/classname.h"
#include "scopebase.h"


namespace Engine
{
	namespace Scripting
	{
		template <class T, class Base = ScopeBase>
		class Scope : public Base
		{
		public:
			using Base::Base;

			virtual const char* key() const
			{
				return ClassName<T>();
			}

			virtual std::string getIdentifier() const override
			{
				return ClassName<T>();
			}

		};
	}
}

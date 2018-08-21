#pragma once

#include "api.h"
#include "scopebase.h"
#include "../../generic/keyvalueiterate.h"


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
				return API<T>::name();
			}

			virtual KeyValueMapList maps() override
			{
				return Base::maps().merge(API<T>::api());
			}

			virtual std::string getIdentifier() const override
			{
				return API<T>::name();
			}

		};
	}
}

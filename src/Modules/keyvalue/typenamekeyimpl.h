#pragma once

#include "../reflection/classname.h"


namespace Engine
{
		template <class T, class Base>
		class TypenameKeyImpl : public Base
		{
		public:
			using Base::Base;

			virtual const char* key() const override
			{
				return typeName<T>();
			}

		};
}

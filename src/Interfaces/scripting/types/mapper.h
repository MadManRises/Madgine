#pragma once

#include "../../generic/valuetype.h"

namespace Engine
{
	namespace Scripting
	{
		struct Mapper
		{
			Mapper() = delete;

			Mapper(ValueType (*getter)(ScopeBase*), void (*setter)(ScopeBase *, const ValueType &) = nullptr) : 
			 mGetter(getter),
			 mSetter(setter)
			{
			}

			ValueType (*mGetter)(ScopeBase*);
			void(*mSetter)(ScopeBase *, const ValueType &);

			bool isWritable() const
			{
				return mSetter != nullptr;
			}

		private:
			template <class T, ValueType(*F)(T*, const ArgumentList&)>
			static ValueType map_f2(ScopeBase* s, const ArgumentList& args)
			{
				return F(scope_cast<T>(s), args);
			}

		public:

			template <class T, ValueType(*F)(T*, const ArgumentList&)>
			static ValueType map_f(ScopeBase*)
			{
				return ValueType(&map_f2<T, F>);
			}
		};
	}
}

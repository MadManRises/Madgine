#pragma once

#include "luatableinstance.h"
#include "../../keyvalue/valuetype.h"

namespace Engine
{
	namespace Scripting
	{

		class LuaTableInstance;

		class MODULES_EXPORT LuaTableFieldAccessor
		{
		public:
			LuaTableFieldAccessor(const std::shared_ptr<LuaTableInstance> &instance, const std::string &name);

			template <class T>
			decltype(auto) asDefault(const T &def) {
				return mInstance->getValue(mName).asDefault(def);
			}

			void operator=(const ValueType &v);

			operator ValueType();

		private:
			std::shared_ptr<LuaTableInstance> mInstance;
			std::string mName;

		};
	}
}

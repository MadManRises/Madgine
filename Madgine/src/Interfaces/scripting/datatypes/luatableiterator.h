#pragma once

#include "../../generic/valuetype.h"

namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT LuaTableIterator
		{
		public:
			LuaTableIterator(const std::shared_ptr<LuaTableInstance>& instance);
			LuaTableIterator();

			bool operator !=(const LuaTableIterator& other) const;
			bool operator ==(const LuaTableIterator& other) const;
			void operator++();
			const std::pair<std::string, ValueType>& operator *() const;

		private:
			std::pair<std::string, ValueType> mCurrent;
			std::shared_ptr<LuaTableInstance> mInstance;
		};
	}
}

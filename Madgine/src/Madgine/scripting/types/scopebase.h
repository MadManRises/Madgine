#pragma once


#include "../datatypes/luatable.h"
#include "../datatypes/argumentlist.h"

namespace Engine
{
	namespace Scripting
	{
		class INTERFACES_EXPORT ScopeBase
		{
		public:
			ScopeBase(const LuaTable& table);
			ScopeBase(const ScopeBase&) = delete;
			ScopeBase(ScopeBase&&) = default;
			virtual ~ScopeBase();

			bool hasMethod(const std::string& name);

			ArgumentList callMethod(const std::string& name,
			                        const ArgumentList& args = {});

			std::optional<ArgumentList> callMethodIfAvailable(const std::string& name, const ArgumentList& args = {});
			std::optional<ArgumentList> callMethodCatch(const std::string& name, const ArgumentList& args = {});

			void push() const;

			std::optional<ValueType> get(const std::string& key);
			std::unique_ptr<KeyValueIterator> iterator();

			virtual KeyValueMapList maps();

			virtual std::string getIdentifier() const;
			//virtual const char *key() const = 0;

		protected:
			LuaTable table() const;

		private:
			LuaTable mTable;
		};
	}
}
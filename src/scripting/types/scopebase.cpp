#include "../../interfaceslib.h"
#include "scopebase.h"

#include "api.h"

#include "../../generic/keyvalueiterate.h"

namespace Engine
{
	namespace Scripting
	{
		ScopeBase::ScopeBase(const LuaTable& table) :
			mTable(table)
		{
			assert(table);
			mTable.setLightUserdata("___scope___", this);
			mTable.setMetatable("Interfaces.Scope");
		}

		ScopeBase::~ScopeBase()
		{
		}

		void ScopeBase::push() const
		{
			mTable.push();
		}

		ArgumentList ScopeBase::methodCall(const std::string& name, const ArgumentList& args)
		{
			return mTable.callMethod(name, args);
		}

		std::pair<bool, ArgumentList> ScopeBase::callMethodIfAvailable(const std::string& name, const ArgumentList& args)
		{
			if (hasMethod(name))
				return callMethodCatch(name, args);
			return make_pair(false, ArgumentList());
		}

		std::pair<bool, ArgumentList> ScopeBase::callMethodCatch(const std::string& name, const ArgumentList& args)
		{
			try
			{
				return make_pair(true, methodCall(name, args));
			}
			catch (std::exception& e)
			{
				LOG_EXCEPTION(e);
			}
			return make_pair(false, ArgumentList());
		}


		bool ScopeBase::hasMethod(const std::string& name)
		{
			return mTable.hasFunction(name);
		}

		std::string ScopeBase::getIdentifier() const
		{
			return typeid(*this).name();
		}

		std::pair<bool, ValueType> ScopeBase::get(const std::string& key)
		{
			return maps().get(key);
		}

		std::unique_ptr<KeyValueIterator> ScopeBase::iterator()
		{
			return maps().iterator();
		}

		KeyValueMapList ScopeBase::maps()
		{
			return KeyValueMapList(this);
		}

		LuaTable ScopeBase::table() const
		{
			return mTable;
		}

	}
}

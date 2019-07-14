#pragma once

#include "../datatypes/luatable.h"
#include "../../keyvalue/scopebase.h"
#include "../datatypes/luathread.h"

namespace Engine {
namespace Scripting {

    template <typename Base = ScopeBase>
    struct ScopeEntity : Base {
    public:
        ScopeEntity(LuaTable table)
            : mTable(table)
        {
            assert(table);
            mTable.setLightUserdata("___scope___", this);
            mTable.setMetatable("Interfaces.Scope");
        }

        ScopeEntity(const LuaThread *thread)
            : ScopeEntity(thread->table())
        {
        }

		template <typename T>
        ScopeEntity(ScopeEntity<T> *parent)
            : ScopeEntity(parent->table().createTable())
        {
        }

        ScopeEntity(const ScopeEntity &) = delete;
        ScopeEntity(ScopeEntity &&) = default;
        ~ScopeEntity() = default;

        /*bool hasMethod(const std::string& name);

			ArgumentList callMethod(const std::string& name,
			                        const ArgumentList& args);

			std::optional<ArgumentList> callMethodIfAvailable(const std::string& name, const ArgumentList& args);
			std::optional<ArgumentList> callMethodCatch(const std::string& name, const ArgumentList& args);

			void push(lua_State *state) const;*/

        /*std::unique_ptr<KeyValueVirtualIterator> find(const std::string &key);
			std::optional<ValueType> get(const std::string& key);
			bool set(const std::string &key, const ValueType &value);
			
			*/
        //virtual KeyValueMapList maps();
        /*
			virtual std::string getIdentifier() const;*/
        //virtual const char *key() const = 0;

        LuaTable table() const
        {
            return mTable;
        }

    private:
        LuaTable mTable;
    };

}
}
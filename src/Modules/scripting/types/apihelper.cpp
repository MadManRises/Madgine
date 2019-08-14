#include "../../moduleslib.h"

#include "../../keyvalue/keyvalueiterate.h"
#include "../../keyvalue/valuetype.h"
#include "apihelper.h"

extern "C" {
#include "../../lua/lauxlib.h"
#include "../../lua/lua.h"
}

namespace Engine {
namespace Scripting {
    const luaL_Reg APIHelper::sUserdataMetafunctions[] = {
        { "__gc", &lua_gc },
        { "__tostring", &lua_toString },
        //{ "__pairs", &lua_pairs },
        //{"__newindex", &lua_newindex}
        { nullptr, nullptr }
    };

    int APIHelper::lua_gc(lua_State *state)
    {
        Userdata *userdata = static_cast<Userdata *>(lua_touserdata(state, -1));
        userdata->~Userdata();
        lua_pop(state, 1);
        return 0;
    }

    int APIHelper::lua_toString(lua_State *state)
    {
        Userdata *userdata = static_cast<Userdata *>(lua_touserdata(state, -1));
        lua_pop(state, 1);
        if (std::holds_alternative<KeyValueVirtualIterator>(*userdata)) {
            (*std::get<KeyValueVirtualIterator>(*userdata)).first.push(state);
        } else if (std::holds_alternative<ApiMethod>(*userdata)) {
            lua_pushstring(state, "<user-defined method>");
        }
        return 1;
    }

    void APIHelper::createMetatables(lua_State *state)
    {
        luaL_newmetatable(state, "Interfaces.kvUserdataMetatable");

        luaL_setfuncs(state, sUserdataMetafunctions, 0);

        lua_pop(state, 1);
    }

    void APIHelper::pop(lua_State *state, int n)
    {
        lua_pop(state, n);
    }

    int APIHelper::stackSize(lua_State *state)
    {
        return lua_gettop(state);
    }

    int APIHelper::error(lua_State *state, const std::string &msg)
    {
        return luaL_error(state, msg.c_str());
    }

}
}

#pragma once

#include "scripting/datatypes/luatable.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class INTERFACES_EXPORT ScriptParser {
public:
    ScriptParser();
	ScriptParser(const ScriptParser &) = delete;
    virtual ~ScriptParser();

	void operator=(const ScriptParser &) = delete;

	void parseScript(std::istream &stream, const std::string &name, bool reload);

	void executeString(lua_State *state, const std::string &cmd);

	static ScriptParser &getSingleton();


	std::string fileExtension();

	LuaTable createThread();
	int pushThread(lua_State *state, lua_State *thread);

	void makeFinalized();

private:
	static const constexpr size_t READ_BUFFER = 256;
	
	static const char *reader(lua_State *L,
		void *parser,
		size_t *size);

	const char * read(size_t *size);

	

private:
	lua_State *mState;
	LuaTable mRegistry;
	LuaTable mEnv;
	LuaTable mGlobal;
	bool mFinalized;

	std::istream *mStream;
	char *mBuffer;
	bool mChunk;

	static ScriptParser *sSingleton;

	std::map<lua_State *, int> mThreads;

	static const luaL_Reg sEnvMetafunctions[];
	static const luaL_Reg sScopeMetafunctions[];
	static const luaL_Reg sGlobalMetafunctions[];
	static const luaL_Reg sGlobalScopeMetafunctions[];

	static int lua_indexScope(lua_State *state);
	static int lua_pairsScope(lua_State *state);
	static int lua_nextScope(lua_State *state);
	static int lua_newindexGlobal(lua_State *state);
	static int lua_indexGlobalScope(lua_State *state);
	static int lua_indexEnv(lua_State *state);
	static int lua_pairsEnv(lua_State *state);
	static int lua_newindexEnv(lua_State *state);
	static int lua_tostringEnv(lua_State *state);
	static void pushGlobalScope(lua_State *state);
};

}
}
}



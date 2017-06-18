#include "interfaceslib.h"
#include "scriptparser.h"
#include "parseexception.h"

#include "scripting/types/scopebase.h"

#include "scripting/types/api.h"

extern "C" {
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
#include <lualib.h>                             /* Always include this when calling Lua */
}

namespace Engine {
namespace Scripting {
namespace Parsing {	

	ScriptParser *ScriptParser::sSingleton = nullptr;

	const luaL_Reg ScriptParser::sMetafunctions[] = 
	{
		{"__index", &lua_index},
		{"__pairs", &lua_pairs},
		//{"__newindex", &lua_newindex}
		{NULL, NULL}
	};

ScriptParser::ScriptParser()
{
	assert(!sSingleton);
	sSingleton = this;

	mState = luaL_newstate();

	luaL_openlibs(mState);

	luaL_newmetatable(mState, "Interfaces.Metatable");

	luaL_setfuncs(mState, sMetafunctions, 0);

	lua_pop(mState, 1);

	APIHelper::createMetatables(mState);
}

ScriptParser::~ScriptParser()
{
	lua_close(mState);
	sSingleton = nullptr;
}

void ScriptParser::executeString(const std::string & cmd)
{
	if (luaL_dostring(mState, cmd.c_str()) != 0) {
		LOG_ERROR(lua_tostring(mState, -1));
	}
}

void ScriptParser::parseScript(std::istream &stream, const std::string &name,
								bool reload)
{
	mStream = &stream;
	mChunk = true;

	char buffer[READ_BUFFER];
	mBuffer = buffer;
	
	while (mChunk) {
		switch (lua_load(mState, &ScriptParser::reader, this, name.c_str(), NULL)) {
		case 0:
			break;
		case LUA_ERRSYNTAX:
			throw ParseException("Lua Syntax-Error!");
		case LUA_ERRMEM:
			throw ParseException("Lua Out-Of-Memory!");
		default:
			throw 0;
		}		
	}
	switch (lua_pcall(mState, 0, 0, 0)) {
	case 0:
		break;
	case LUA_ERRRUN: {
		int iType = lua_type(mState, -1);
		const char *msg = "unknown Error";
		if (iType == LUA_TSTRING){
			msg = lua_tostring(mState, -1);
		}
		lua_pop(mState, 1);
		throw ParseException(std::string("Runtime Error: ")+msg);
	}
	case LUA_ERRMEM:
		throw ParseException("Lua Out-Of-Memory!");
	default:
		throw 0;
	}
}


void ScriptParser::reparseFile(const std::string & name)
{
	/*Ogre::DataStreamPtr stream =
		Ogre::ResourceGroupManager::getSingleton().openResource(
			name, "General", true);
	parseScript(stream, group, true);*/
}

ScriptParser & ScriptParser::getSingleton()
{
	assert(sSingleton);
	return *sSingleton;
}

const char *ScriptParser::reader(lua_State *L,
	void *parser,
	size_t *size) {
	return static_cast<ScriptParser*>(parser)->read(size);
}

const char *ScriptParser::read(size_t *size) {
	if (mStream->eof()) {
		mChunk = false;
		return NULL;
	}
	mStream->read(mBuffer, READ_BUFFER);
	*size = mStream->gcount();
	if (*size == 0)
		mChunk = false;
	return mBuffer;
}



std::string ScriptParser::fileExtension() {
	return "lua";
}


lua_State *ScriptParser::lua_state() {
	return mState;
}

lua_State* ScriptParser::createThread() {
	lua_State *thread = lua_newthread(mState);

	mThreads[thread] = luaL_ref(mState, LUA_REGISTRYINDEX);

	lua_newtable(thread);
	
	lua_rawgeti(thread, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);

	lua_newtable(thread);

	lua_pushnil(thread);
	while (lua_next(thread, -3) != 0) {
		lua_pushvalue(thread, -2);
		lua_insert(thread, -2);
		lua_settable(thread, -4);
	}

	lua_rawseti(thread, -3, LUA_RIDX_GLOBALS);

	lua_pop(thread, 1);

	lua_replace(thread, LUA_REGISTRYINDEX);



	return thread;
}

int ScriptParser::lua_index(lua_State *state) {

	ScopeBase *scope = APIHelper::to<ScopeBase*>(state, -2);

	std::string key = lua_tostring(state, -1);

	lua_pop(state, 2);

	return scope->resolve(state, key);
}

int ScriptParser::lua_newindex(lua_State *state) {
	return 0;
}

int ScriptParser::lua_pairs(lua_State * state)
{
	if (lua_gettop(state) != 1 || !lua_istable(state, -1))
		luaL_error(state, "illegal Call to __pairs!");

	lua_pushcfunction(state, &lua_nextImpl);

	lua_insert(state, -2);

	return 2;
}

int ScriptParser::lua_nextImpl(lua_State * state)
{
	lua_settop(state, 2);

	ScopeBase *scope = APIHelper::to<ScopeBase*>(state, -2);

	assert(scope);

	KeyValueIterator *it = nullptr;

	if (lua_isnil(state, -1)) {
		it = scope->iterator();
		lua_pop(state, 1);
		APIHelper::push(state, it);
	}
	else if (lua_isuserdata(state, -1)) {
		it = APIHelper::to<KeyValueIterator*>(state, -1);
		++(*it);
	}

	if (it) {
		if (!it->ended()) {
			return 1 + it->push(state);
		}
		lua_pop(state, 1);
		lua_pushnil(state);
	}

	lua_getglobal(state, "next");
	lua_insert(state, -3);
	lua_call(state, 2, 2);
	return 2;
}

}
}
}

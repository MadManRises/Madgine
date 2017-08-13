#include "interfaceslib.h"
#include "scriptparser.h"
#include "parseexception.h"

#include "scripting/types/globalscopebase.h"

#include "scripting/types/apihelper.h"

#include "scripting/types/keyvalue.h"

extern "C" {
#include <lua.h>                                /* Always include this when calling Lua */
#include <lauxlib.h>                            /* Always include this when calling Lua */
#include <lualib.h>                             /* Always include this when calling Lua */
}

#include <iostream>

namespace Engine {
namespace Scripting {
namespace Parsing {	

	ScriptParser *ScriptParser::sSingleton = nullptr;

	const luaL_Reg ScriptParser::sScopeMetafunctions[] = 
	{
		{"__index", &lua_indexScope},
		{"__pairs", &lua_pairsScope},
		{NULL, NULL}
	};

	const luaL_Reg ScriptParser::sGlobalMetafunctions[] =
	{
		{ "__newindex", &lua_newindexGlobal },
		{ NULL, NULL }
	};

	const luaL_Reg ScriptParser::sEnvMetafunctions[] =
	{
		{ "__index", &lua_indexEnv },
		{ "__newindex", &lua_newindexEnv },		
		{ "__pairs", &lua_pairsEnv},
		{"__tostring", &lua_tostringEnv},
		{ NULL, NULL }
	};

	const luaL_Reg ScriptParser::sGlobalScopeMetafunctions[] = 
	{
		{"__index", &lua_indexGlobalScope },
		{"__pairs", &lua_pairsScope},
		{NULL, NULL}
	};

ScriptParser::ScriptParser() :
	mFinalized(false),
	mStream(nullptr),
	mBuffer(nullptr),
	mChunk(false)
{
	assert(!sSingleton);
	sSingleton = this;

	mState = luaL_newstate();

	mRegistry = LuaTable::registry(mState);

	luaL_openlibs(mState);

	luaL_newmetatable(mState, "Interfaces.Scope");
	luaL_setfuncs(mState, sScopeMetafunctions, 0);
	lua_pop(mState, 1);

	luaL_newmetatable(mState, "Interfaces.Global");
	luaL_setfuncs(mState, sGlobalMetafunctions, 0);
	lua_pop(mState, 1);

	luaL_newmetatable(mState, "Interfaces.Env");
	luaL_setfuncs(mState, sEnvMetafunctions, 0);
	lua_pop(mState, 1);

	luaL_newmetatable(mState, "Interfaces.GlobalScope");
	luaL_setfuncs(mState, sGlobalScopeMetafunctions, 0);
	lua_pop(mState, 1);

	APIHelper::createMetatables(mState);

	mEnv = mRegistry.createTable();

	LuaTable tempMetatable = mRegistry.createTable();

	mGlobal = LuaTable::global(mState);

	tempMetatable.setValue("__index", ValueType(mGlobal));
	tempMetatable.setValue("__newindex", ValueType(mGlobal));

	mEnv.setMetatable(tempMetatable);
}

ScriptParser::~ScriptParser()
{
	mGlobal.clear();
	mRegistry.clear();
	mEnv.clear();
	lua_close(mState);
	sSingleton = nullptr;
}

void ScriptParser::makeFinalized()
{
	mGlobal.setMetatable("Interfaces.Global");
	mEnv.setMetatable("Interfaces.Env");

	mFinalized = true;
}

void ScriptParser::parseScript(std::istream &stream, const std::string &name,
								bool reload)
{
	assert(!mFinalized);

	mStream = &stream;
	mChunk = true;

	char buffer[READ_BUFFER];
	mBuffer = buffer;
	
	while (mChunk) {
		switch (lua_load(mState, &ScriptParser::reader, this, name.c_str(), NULL)) {
		case 0:
			break;
		case LUA_ERRSYNTAX:
			LOG_ERROR(lua_tostring(mState, -1));
			return;
		case LUA_ERRMEM:
			throw ParseException("Lua Out-Of-Memory!");
		default:
			throw 0;
		}		
	}
	
	mEnv.push();
	lua_setupvalue(mState, -2, 1);
	switch (lua_pcall(mState, 0, 0, 0)) {
	case 0:
		break;
	case LUA_ERRRUN: {
		int iType = lua_type(mState, -1);
		const char *msg = "unknown Error";
		if (iType == LUA_TSTRING) {
			msg = lua_tostring(mState, -1);
		}
		lua_pop(mState, 1);
		throw ParseException(std::string("Runtime Error: ") + msg);
	}
	case LUA_ERRMEM:
		throw ParseException("Lua Out-Of-Memory!");
	default:
		throw 0;
	}
	
}

void ScriptParser::executeString(lua_State *state, const std::string & cmd)
{
	if (luaL_loadstring(state, cmd.c_str())) {
		LOG_ERROR(lua_tostring(state, -1));
		return;
	}
	mEnv.push(state);
	lua_setupvalue(state, -2, 1);

	if (lua_pcall(state, 0, LUA_MULTRET, 0)) {
		LOG_ERROR(lua_tostring(state, -1));
	}
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
	return ".lua";
}


LuaTable ScriptParser::createThread() {
	assert(mFinalized);

	lua_State *thread = lua_newthread(mState);

	mThreads[thread] = luaL_ref(mState, LUA_REGISTRYINDEX);

	return mRegistry.createTable(thread);
}

int ScriptParser::pushThread(lua_State * state, lua_State * thread)
{
	lua_rawgeti(state, LUA_REGISTRYINDEX, mThreads.at(thread));

	return 1;
}

int ScriptParser::lua_indexScope(lua_State *state) {

	ScopeBase *scope = ValueType::fromStack(state, -2).as<ScopeBase*>();

	std::string key = lua_tostring(state, -1);

	lua_pop(state, 2);

	std::pair<bool, ValueType> p = scope->get(key);
	if (!p.first)
		return 0;
	else {
		p.second.push(state);
		return 1;
	}
}

int ScriptParser::lua_newindexGlobal(lua_State *state) {
	return luaL_error(state, "Trying to modify read-only Global");
}

int ScriptParser::lua_indexGlobalScope(lua_State *state) {
	ScopeBase *scope = ValueType::fromStack(state, -2).as<ScopeBase*>();

	std::string key = lua_tostring(state, -1);

	lua_pop(state, 2);

	std::pair<bool, ValueType> p = scope->get(key);
	if (p.first) {
		p.second.push(state);
		return 1;
	}

	lua_getglobal(state, key.c_str());
	return 1;
}

int ScriptParser::lua_pairsScope(lua_State * state)
{
	if (lua_gettop(state) != 1 || !lua_istable(state, -1))
		luaL_error(state, "illegal Call to __pairs!");

	lua_pushcfunction(state, &lua_nextScope);

	lua_insert(state, -2);

	return 2;
}

int ScriptParser::lua_nextScope(lua_State * state)
{
	lua_settop(state, 2);

	ScopeBase *scope = ValueType::fromStack(state, -2).as<ScopeBase*>();

	assert(scope);

	std::shared_ptr<KeyValueIterator> it;

	if (lua_isnil(state, -1)) {
		lua_pop(state, 1);
		it = scope->iterator();
		ValueType(it).push(state);
	}
	else if (lua_isuserdata(state, -1)) {
		it = ValueType::fromStack(state, -1).as<std::shared_ptr<KeyValueIterator>>();
		++(*it);
	}

	if (it) {
		if (!it->ended()) {
			return 1 + it->value().push(state);
		}
		lua_pop(state, 1);
		lua_pushnil(state);
	}

	lua_getglobal(state, "next");
	lua_insert(state, -3);
	lua_call(state, 2, 2);
	return 2;
}

int ScriptParser::lua_newindexEnv(lua_State * state)
{
	pushGlobalScope(state);
	lua_replace(state, -4);
	lua_settable(state, -3);
	return 1;
}



void stackdump_g(lua_State* l)
{
	int i;
	int top = lua_gettop(l);

	std::cout << "total in stack " << top << std::endl;

	for (i = 1; i <= top; i++)
	{  // repeat for each level 
		int t = lua_type(l, i);
		switch (t) {
		case LUA_TSTRING:  // strings 
			std::cout << "string: '" << lua_tostring(l, i) << "'" << std::endl;
			break;
		case LUA_TBOOLEAN:  // booleans 
			std::cout << "boolean " << (lua_toboolean(l, i) ? "true" : "false") << std::endl;
			break;
		case LUA_TNUMBER:  // numbers 
			std::cout << "number: " << lua_tonumber(l, i) << std::endl;
			break;
		default:  // other values 
			std::cout << lua_typename(l, t) << std::endl;;
			break;
		}
	}
}

int ScriptParser::lua_indexEnv(lua_State * state)
{
	pushGlobalScope(state);
	lua_replace(state, -3);	
	lua_gettable(state, -2);
	lua_replace(state, -2);		
	return 1;
}

int ScriptParser::lua_pairsEnv(lua_State * state)
{
	lua_pop(state, 1);
	lua_pushcfunction(state, &lua_nextScope);
	pushGlobalScope(state);
	return 2;
}

int ScriptParser::lua_tostringEnv(lua_State * state)
{
	pushGlobalScope(state);
	lua_replace(state, -2);
	luaL_tolstring(state, -1, NULL);
	return 1;
}

void ScriptParser::pushGlobalScope(lua_State * state)
{
	GlobalScopeBase::getSingleton().push(); //TODO maybe assert state
}

}
}
}

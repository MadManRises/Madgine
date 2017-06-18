#pragma once

#include "scripting/datatypes/argumentlist.h"


namespace Engine {
namespace Scripting {
namespace Parsing {

class INTERFACES_EXPORT ScriptParser {
public:
    ScriptParser();
	ScriptParser(const ScriptParser &) = delete;
    virtual ~ScriptParser();

	void operator=(const ScriptParser &) = delete;

	void executeString(const std::string &cmd);

	void parseScript(std::istream &stream, const std::string &name, bool reload);

	void reparseFile(const std::string &name);

	static ScriptParser &getSingleton();


	std::string fileExtension();

	lua_State *lua_state();

	lua_State* createThread();

private:
	static const constexpr size_t READ_BUFFER = 256;
	
	static const char *reader(lua_State *L,
		void *parser,
		size_t *size);

	const char * read(size_t *size);

	

private:
	lua_State *mState;

	std::istream *mStream;
	char *mBuffer;
	bool mChunk;

	static ScriptParser *sSingleton;

	std::map<lua_State *, int> mThreads;

	static const luaL_Reg sMetafunctions[];

	static int lua_index(lua_State *state);
	static int lua_newindex(lua_State *state);
	static int lua_pairs(lua_State *state);
	static int lua_nextImpl(lua_State *state);
};

}
}
}



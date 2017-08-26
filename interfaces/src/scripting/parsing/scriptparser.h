#pragma once

#include "scripting/datatypes/luatable.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class INTERFACES_EXPORT ScriptParser {
public:
    ScriptParser(LuaState *state);
	ScriptParser(const ScriptParser &) = delete;
    virtual ~ScriptParser();

	void operator=(const ScriptParser &) = delete;

	void parseScript(std::istream &stream, const std::string &name, bool reload);

	std::string fileExtension();
	LuaState *state();

private:
	static const constexpr size_t READ_BUFFER = 256;
	
	static const char *reader(lua_State *L,
		void *parser,
		size_t *size);

	const char * read(size_t *size);

	

private:


	std::istream *mStream;
	char *mBuffer;
	bool mChunk;

	LuaState *mState;
};

}
}
}



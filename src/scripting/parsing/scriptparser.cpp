#include "../../interfaceslib.h"
#include "scriptparser.h"
#include "parseexception.h"

#include "../types/luastate.h"

extern "C"
{
#include <lua/lua.h>                                /* Always include this when calling Lua */
}

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			ScriptParser::ScriptParser(LuaState* state) :
				mStream(nullptr),
				mBuffer(nullptr),
				mChunk(false),
				mState(state)
			{
			}

			ScriptParser::~ScriptParser()
			{
			}


			void ScriptParser::parseScript(std::istream& stream, const std::string& name,
			                               bool reload)
			{
				assert(!mState->isFinalized());

				mStream = &stream;
				mChunk = true;

				char buffer[READ_BUFFER];
				mBuffer = buffer;

				lua_State* state = mState->state();

				while (mChunk)
				{
					switch (lua_load(state, &ScriptParser::reader, this, name.c_str(), nullptr))
					{
					case 0:
						break;
					case LUA_ERRSYNTAX:
						LOG_ERROR(lua_tostring(state, -1));
						return;
					case LUA_ERRMEM:
						throw ParseException("Lua Out-Of-Memory!");
					default:
						throw 0;
					}
				}

				mState->env().push();
				lua_setupvalue(state, -2, 1);
				switch (lua_pcall(state, 0, 0, 0))
				{
				case 0:
					break;
				case LUA_ERRRUN:
					{
						int iType = lua_type(state, -1);
						const char* msg = "unknown Error";
						if (iType == LUA_TSTRING)
						{
							msg = lua_tostring(state, -1);
						}
						lua_pop(state, 1);
						throw ParseException(std::string("Runtime Error: ") + msg);
					}
				case LUA_ERRMEM:
					throw ParseException("Lua Out-Of-Memory!");
				default:
					throw 0;
				}
			}


			const char* ScriptParser::reader(lua_State* L,
			                                 void* parser,
			                                 size_t* size)
			{
				return static_cast<ScriptParser*>(parser)->read(size);
			}

			const char* ScriptParser::read(size_t* size)
			{
				if (mStream->eof())
				{
					mChunk = false;
					return nullptr;
				}
				mStream->read(mBuffer, READ_BUFFER);
				*size = mStream->gcount();
				if (*size == 0)
					mChunk = false;
				return mBuffer;
			}


			std::string ScriptParser::fileExtension()
			{
				return ".lua";
			}

			LuaState* ScriptParser::state() const
			{
				return mState;
			}
		}
	}
}

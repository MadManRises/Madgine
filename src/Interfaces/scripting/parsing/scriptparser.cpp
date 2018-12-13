#include "../../interfaceslib.h"
#include "scriptparser.h"
#include "parseexception.h"

#include "../types/luastate.h"

extern "C"
{
#include "../../lua/lua.h"
#include "../../lua/lauxlib.h"
}

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{


			MethodHolder::MethodHolder(lua_State* state) :
			mState(state)
			{
				mIndex = luaL_ref(state, LUA_REGISTRYINDEX);
			}

			MethodHolder::MethodHolder(MethodHolder&& other) :
				mState(other.mState),
				mIndex(other.mIndex)
			{
				other.mState = nullptr;
			}

			MethodHolder::~MethodHolder()
			{
				if (mState)
					luaL_unref(mState, LUA_REGISTRYINDEX, mIndex);
			}

			void MethodHolder::call(lua_State *state)
			{
				lua_rawgeti(state, LUA_REGISTRYINDEX, mIndex);
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
						throw ParseException("Runtime Error: "s + msg);
					}
				case LUA_ERRMEM:
					throw std::bad_alloc{};
				default:
					throw 0;
				}
			}

			MethodHolder::operator bool()
			{
				return mState;
			}

			ScriptParser::ScriptParser() :
				mStream(nullptr),
				mBuffer(nullptr),
				mChunk(false),
			mState(LuaState::getSingleton())
			{
			}

			ScriptParser::~ScriptParser()
			{
			}


			std::optional<MethodHolder> ScriptParser::parseScript(std::istream& stream, const std::string& name)
			{

				mStream = &stream;
				mChunk = true;

				char buffer[READ_BUFFER];
				mBuffer = buffer;

				lua_State *state = mState.state();

				while (mChunk)
				{
					switch (lua_load(state, &ScriptParser::reader, this, name.c_str(), nullptr))
					{
					case 0:
						break;
					case LUA_ERRSYNTAX:
						LOG_ERROR(lua_tostring(state, -1));
						return {};
					case LUA_ERRMEM:
						throw std::bad_alloc{};
					default:
						throw 0;
					}
				}

				mState.env().push();
				lua_setupvalue(state, -2, 1);
				return state;
				
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

		}
	}
}
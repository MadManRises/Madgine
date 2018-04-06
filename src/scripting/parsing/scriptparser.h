#pragma once


namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			class INTERFACES_EXPORT ScriptParser
			{
			public:
				ScriptParser(LuaState* state);
				ScriptParser(const ScriptParser&) = delete;
				virtual ~ScriptParser();

				void operator=(const ScriptParser&) = delete;

				void parseScript(std::istream& stream, const std::string& name, bool reload);

				static std::string fileExtension();
				LuaState* state() const;

			private:
				static const constexpr size_t READ_BUFFER = 256;

				static const char* reader(lua_State* L,
				                          void* parser,
				                          size_t* size);

				const char* read(size_t* size);


			private:


				std::istream* mStream;
				char* mBuffer;
				bool mChunk;

				LuaState* mState;
			};
		}
	}
}

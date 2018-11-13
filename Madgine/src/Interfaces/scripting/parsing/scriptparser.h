#pragma once



namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{

			class INTERFACES_EXPORT MethodHolder
			{
				
			public:				
				MethodHolder(lua_State *state);
				MethodHolder(const MethodHolder &) = delete;
				MethodHolder(MethodHolder &&other);

				~MethodHolder();

				void call(lua_State *state);

				operator bool();

			private:
				lua_State *mState;
				int mIndex;
			};

			class INTERFACES_EXPORT ScriptParser
			{
			public:
				ScriptParser();
				virtual ~ScriptParser();
			

				std::optional<MethodHolder> parseScript(std::istream& stream, const std::string& name);

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

				LuaState &mState;
			};
		}
	}
}

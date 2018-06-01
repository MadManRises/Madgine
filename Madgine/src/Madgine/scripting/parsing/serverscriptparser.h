#pragma once

#include "scriptparser.h"

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			class MADGINE_SERVER_EXPORT ServerScriptParser : public ScriptParser
			{
			public:
				ServerScriptParser(LuaState* state, const std::string& rootFolder);

				void parse();

				const std::string& rootFolder() const;

			private:
				std::string mRootFolder;
			};
		}
	}
}

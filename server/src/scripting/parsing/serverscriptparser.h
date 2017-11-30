#pragma once

#include "scripting/parsing/scriptparser.h"

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

				const std::string& rootFolder() const;

			private:
				std::string mRootFolder;
			};
		}
	}
}

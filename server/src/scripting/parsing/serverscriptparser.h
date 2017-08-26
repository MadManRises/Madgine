#pragma once

#include "scripting/parsing/scriptparser.h"

namespace Engine {
	namespace Scripting {
		namespace Parsing {

			class MADGINE_SERVER_EXPORT ServerScriptParser : public Scripting::Parsing::ScriptParser {
			public:
				ServerScriptParser(LuaState *state, const std::string &rootFolder);

				const std::string &rootFolder();

			private:
				std::string mRootFolder;
			};

		}
	}
}
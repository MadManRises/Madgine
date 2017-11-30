#include "serverlib.h"

#include "serverscriptparser.h"

#include "scripting/types/luastate.h"

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			ServerScriptParser::ServerScriptParser(LuaState* state, const std::string& rootFolder) :
				ScriptParser(state),
				mRootFolder(rootFolder)
			{
				for (auto& file : std::experimental::filesystem::recursive_directory_iterator(rootFolder))
				{
					/*, std::string("*.") + fileExtension())) {*/
					if (file.path().extension() == fileExtension())
					{
						std::ifstream in(file.path());
						parseScript(in, file.path().generic_string(), false);
					}
				}

				state->setFinalized();
			}

			const std::string& ServerScriptParser::rootFolder() const
			{
				return mRootFolder;
			}
		}
	}
}

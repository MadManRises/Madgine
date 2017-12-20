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
				
			}

			void ServerScriptParser::parse()
			{
				for (auto& file : std::experimental::filesystem::recursive_directory_iterator(mRootFolder))
				{
					/*, std::string("*.") + fileExtension())) {*/
					if (file.path().extension() == fileExtension())
					{
						std::ifstream in(file.path().string());
						parseScript(in, file.path().string(), false);
					}
				}

				state()->setFinalized();
			}

			const std::string& ServerScriptParser::rootFolder() const
			{
				return mRootFolder;
			}
		}
	}
}

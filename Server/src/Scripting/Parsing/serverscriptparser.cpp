#include "serverlib.h"

#include "serverscriptparser.h"

#include "Os\os.h"

#include <fstream>

namespace Engine {
	namespace Scripting {
		namespace Parsing {

			ServerScriptParser::ServerScriptParser(const std::string & rootFolder) :
				mRootFolder(rootFolder)
			{

				for (std::string path : Engine::Os::filesMatchingPattern(rootFolder, "*.script")) {
					std::ifstream in(path);
					parseScript(in, path, false);
				}

			}

			const std::string & ServerScriptParser::rootFolder()
			{
				return mRootFolder;
			}

		}
	}
}
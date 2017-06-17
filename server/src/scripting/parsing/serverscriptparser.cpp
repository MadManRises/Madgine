#include "serverlib.h"

#include "serverscriptparser.h"

#include "os/os.h"

namespace Engine {
	namespace Scripting {
		namespace Parsing {

			ServerScriptParser::ServerScriptParser(const std::string & rootFolder) :
				mRootFolder(rootFolder)
			{

				for (std::string path : Engine::Os::filesMatchingPattern(rootFolder, std::string("*.") + fileExtension())) {
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
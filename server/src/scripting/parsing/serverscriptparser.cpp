#include "serverlib.h"

#include "serverscriptparser.h"

namespace Engine {
	namespace Scripting {
		namespace Parsing {

			ServerScriptParser::ServerScriptParser(const std::string & rootFolder) :
				mRootFolder(rootFolder)
			{

				for (auto &file : std::experimental::filesystem::recursive_directory_iterator(rootFolder)){
					/*, std::string("*.") + fileExtension())) {*/
					if (file.path().extension() == fileExtension()) {
						std::ifstream in(file.path());
						parseScript(in, file.path().generic_string(), false);
					}
				}

				makeFinalized();

			}

			const std::string & ServerScriptParser::rootFolder()
			{
				return mRootFolder;
			}

		}
	}
}
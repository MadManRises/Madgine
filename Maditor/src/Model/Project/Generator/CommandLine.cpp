
#include "CommandLine.h"
#include <qdebug.h>
#include "OgreLogManager.h"

namespace Maditor {
	namespace Model {
		namespace Generator {

			int CommandLine::exec(const char* cmd) {
				static Ogre::Log *log = Ogre::LogManager::getSingleton().getLog("Project.log");
				log->logMessage(std::string("Executing Command: ") + cmd);

				char buffer[128];
				std::string result = "";
				FILE* pipe = _popen(cmd, "r");
				if (!pipe) throw std::runtime_error("popen() failed!");
				while (!feof(pipe)) {
					if (fgets(buffer, 128, pipe) != NULL)
						result += buffer;
				}
				log->logMessage(result);
				return _pclose(pipe);
				
			}

		}
	}
}
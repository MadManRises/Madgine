#include "maditormodellib.h"

#include "commandline.h"
//#include "Model\Engine\Watcher\LogWatcher.h"

#ifdef __linux__
#define _popen popen
#define _pclose pclose
#endif

namespace Maditor {
	namespace Model {

		namespace Generators {

			//Watcher::OgreLogWatcher *CommandLine::sLog = 0;

			std::pair<int, std::string> CommandLine::exec(std::string cmd) {
				LOG("Executing Command: "s + cmd);

#ifdef _WIN32
				cmd += " 2>&1";
#endif
				
				FILE* pipe = _popen(cmd.c_str(), "r");
				if (!pipe) throw std::runtime_error("popen() failed!");

				char buffer[128];
				std::string result;
				while (!feof(pipe)) {
					if (fgets(buffer, 128, pipe) != NULL)
						result += buffer;
				}
				return std::make_pair(_pclose(pipe), std::move(result));
				
			}



		}
	}
}

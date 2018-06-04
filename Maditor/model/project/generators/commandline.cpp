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

			int CommandLine::exec(const char* cmd) {
				LOG(std::string("Executing Command: ") + cmd);

				char buffer[128];
				std::string result = "result: ";
				FILE* pipe = _popen(cmd, "r");
				if (!pipe) throw std::runtime_error("popen() failed!");
				while (!feof(pipe)) {
					if (fgets(buffer, 128, pipe) != NULL)
						result += buffer;
				}
				int rVal = _pclose(pipe);
				if (rVal == 0)
					LOG(result);
				else
					LOG_ERROR(result);
				return rVal;
				
			}



		}
	}
}

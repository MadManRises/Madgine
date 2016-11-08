#include "madgineinclude.h"

#include "CommandLine.h"
#include "Model\Engine\Watcher\LogWatcher.h"

namespace Maditor {
	namespace Model {

		namespace Generator {

			Watcher::OgreLogWatcher *CommandLine::sLog = 0;

			int CommandLine::exec(const char* cmd) {
				if (sLog)
					sLog->logMessage(QString("Executing Command: ") + cmd);

				char buffer[128];
				QString result = "result: ";
				FILE* pipe = _popen(cmd, "r");
				if (!pipe) throw std::runtime_error("popen() failed!");
				while (!feof(pipe)) {
					if (fgets(buffer, 128, pipe) != NULL)
						result += buffer;
				}
				if (sLog)
					sLog->logMessage(result);
				return _pclose(pipe);
				
			}

			void CommandLine::setLog(Watcher::OgreLogWatcher * log)
			{
				sLog = log;
			}



		}
	}
}
#include "maditorinclude.h"

#include "CommandLine.h"
#include "Model/Project/ProjectLog.h"

namespace Maditor {
	namespace Model {

		namespace Generator {

			ProjectLog *CommandLine::sLog = 0;

			int CommandLine::exec(const char* cmd) {
				if (sLog)
					sLog->logMessage(std::string("Executing Command: ") + cmd);

				char buffer[128];
				std::string result = "";
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

			void CommandLine::setLog(ProjectLog * log)
			{
				sLog = log;
			}



		}
	}
}
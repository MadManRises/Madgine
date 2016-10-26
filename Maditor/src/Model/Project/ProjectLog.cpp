#include "maditorinclude.h"

#include "ProjectLog.h"


namespace Maditor {
	namespace Model {
		std::string ProjectLog::getName()
		{
			return "Project";
		}
		void ProjectLog::logMessage(const std::string & msg)
		{
			emit messageReceived(QString::fromStdString(msg));
		}
	}
}
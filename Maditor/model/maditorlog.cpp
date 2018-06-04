#include "maditormodellib.h"

#include "maditorlog.h"
#include "logs/logtablemodel.h"

namespace Maditor {
	namespace Model {
		MaditorLog::MaditorLog(LogTableModel *model) :
			mModel(model)
		{
		}

		void MaditorLog::log(const std::string & msg, Engine::Util::MessageType lvl, const std::list<Engine::Util::TraceBack>& traceBack)
		{			
			mModel->addMessage(msg.c_str(), lvl, getName().c_str(), "", "", -1);
		}

		std::string MaditorLog::getName()
		{
			return "Maditor.log";
		}

	}
}


#include "maditormodellib.h"

#include "logreceiver.h"

#include "logs/logtablemodel.h"

namespace Maditor {
	namespace Model {

		LogReceiver::LogReceiver(LogTableModel * model, const std::list<std::string> &textLogs) :
			mModel(model)
		{

		}

		LogReceiver::~LogReceiver()
		{
		}

		void LogReceiver::receiveImpl(const std::string & msg, Engine::Util::MessageType level, const std::string & logName, const std::string & fullTraceback, const std::string & fileName, int lineNr)
		{
			mModel->addMessage(msg.c_str(), level, logName.c_str(), fullTraceback.c_str(), fileName, lineNr);
		}

}
}

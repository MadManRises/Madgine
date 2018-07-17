#pragma once

#include "Madgine/serialize/container/action.h"

#include "Madgine/serialize/serializableunit.h"

namespace Maditor {
	namespace Model {

		class LogReceiver : public Engine::Serialize::SerializableUnit<LogReceiver> {

		public:
			LogReceiver(LogTableModel *model, const std::vector<std::string> &textLogs);
			virtual ~LogReceiver();

			void clear();

		private:
			void receiveImpl(const std::string &msg, Engine::Util::MessageType level, const std::string &logName, const std::string &fullTraceback, const std::string &fileName, int lineNr);
			

		private:
			
			LogTableModel *mModel;

			Engine::Serialize::Action<&LogReceiver::receiveImpl, Engine::Serialize::ActionPolicy::notification> receiveMessage;

			
		};
	}
}

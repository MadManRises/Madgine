#pragma once

#include "LogTableModel.h"
#include "Model\log.h"

#include "LogInfo.h"

Q_DECLARE_METATYPE(MessageType);
Q_DECLARE_METATYPE(Traceback);

namespace Maditor {
	namespace Model {
		namespace Watcher {


			class OgreLogWatcher : public Log {
				Q_OBJECT
			public:
				enum LogType {
					TextLog,
					GuiLog
				};

				OgreLogWatcher(LogType type, const std::string &name);
				~OgreLogWatcher();

				LogType type();

				virtual std::string getName() override;

				LogTableModel *model();

				void logMessage(const QString &msg, MessageType level = LOG_TYPE, const Traceback &traceback = {}, const QString &fullTraceback = "");

			signals:
				void ogreMessageReceived(const QString &msg, MessageType level = LOG_TYPE, const Traceback &traceback = {}, const QString &fullTraceback = "");

			private:
				std::unique_ptr<LogTableModel> mModel;
				LogType mType;
				std::string mName;


			};

		}
	}
}
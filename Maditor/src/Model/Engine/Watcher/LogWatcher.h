#pragma once

#include "OgreLog.h"
#include "LogTableModel.h"
#include <memory>


namespace Maditor {
	namespace Model {
		namespace Watcher {

			class LogWatcher : public QObject, public Ogre::LogListener {
				Q_OBJECT

			public:
				enum LogType {
					TextLog,
					GuiLog
				};
				
				LogWatcher(Ogre::Log *log, LogType type, const QString &root);
				~LogWatcher();

				const std::string &getName();

				LogTableModel *model();

				LogType type();

			signals:
				void messageReceived(const QString &msg, Ogre::LogMessageLevel level, const QList<Engine::Util::UtilMethods::TraceBack> &traceback);
				void openScriptFile(const QString &path, int line);

			private:
				Ogre::Log *mLog;
				LogType mType;

				// Geerbt über LogListener
				virtual void messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage) override;

				std::unique_ptr<LogTableModel> mModel;

			};

		}
	}
}
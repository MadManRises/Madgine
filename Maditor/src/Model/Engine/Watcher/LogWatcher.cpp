#include "LogWatcher.h"
#include <qapplication.h>


namespace Maditor {
	namespace Model {
		namespace Watcher {

			LogWatcher::LogWatcher(Ogre::Log * log, LogType type, const QString &root) :
				mLog(log),
				mType(type)
			{
				mLog->addListener(this);
				if (type == GuiLog) {
					mModel = std::unique_ptr<LogTableModel>(new LogTableModel(root));
					mModel->moveToThread(QApplication::instance()->thread());
					connect(this, &Model::Watcher::LogWatcher::messageReceived, mModel.get(), &Model::Watcher::LogTableModel::addMessage, Qt::QueuedConnection);
					connect(mModel.get(), &LogTableModel::openScriptFile, this, &LogWatcher::openScriptFile);
				}
			}

			LogWatcher::~LogWatcher()
			{
				mLog->removeListener(this);
			}

			const std::string & LogWatcher::getName()
			{
				return mLog->getName();
			}

			LogTableModel * LogWatcher::model()
			{
				return mModel.get();
			}

			LogWatcher::LogType LogWatcher::type()
			{
				return mType;
			}

			void LogWatcher::messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage)
			{
				QList<Engine::Util::UtilMethods::TraceBack> list;
				for (const Engine::Util::UtilMethods::TraceBack &t : Engine::Util::UtilMethods::traceBack()) {
					list << t;
				}
				emit messageReceived(message.c_str(), lml, list);
			}

		}
	}
}
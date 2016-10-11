#include "LogWatcher.h"
#include <qapplication.h>
#include "LogsWatcher.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {

			void LogsWatcher::setup(const QString &root) {
				Ogre::LogManager::getSingleton().createLog("Project.log");
				addLogWatcher("Project.log", LogWatcher::TextLog, root);
				addLogWatcher("Ogre.log", LogWatcher::TextLog, root);
				addLogWatcher("Madgine.log", LogWatcher::GuiLog, root);
			}

			void LogsWatcher::cleanUp()
			{
				mLogWatchers.clear();
			}

			void LogsWatcher::addLogWatcher(const std::string & name, LogWatcher::LogType type, const QString &root)
			{
				LogWatcher *watcher = new LogWatcher(Ogre::LogManager::getSingleton().getLog(name), type, root);
				watcher->moveToThread(QApplication::instance()->thread());
				mLogWatchers.emplace_back(watcher);
				connect(watcher, &LogWatcher::openScriptFile, this, &LogsWatcher::openScriptFile);
				emit logWatcherCreated(watcher);
			}

		}
	}
}
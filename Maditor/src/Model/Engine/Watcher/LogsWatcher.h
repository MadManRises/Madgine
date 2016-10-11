#pragma once

#include <memory>

#include "LogWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			class LogsWatcher : public QObject {
				Q_OBJECT

			public:
				void setup(const QString &root);
				void cleanUp();

				void addLogWatcher(const std::string &name, LogWatcher::LogType type, const QString &root);

			signals:
				void logWatcherCreated(LogWatcher *watcher);
				void openScriptFile(const QString &path, int line);

			private:
				std::list<std::unique_ptr<LogWatcher>> mLogWatchers;

			};

		}
	}
}
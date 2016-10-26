#pragma once


namespace Maditor {
	namespace Model {

			class LogsModel : public QObject {
				Q_OBJECT

			public:

				void addLog(Log *log);
				void removeLog(Log *log);
				const std::list<Log*> &logs() const;

			signals:
				void logWatcherCreated(Log *log);
				void logWatcherDeleted(Log *log);
				void openScriptFile(const QString &path, int line);

			private:
				std::list<Log*> mLogs;

			};

	}
}
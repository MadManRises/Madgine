#include "maditorlib.h"

#include "Log.h"

#include "LogsModel.h"

namespace Maditor {
	namespace Model {


			void LogsModel::addLog(Log * log)
			{
				mLogs.emplace_back(log);
				emit logWatcherCreated(log);
			}

			void LogsModel::removeLog(Log * log)
			{
				auto it = std::find(mLogs.begin(), mLogs.end(), log);
				emit logWatcherDeleted(log);
				mLogs.erase(it);
			}

			const std::list<Log*> &LogsModel::logs() const{
				return mLogs;
			}
	}
}
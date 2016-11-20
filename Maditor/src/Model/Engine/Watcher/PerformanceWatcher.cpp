#include "maditorlib.h"

#include "PerformanceWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			PerformanceWatcher::PerformanceWatcher() :
				TreeModel(&mRootItem, 4)
			{
				startTimer(3000);
			}

			void PerformanceWatcher::clear()
			{
				beginResetModel();
				mRootItem.clear();
				endResetModel();
			}

			void PerformanceWatcher::timerEvent(QTimerEvent * event)
			{

				mRootItem.update(this);
				emit dataChanged(index(0, 1), index(rowCount() - 1, 3));
			}

			QVariant PerformanceWatcher::headerData(int section, Qt::Orientation orientation, int role) const
			{
				if (role != Qt::DisplayRole)
					return QVariant();

				if (orientation == Qt::Horizontal)
					return RootProfilerNode::header(section);
				else
					return QVariant();
			}



			
		}
	}
}
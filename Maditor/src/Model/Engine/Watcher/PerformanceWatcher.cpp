#include "madgineinclude.h"

#include "PerformanceWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			PerformanceWatcher::PerformanceWatcher() :
				TreeModel(&mRootItem, 4),
				mUpdatePending(false)
			{
				startTimer(3000);

				connect(this, &PerformanceWatcher::dataChangedQueued, this, &PerformanceWatcher::updateData, Qt::QueuedConnection);
				connect(this, &PerformanceWatcher::resetModelQueued, this, &PerformanceWatcher::resetModel, Qt::QueuedConnection);
			}

			void PerformanceWatcher::resetModel()
			{
				beginResetModel();
				mRootItem.clear();
				endResetModel();
			}

			void PerformanceWatcher::timerEvent(QTimerEvent * event)
			{
				mUpdatePending = true;
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

			void PerformanceWatcher::update()
			{
				if (mUpdatePending) {
					mUpdatePending = false;

					mRootItem.update(this);
					
					emit dataChangedQueued();					
				}
			}

			void PerformanceWatcher::clear()
			{
				emit resetModelQueued();
			}


			void PerformanceWatcher::updateData() {
				emit dataChanged(index(0, 1), index(rowCount() - 1, 3));
				//emit dataChanged(QModelIndex(), QModelIndex());
			}

			
		}
	}
}
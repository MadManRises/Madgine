#include "madgineinclude.h"

#include "OgreSceneWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			OgreSceneWatcher::OgreSceneWatcher() :
				mUpdatePending(false),
				mRootItem(&mMirroredRootItem),
				TreeModel(&mRootItem, 1)
				
			 {
				connect(this, &OgreSceneWatcher::modelChanged, this, &OgreSceneWatcher::onModelChanged, Qt::QueuedConnection);
				
				startTimer(3000);
			}

			void OgreSceneWatcher::timerEvent(QTimerEvent * event)
			{				
				mUpdatePending = true;
			}

			QVariant OgreSceneWatcher::headerData(int section, Qt::Orientation orientation, int role) const
			{
				if (role != Qt::DisplayRole)
					return QVariant();

				if (orientation == Qt::Horizontal)
					return "Node";
				else
					return QVariant();
			}

			void OgreSceneWatcher::performRowsRemove(const QModelIndex & parent, int from, int to)
			{
				TreeModel::performRowsRemove(parent, from, to);
			}

			void OgreSceneWatcher::update()
			{
				if (mUpdatePending) {
					mUpdatePending = false;
					mMutex.lock();
					mMirroredRootItem.update();
					mMutex.unlock();
					emit modelChanged();
				}
			}

			void OgreSceneWatcher::clear()
			{
				beginResetModel();
				mRootItem.clear();
				endResetModel();
			}

			void OgreSceneWatcher::onModelChanged() {
				mMutex.lock();
				mRootItem.update(this, QModelIndex());
				mMutex.unlock();
			}


			
		}
	}
}
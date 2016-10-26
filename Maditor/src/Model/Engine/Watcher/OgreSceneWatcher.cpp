#include "maditorinclude.h"

#include "OgreSceneWatcher.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			OgreSceneWatcher::OgreSceneWatcher() :
				mUpdatePending(false),
				TreeModel(&mRootItem, 1)
			 {
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

			void OgreSceneWatcher::update()
			{
				if (mUpdatePending) {
					mUpdatePending = false;
					mRootItem.update(this, QModelIndex());
				}
			}


			
		}
	}
}
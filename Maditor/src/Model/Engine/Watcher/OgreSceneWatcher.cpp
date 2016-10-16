#include "OgreSceneWatcher.h"
#include <qdebug.h>

#include "libinclude.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {

			OgreSceneWatcher::OgreSceneWatcher() :
				mUpdatePending(false),
				mRootSceneNode(0)
			 {
				startTimer(3000);

				connect(this, &OgreSceneWatcher::dataChangedQueued, this, &OgreSceneWatcher::updateData, Qt::QueuedConnection);


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
					
					emit dataChangedQueued();					
				}
			}

			void OgreSceneWatcher::setRoot(Ogre::SceneNode * node)
			{
				mRootSceneNode = node;
			}

			void OgreSceneWatcher::updateData() {
				beginResetModel();
				endResetModel();
			}

			Q_INVOKABLE QModelIndex OgreSceneWatcher::index(int row, int column, const QModelIndex & parent) const
			{
				if (!hasIndex(row, column, parent))
					return QModelIndex();

				Ogre::SceneNode *node;
				if (!parent.isValid()) {
					node = mRootSceneNode;
				}
				else {
					node = static_cast<Ogre::SceneNode*>(parent.internalPointer());
				}


				if (node->numChildren() > row) {
					return createIndex(row, column, static_cast<Ogre::SceneNode*>(node->getChild(row)));
				}
				else {
					return QModelIndex();
				}
			}

			Q_INVOKABLE QModelIndex OgreSceneWatcher::parent(const QModelIndex & child) const
			{
				if (!child.isValid())
					return QModelIndex();


				Ogre::SceneNode *node = static_cast<Ogre::SceneNode*>(child.internalPointer());
				if (node == mRootSceneNode)
					return QModelIndex();

				return createIndex(0, 0, node->getParentSceneNode()); //TODO
			}

			Q_INVOKABLE int OgreSceneWatcher::rowCount(const QModelIndex & parent) const
			{
				Ogre::SceneNode *node;
				if (!parent.isValid()) {
					node = mRootSceneNode;
					if (!node)
						return 0;
				}
				else {
					node = static_cast<Ogre::SceneNode*>(parent.internalPointer());
				}
				return node->numChildren();
			}

			Q_INVOKABLE int OgreSceneWatcher::columnCount(const QModelIndex & parent) const
			{
				return 1;
			}

			Q_INVOKABLE QVariant OgreSceneWatcher::data(const QModelIndex & index, int role) const
			{
				if (!index.isValid())
					return QVariant();

				if (role != Qt::DisplayRole)
					return QVariant();


				Ogre::SceneNode *node = static_cast<Ogre::SceneNode*>(index.internalPointer());

				return node->getName().c_str();

			}
		}
	}
}
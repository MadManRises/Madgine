#include "OgreSceneWatcher.h"
#include <qdebug.h>

#include "libinclude.h"
#include "Scene\scenemanager.h"
#include "OgreNodeItem.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {
			OgreNodeItem::OgreNodeItem(OgreSceneNodeItem *parent, const std::string &name) :
				mName(name),
				mParent(parent)
			{
			}

			QVariant OgreNodeItem::data(int col) const
			{
				return mName.c_str();
			}

			OgreNodeItem * OgreNodeItem::parentItem()
			{
				return mParent;
			}

			OgreSceneNodeItem::OgreSceneNodeItem(OgreSceneNodeItem * parent, Ogre::SceneNode * node) :
				OgreNodeItem(parent, node->getName()),
				mNode(node)
			{
			}
			OgreSceneNodeItem::~OgreSceneNodeItem()
			{
				for (OgreEntityItem *e : mObjects)
					delete e;
				for (OgreSceneNodeItem *n : mNodes)
					delete n;
			}
			void OgreSceneNodeItem::update(OgreSceneWatcher *watcher, const QModelIndex &index)
			{
				Ogre::SceneNode *node = getNode();

				std::set<Ogre::SceneNode*> nodes;
				for (OgreSceneNodeItem *n : mNodes) {
					nodes.insert(n->getNode());
				}
				std::set<Ogre::SceneNode *> newNodes;
				for (const std::pair<Ogre::String, Ogre::Node *> &n : node->getChildIterator()) {
					newNodes.insert(static_cast<Ogre::SceneNode*>(n.second));
				}

				std::list<Ogre::SceneNode*> removedNodes;
				std::set_difference(nodes.begin(), nodes.end(), newNodes.begin(), newNodes.end(),
					std::inserter(removedNodes, removedNodes.begin()));

				for (Ogre::SceneNode *n : removedNodes) {
					auto it = std::find_if(mNodes.begin(), mNodes.end(), [=](OgreSceneNodeItem *item) {return item->getNode() == n; });
					watcher->beginRemoveRows(index, std::distance(mNodes.begin(), it), std::distance(mNodes.begin(), it));
					(*it)->deleteLater();
					mNodes.erase(it);
					watcher->endRemoveRows();
				}
				
				int i = 0;
				for (OgreSceneNodeItem *n : mNodes) {
					n->update(watcher, watcher->index(i, 0, index));
					++i;
				}

				std::list<Ogre::SceneNode*> addedNodes;
				std::set_difference(newNodes.begin(), newNodes.end(), nodes.begin(), nodes.end(),
					std::inserter(addedNodes, addedNodes.begin()));

				if (!addedNodes.empty()) {
					watcher->beginInsertRows(index, mNodes.size(), mNodes.size() + addedNodes.size() - 1);
					for (Ogre::SceneNode* n : addedNodes) {
						mNodes.push_back(new OgreSceneNodeItem(this, n));
						mNodes.back()->update(watcher, watcher->index(mNodes.size() - 1, 0, index));
					}
					watcher->endInsertRows();
				}

				std::set<Ogre::MovableObject*> objects;
				for (OgreEntityItem *o : mObjects) {
					objects.insert(o->object());
				}
				std::set<Ogre::MovableObject*> newObjects;
				for (const std::pair<Ogre::String, Ogre::MovableObject*> &o : node->getAttachedObjectIterator()) {
					newObjects.insert(o.second);
				}

				std::list<Ogre::MovableObject*> removedObjects;
				std::set_difference(objects.begin(), objects.end(), newObjects.begin(), newObjects.end(),
					std::inserter(removedObjects, removedObjects.begin()));

				for (Ogre::MovableObject *o : removedObjects) {
					auto it = std::find_if(mObjects.begin(), mObjects.end(), [=](OgreEntityItem *item) {return item->object() == o; });
					watcher->beginRemoveRows(index, std::distance(mObjects.begin(), it) + mNodes.size(), std::distance(mObjects.begin(), it) + mNodes.size());
					(*it)->deleteLater();
					mObjects.erase(it);
					watcher->endRemoveRows();					
				}

				std::list<Ogre::MovableObject*> addedObjects;
				std::set_difference(newObjects.begin(), newObjects.end(), objects.begin(), objects.end(),
					std::inserter(addedObjects, addedObjects.begin()));

				if (!addedObjects.empty()) {
					watcher->beginInsertRows(index, mNodes.size() + mObjects.size(), mNodes.size() + mObjects.size() + addedObjects.size() - 1);
					for (Ogre::MovableObject* o : addedObjects) {
						mObjects.push_back(new OgreEntityItem(this, o));
					}
					watcher->endInsertRows();
				}


			}
			OgreSceneNodeItem::OgreSceneNodeItem() :
				OgreNodeItem(0, "")
			{
			}
			int OgreSceneNodeItem::childCount()
			{
				return mNodes.size() + mObjects.size();
			}
			TreeItem * OgreSceneNodeItem::child(int i)
			{
				if (i < mNodes.size()) {
					auto it = mNodes.begin();
					std::advance(it, i);
					return *it;
				}
				else {
					i -= mNodes.size();
					auto it = mObjects.begin();
					std::advance(it, i);
					return *it;
				}
			}
			Ogre::SceneNode * OgreSceneNodeItem::getNode()
			{
				return mNode;
			}
			Ogre::SceneNode * OgreRootSceneNodeItem::getNode()
			{
				return Engine::Scene::SceneManager::getSingleton().getSceneManager()->getRootSceneNode();
			}

			OgreEntityItem::OgreEntityItem(OgreSceneNodeItem *parent, Ogre::MovableObject * o) :
				OgreNodeItem(parent, o->getName()),
				mObject(o)
			{
			}

			Ogre::MovableObject * OgreEntityItem::object()
			{
				return mObject;
			}

			int OgreEntityItem::childCount()
			{
				return 0;
			}

			TreeItem * OgreEntityItem::child(int i)
			{
				throw 0;
			}

		}
	}
}
#include "madgineinclude.h"

#include "OgreSceneWatcher.h"

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
				return QString::fromStdString(mName);
			}

			

			OgreNodeItem * OgreNodeItem::parentItem()
			{
				return mParent;
			}

			OgreSceneNodeItem::OgreSceneNodeItem(OgreSceneNodeItem * parent, Ogre::SceneNode * node) :
				OgreNodeItem(parent, node->getName()),
				mNode(node)
			{
				for (const std::pair<Ogre::String, Ogre::Node *> &n : node->getChildIterator()) {
					mNodes.emplace_back(this, static_cast<Ogre::SceneNode*>(n.second));
				}
				for (const std::pair<Ogre::String, Ogre::MovableObject*> &o : node->getAttachedObjectIterator()) {
					mObjects.emplace_back(this, o.second);
				}
			}
			OgreSceneNodeItem::~OgreSceneNodeItem()
			{
			}
			void OgreSceneNodeItem::update(OgreSceneWatcher *watcher, const QModelIndex &index)
			{
				Ogre::SceneNode *node = getNode();

				std::set<Ogre::SceneNode*> nodes;
				for (OgreSceneNodeItem &n : mNodes) {
					nodes.insert(n.getNode());
				}
				std::set<Ogre::SceneNode *> newNodes;
				for (const std::pair<Ogre::String, Ogre::Node *> &n : node->getChildIterator()) {
					newNodes.insert(static_cast<Ogre::SceneNode*>(n.second));
				}

				std::list<Ogre::SceneNode*> removedNodes;
				std::set_difference(nodes.begin(), nodes.end(), newNodes.begin(), newNodes.end(),
					std::inserter(removedNodes, removedNodes.begin()));

				for (Ogre::SceneNode *n : removedNodes) {
					auto it = std::find_if(mNodes.begin(), mNodes.end(), [=](OgreSceneNodeItem &item) {return item.getNode() == n; });
					
					it->deleteLater();
					int i = std::distance(mNodes.begin(), it);
					mNodes.erase(it);
					watcher->removeRowsQueued(index, i, i);
				}
				
				int i = 0;
				for (OgreSceneNodeItem &n : mNodes) {
					n.update(watcher, watcher->index(i, 0, index));
					++i;
				}

				std::list<Ogre::SceneNode*> addedNodes;
				std::set_difference(newNodes.begin(), newNodes.end(), nodes.begin(), nodes.end(),
					std::inserter(addedNodes, addedNodes.begin()));

				if (!addedNodes.empty()) {
					int oldCount = mNodes.size();
					for (Ogre::SceneNode* n : addedNodes) {
						mNodes.emplace_back(this, n);
					}
					watcher->insertRowsQueued(index, oldCount, mNodes.size() - 1);
				}

				std::set<Ogre::MovableObject*> objects;
				for (OgreEntityItem &o : mObjects) {
					objects.insert(o.object());
				}
				std::set<Ogre::MovableObject*> newObjects;
				for (const std::pair<Ogre::String, Ogre::MovableObject*> &o : node->getAttachedObjectIterator()) {
					newObjects.insert(o.second);
				}

				std::list<Ogre::MovableObject*> removedObjects;
				std::set_difference(objects.begin(), objects.end(), newObjects.begin(), newObjects.end(),
					std::inserter(removedObjects, removedObjects.begin()));

				for (Ogre::MovableObject *o : removedObjects) {
					auto it = std::find_if(mObjects.begin(), mObjects.end(), [=](OgreEntityItem &item) {return item.object() == o; });
					it->deleteLater();
					mObjects.erase(it);
					watcher->removeRowsQueued(index, std::distance(mObjects.begin(), it) + mNodes.size(), std::distance(mObjects.begin(), it) + mNodes.size());
				}

				std::list<Ogre::MovableObject*> addedObjects;
				std::set_difference(newObjects.begin(), newObjects.end(), objects.begin(), objects.end(),
					std::inserter(addedObjects, addedObjects.begin()));

				if (!addedObjects.empty()) {
					int oldCount = mObjects.size();
					for (Ogre::MovableObject* o : addedObjects) {
						mObjects.emplace_back(this, o);
					}
					watcher->insertRowsQueued(index, mNodes.size() + oldCount, mNodes.size() + mObjects.size() - 1);
				}


			}
			void OgreSceneNodeItem::clear()
			{
				mObjects.clear();
				mNodes.clear();
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
					return &*it;
				}
				else {
					i -= mNodes.size();
					auto it = mObjects.begin();
					std::advance(it, i);
					return &*it;
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
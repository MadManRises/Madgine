#include "madgineinclude.h"

#include "OgreSceneWatcher.h"

#include "OgreNodeItem.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {
			OgreMirroredNodeItem::OgreMirroredNodeItem(const std::string &name) :
				mName(name)
			{



			}

			QString OgreMirroredNodeItem::name() const
			{
				return QString::fromStdString(mName);
			}

			std::set<OgreMirroredNodeItem*> OgreMirroredNodeItem::getChildren()
			{
				return std::set<OgreMirroredNodeItem*>();
			}


			OgreMirroredSceneNodeItem::OgreMirroredSceneNodeItem(Ogre::SceneNode * node) :
				OgreMirroredNodeItem(node->getName()),
				mNode(node)
			{
				for (const std::pair<Ogre::String, Ogre::Node *> &n : node->getChildIterator()) {
					mNodes.emplace_back(static_cast<Ogre::SceneNode*>(n.second));
				}
				for (const std::pair<Ogre::String, Ogre::MovableObject*> &o : node->getAttachedObjectIterator()) {
					mObjects.emplace_back(o.second->getName());
				}
			}

			void OgreMirroredSceneNodeItem::update()
			{
				std::set<Ogre::SceneNode *> nodes;
				for (OgreMirroredSceneNodeItem &n : mNodes) {
					nodes.insert(n.getNode());
				}

				std::set<Ogre::SceneNode*> newNodes;
				Ogre::SceneNode *node = getNode();
				for (const std::pair<Ogre::String, Ogre::Node *> &n : node->getChildIterator()) {
					newNodes.insert(static_cast<Ogre::SceneNode*>(n.second));
				}
				
				std::list<Ogre::SceneNode*> removedNodes;
				std::set_difference(nodes.begin(), nodes.end(), newNodes.begin(), newNodes.end(),
					std::inserter(removedNodes, removedNodes.begin()));
				for (Ogre::SceneNode* n : removedNodes) {
					auto it = std::find_if(mNodes.begin(), mNodes.end(), [=](OgreMirroredSceneNodeItem &item) {return item.getNode() == n; });
					mNodes.erase(it);
				}
				
				std::list<Ogre::SceneNode*> addedNodes;
				std::set_difference(newNodes.begin(), newNodes.end(), nodes.begin(), nodes.end(),
				std::inserter(addedNodes, addedNodes.begin()));
				for (Ogre::SceneNode* n : addedNodes){
					mNodes.emplace_back(n);
				}

				for (OgreMirroredSceneNodeItem &n : mNodes)
					n.update();
				
				mObjects.clear();
				for (const std::pair<Ogre::String, Ogre::MovableObject*> &o : node->getAttachedObjectIterator()) {
					mObjects.emplace_back(o.second->getName());
				}
			}
			std::set<OgreMirroredNodeItem*> OgreMirroredSceneNodeItem::getChildren()
			{
				std::set<OgreMirroredNodeItem*> result;
				for (OgreMirroredNodeItem &o : mObjects) {
					result.insert(&o);
				}
				for (OgreMirroredSceneNodeItem &n : mNodes) {
					result.insert(&n);
				}
				return result;
			}
			OgreMirroredSceneNodeItem::OgreMirroredSceneNodeItem() :
				OgreMirroredNodeItem("")
			{
			}
			void OgreNodeItem::clear()
			{
				mChildren.clear();
			}
			
			Ogre::SceneNode * OgreMirroredSceneNodeItem::getNode()
			{
				return mNode;
			}
			Ogre::SceneNode * OgreMirroredRootSceneNodeItem::getNode()
			{
				return Engine::Scene::SceneManager::getSingleton().getSceneManager()->getRootSceneNode();
			}

			OgreNodeItem::OgreNodeItem(OgreMirroredNodeItem * mirror, OgreNodeItem * parent) :
				mMirror(mirror),
				mParent(parent),
				mName(mirror->name())
			{
			}

			void OgreNodeItem::update(OgreSceneWatcher * watcher, const QModelIndex & index)
			{

				std::set<OgreMirroredNodeItem*> nodes;
				for (OgreNodeItem &n : mChildren) {
					nodes.insert(n.getMirror());
				}

				std::set<OgreMirroredNodeItem*> mirroredChildren = mMirror->getChildren();
				std::list<OgreMirroredNodeItem*> removedNodes;
				std::set_difference(nodes.begin(), nodes.end(), mirroredChildren.begin(), mirroredChildren.end(),
					std::inserter(removedNodes, removedNodes.begin()));

				for (OgreMirroredNodeItem *n : removedNodes) {
					auto it = std::find_if(mChildren.begin(), mChildren.end(), [=](OgreNodeItem &item) {return item.getMirror() == n; });

					int i = std::distance(mChildren.begin(), it);
					watcher->beginRemoveRows(index, i, i);
					mChildren.erase(it);
					watcher->endRemoveRows();
				}



				std::list<OgreMirroredNodeItem*> addedNodes;
				std::set_difference(mirroredChildren.begin(), mirroredChildren.end(), nodes.begin(), nodes.end(),
					std::inserter(addedNodes, addedNodes.begin()));

				if (!addedNodes.empty()) {
					watcher->beginInsertRows(index, mChildren.size(), mChildren.size() + addedNodes.size() - 1);
					for (OgreMirroredNodeItem* n : addedNodes) {
						mChildren.emplace_back(n, this);
					}
					watcher->endInsertRows();
				}

				int i = 0;
				for (OgreNodeItem &n : mChildren) {
					n.update(watcher, watcher->index(i, 0, index));
					++i;
				}

			}

			OgreMirroredNodeItem * OgreNodeItem::getMirror()
			{
				return mMirror;
			}

			int OgreNodeItem::childCount()
			{
				return mChildren.size();
			}

			TreeItem * OgreNodeItem::child(int i)
			{
				auto it = mChildren.begin();
				std::advance(it, i);
				return &*it;
			}

			TreeItem * OgreNodeItem::parentItem()
			{
				return mParent;
			}

			QVariant OgreNodeItem::data(int col) const
			{
				return mName;
			}

		}
	}
}
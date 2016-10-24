#include "ResourceGroupItem.h"

#include <OgreResourceManager.h>
#include "ResourceItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			ResourceGroupItem::ResourceGroupItem(TreeItem * parent, Ogre::ResourceManager *manager) :
				mName(manager->getResourceType().c_str()),
				mParent(parent)
			{
				for (const std::pair<Ogre::ResourceHandle, Ogre::ResourcePtr> &res : manager->getResourceIterator()) {
					QString name = res.second->getName().c_str();
					
					if (name.contains("::")) {
						ResourceItem *parent = mResources.find(name.mid(0, name.lastIndexOf("::")).toStdString())->second;
						parent->addChild(new ResourceItem(parent, res.second, name.mid(name.lastIndexOf("::") + 2)));
					}
					else {
						mResources[name.toStdString()] = new ResourceItem(this, res.second, name);
					}				

				}
			}

			ResourceGroupItem::~ResourceGroupItem()
			{
				for (const std::pair<const std::string, ResourceItem*> &p : mResources)
					delete p.second;
			}

			int ResourceGroupItem::childCount()
			{
				return mResources.size();
			}

			TreeItem * ResourceGroupItem::child(int i)
			{
				auto it = mResources.begin();
				std::advance(it, i);
				return it->second;
			}

			TreeItem * ResourceGroupItem::parentItem()
			{
				return mParent;
			}

			QVariant ResourceGroupItem::data(int col) const
			{
				return mName;
			}

		}
	}
}
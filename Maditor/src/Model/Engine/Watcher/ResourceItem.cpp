#include "maditorinclude.h"

#include "ResourceItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			ResourceItem::ResourceItem(TreeItem * parent, const Ogre::ResourcePtr & resource, const QString &name) :
				mResource(resource),
				mName(name),
				mParent(parent)
			{
			}

			ResourceItem::~ResourceItem()
			{
				for (ResourceItem *child : mChildren)
					delete child;
			}

			const Ogre::ResourcePtr & ResourceItem::resource()
			{
				return mResource;
			}
			int ResourceItem::childCount()
			{
				return mChildren.size();
			}
			TreeItem * ResourceItem::child(int i)
			{
				auto it = mChildren.begin();
				std::advance(it, i);
				return *it;
			}
			TreeItem * ResourceItem::parentItem()
			{
				return mParent;
			}
			QVariant ResourceItem::data(int col) const
			{
				return mName;
			}
			void ResourceItem::addChild(ResourceItem * child)
			{
				mChildren.push_back(child);
			}
		}
	}
}
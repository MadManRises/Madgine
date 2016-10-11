#include <libinclude.h>

#include "ResourceItem.h"
#include "ResourceGroupItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			ResourceItem::ResourceItem(TreeItem * parent, const Ogre::ResourcePtr & resource, const QString &name) :
				TreeItem(parent),
				mResource(resource),
				mName(name)
			{
			}
			const QString &ResourceItem::name()
			{
				return mName;
			}
			const Ogre::ResourcePtr & ResourceItem::resource()
			{
				return mResource;
			}
		}
	}
}
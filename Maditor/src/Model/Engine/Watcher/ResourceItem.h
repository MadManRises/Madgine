#pragma once

#include "Model\TreeItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			class ResourceGroupItem;

			class ResourceItem : public TreeItem {
			public:
				ResourceItem(TreeItem *parent, const Ogre::ResourcePtr &resource, const QString &name);

				const QString &name();
				const Ogre::ResourcePtr &resource();

			private:
				Ogre::ResourcePtr mResource;
				QString mName;
			};
		}
	}
}
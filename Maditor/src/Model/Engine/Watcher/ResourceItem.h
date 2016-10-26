#pragma once

#include "Model\TreeItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			class ResourceItem : public TreeItem {
			public:
				ResourceItem(TreeItem *parent, const Ogre::ResourcePtr &resource, const QString &name);
				virtual ~ResourceItem();

				const Ogre::ResourcePtr &resource();

				// Inherited via TreeItem
				virtual int childCount() override;
				virtual TreeItem * child(int i) override;
				virtual TreeItem * parentItem() override;
				virtual QVariant data(int col) const override;

				void addChild(ResourceItem *child);

			private:
				Ogre::ResourcePtr mResource;
				std::list<ResourceItem *> mChildren;
				QString mName;
				TreeItem *mParent;


			};
		}
	}
}
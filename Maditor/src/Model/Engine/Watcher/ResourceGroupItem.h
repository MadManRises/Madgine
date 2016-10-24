#pragma once

#include "Model\TreeItem.h"
#include <unordered_map>

namespace Ogre {
	class ResourceManager;
}

namespace Maditor {
	namespace Model {
		namespace Watcher {
			class ResourceItem;

			class ResourceGroupItem : public TreeItem {
			public:
				ResourceGroupItem(TreeItem *parent, Ogre::ResourceManager *manager);
				virtual ~ResourceGroupItem();

				// Inherited via TreeItem
				virtual int childCount() override;

				virtual TreeItem * child(int i) override;

				virtual TreeItem * parentItem() override;

				virtual QVariant data(int col) const override;


			private:
				QString mName;

				std::unordered_map<std::string, ResourceItem*> mResources;
				TreeItem *mParent;

			};
		}
	}
}
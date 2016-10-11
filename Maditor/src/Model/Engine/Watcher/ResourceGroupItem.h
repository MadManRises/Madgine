#pragma once

#include "Model\TreeItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			class ResourceGroupItem : public TreeItem {
			public:
				ResourceGroupItem(TreeItem *parent, const QString &name);

				const QString &name();

			private:
				QString mName;
			};
		}
	}
}
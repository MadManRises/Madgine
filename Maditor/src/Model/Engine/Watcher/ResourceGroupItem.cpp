#include "ResourceGroupItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {
			ResourceGroupItem::ResourceGroupItem(TreeItem * parent, const QString & name) :
				TreeItem(parent),
				mName(name)
			{
			}
			const QString & ResourceGroupItem::name()
			{
				return mName;
			}
		}
	}
}
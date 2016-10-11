

#include <QStringList>

#include "treeitem.h"

namespace Maditor {
	namespace Model {


		TreeItem::TreeItem(TreeItem * parent) :
			mParentItem(parent)
		{
			if (parent)
				parent->mChildren.push_back(this);
		}


		void TreeItem::extendContextMenu(QMenu & menu)
		{
			if (mContextMenuItems.empty()) return;
			if (!menu.actions().isEmpty())
				menu.addSeparator();
			for (const std::pair<QString, std::function<void()>> &p : mContextMenuItems) {
				menu.addAction(p.first, p.second);
			}
		}

		int TreeItem::childCount()
		{
			return mChildren.size();
		}
		int TreeItem::index()
		{
			if (mParentItem) {
				auto it = std::find(mParentItem->mChildren.begin(), mParentItem->mChildren.end(), this);
				return std::distance(mParentItem->mChildren.begin(), it);
			}
			else {
				return 0;
			}
		}
		TreeItem * TreeItem::child(int i) const
		{
			auto it = mChildren.begin();
			std::advance(it, i);
			return *it;
		}
		TreeItem * TreeItem::parentItem()
		{
			return mParentItem;
		}

		void TreeItem::setContextMenuItems(std::list<std::pair<QString, std::function<void()>>>&& contextMenuItems)
		{
			mContextMenuItems = std::forward<std::list<std::pair<QString, std::function<void()>>>>(contextMenuItems);
		}

	}
}

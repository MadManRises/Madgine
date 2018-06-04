#include "maditormodellib.h"

#include "treeitem.h"

namespace Maditor {
	namespace Model {


		void TreeItem::extendContextMenu(QMenu & menu)
		{
			if (parentItem()) {
				parentItem()->extendContextMenu(menu);
			}
			if (!mContextMenuItems.empty()) {
				if (!menu.actions().isEmpty())
					menu.addSeparator();
				for (const std::pair<QString, std::function<void()>> &p : mContextMenuItems) {
					menu.addAction(p.first, p.second);
				}
			}
		}

		void TreeItem::doubleClicked()
		{
		}		

		void TreeItem::setContextMenuItems(std::list<std::pair<QString, std::function<void()>>>&& contextMenuItems)
		{
			mContextMenuItems = std::forward<std::list<std::pair<QString, std::function<void()>>>>(contextMenuItems);
		}

		const std::list<std::pair<QString, std::function<void()>>>& TreeItem::getContextMenuItems()
		{
			return mContextMenuItems;
		}

		int TreeItem::childCount() const
		{
			return 0;
		}

		int TreeItem::parentIndex()
		{
			int i;
			TreeItem *parent = parentItem();
			int count = parent->childCount();
			for (i = 0; i < count; ++i) {
				if (parent->child(i) == this)
					return i;
			}
			throw 0;
		}

		QVariant TreeItem::icon() const
		{
			return QVariant();
		}
		

	}
}

#include "maditorlib.h"

#include "treeitem.h"

namespace Maditor {
	namespace Model {


		void TreeItem::extendContextMenu(QMenu & menu)
		{
			if (parentItem()) {
				parentItem()->extendContextMenu(menu);
			}
			if (mContextMenuItems.empty()) return;
			if (!menu.actions().isEmpty())
				menu.addSeparator();
			for (const std::pair<QString, std::function<void()>> &p : mContextMenuItems) {
				menu.addAction(p.first, p.second);
			}
		}

		void TreeItem::doubleClicked()
		{
		}		

		void TreeItem::setContextMenuItems(std::list<std::pair<QString, std::function<void()>>>&& contextMenuItems)
		{
			mContextMenuItems = std::forward<std::list<std::pair<QString, std::function<void()>>>>(contextMenuItems);
		}

		int TreeItem::parentIndex()
		{
			int i;
			for (i = 0; i < parentItem()->childCount(); ++i) {
				if (parentItem()->child(i) == this)
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

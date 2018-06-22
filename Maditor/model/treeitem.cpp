#include "maditormodellib.h"

#include "treeitem.h"

#include "treemodel.h"

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
				for (const std::pair<const QString, std::function<void()>> &p : mContextMenuItems) {
					menu.addAction(p.first, p.second);
				}
			}
		}

		void TreeItem::doubleClicked()
		{
		}		

		void TreeItem::setContextMenuItems(std::map<QString, std::function<void()>>&& contextMenuItems)
		{
			mContextMenuItems = std::forward<std::map<QString, std::function<void()>>>(contextMenuItems);
		}

		void TreeItem::addContextMenuItem(const QString& name, std::function<void()> f)
		{
			mContextMenuItems[name] = f;
		}

		void TreeItem::removeContextMenuItem(const QString& name)
		{
			mContextMenuItems.erase(name);
		}

		const std::map<QString, std::function<void()>>& TreeItem::getContextMenuItems()
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
			return -1;
		}

		QVariant TreeItem::icon() const
		{
			return QVariant();
		}
		

	}
}

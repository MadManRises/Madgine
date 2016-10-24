

#include <QStringList>

#include "treeitem.h"
#include "TreeModel.h"

namespace Maditor {
	namespace Model {

		TreeModel::TreeModel(TreeItem * root, int columnCount) :
			mRoot(root),
			mColumnCount(columnCount)
		{

		}

		Q_INVOKABLE QModelIndex TreeModel::index(int row, int column, const QModelIndex & parent) const
		{
			if (!hasIndex(row, column, parent))
				return QModelIndex();

			TreeItem *item;
			if (!parent.isValid()) {
				item = mRoot;
			}
			else {
				item = static_cast<TreeItem*>(parent.internalPointer());
			}


			if (item->childCount() > row) {
				return createIndex(row, column, item->child(row));
			}
			else {
				return QModelIndex();
			}
		}

		Q_INVOKABLE QModelIndex TreeModel::parent(const QModelIndex & child) const
		{
			if (!child.isValid())
				return QModelIndex();


			TreeItem *item = static_cast<TreeItem*>(child.internalPointer());
			if (item->parentItem() == mRoot)
				return QModelIndex();

			return createIndex(item->parentItem()->parentIndex(), 0, item->parentItem());
		}

		Q_INVOKABLE int TreeModel::rowCount(const QModelIndex & parent) const
		{
			TreeItem *item;
			if (!parent.isValid())
				item = mRoot;
			else {
				item = static_cast<TreeItem*>(parent.internalPointer());
			}
			return item->childCount();
		}

		Q_INVOKABLE int TreeModel::columnCount(const QModelIndex & parent) const
		{
			return mColumnCount;
		}

		Q_INVOKABLE QVariant TreeModel::data(const QModelIndex & index, int role) const
		{

			if (!index.isValid())
				return QVariant();

			if (role != Qt::DisplayRole && role != Qt::DecorationRole)
				return QVariant();

			TreeItem *el = static_cast<TreeItem*>(index.internalPointer());
			switch (role) {
			case Qt::DisplayRole:
				return el->data(index.column());
			case Qt::DecorationRole:
				return el->icon();
			default:
				return QVariant();
			}
		}

		void TreeModel::extendContextMenu(QMenu & menu)
		{
			for (const std::pair<QString, std::function<void()>> &p : mContextMenuItems) {
				menu.addAction(p.first, p.second);
			}
		}

		

		void TreeModel::handleContextMenuRequest(const QModelIndex & p, QMenu & menu)
		{
			extendContextMenu(menu);
			if (p.isValid()) {
				TreeItem *item = static_cast<TreeItem*>(p.internalPointer());
				item->extendContextMenu(menu);
			}
		}

		void TreeModel::setModelContextMenuItems(std::list<std::pair<QString, std::function<void()>>>&& contextMenuItems)
		{
			mContextMenuItems = std::forward<std::list<std::pair<QString, std::function<void()>>>>(contextMenuItems);
		}


	}
}

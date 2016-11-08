#include "madgineinclude.h"

#include "treeitem.h"
#include "TreeModel.h"

namespace Maditor {
	namespace Model {

		TreeModel::TreeModel(TreeItem * root, int columnCount, bool sortByParentNodes) :
			mRoot(root),
			mColumnCount(columnCount),
			mSorter(sortByParentNodes)
		{
			mSorter.setDynamicSortFilter(true);
			mSorter.setSourceModel(this);	

			connect(this, &TreeModel::insertRowsQueued, this, &TreeModel::performRowsInsert);
			connect(this, &TreeModel::removeRowsQueued, this, &TreeModel::performRowsRemove);
			connect(&mSorter, &TreeSorter::doubleClicked, this, &TreeModel::itemDoubleClicked);
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

		TreeSorter * TreeModel::sorted()
		{
			return &mSorter;
		}

		void TreeModel::performRowsInsert(const QModelIndex & parent, int start, int end)
		{
			beginInsertRows(parent, start, end);
			endInsertRows();
			QModelIndex i = index(start, 0, parent);
			qDebug() << "Inserted" << (end - start + 1) << "items starting with " << data(i) << "at" << i;
			if (i.parent().isValid())
				qDebug() << "at Parent" << data(i.parent());
		}

		void TreeModel::performRowsRemove(const QModelIndex & parent, int start, int end)
		{
			beginRemoveRows(parent, start, end);
			endRemoveRows();
		}
	

		void TreeModel::handleContextMenuRequest(const QModelIndex & p, QMenu & menu)
		{
			if (p.isValid()) {
				TreeItem *item = static_cast<TreeItem*>(p.internalPointer());
				item->extendContextMenu(menu);
			}
		}

		void TreeModel::itemDoubleClicked(const QModelIndex &index) {
			if (index.isValid()) {
				TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
				item->doubleClicked();
			}
		}

	}
}

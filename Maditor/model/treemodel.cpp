#include "maditormodellib.h"

#include "treeitem.h"
#include "treemodel.h"

namespace Maditor {
	namespace Model {

		TreeModel::TreeModel(TreeItem * root, int columnCount, bool sortByParentNodes) :
			mRoot(root),
			mColumnCount(columnCount),
			mSorter(sortByParentNodes)
		{
			mSorter.setDynamicSortFilter(true);
			mSorter.setSourceModel(this);	

			connect(&mSorter, &TreeSorter::doubleClicked, this, &TreeModel::itemDoubleClicked);
		}

		Q_INVOKABLE QModelIndex TreeModel::index(int row, int column, const QModelIndex & parent) const
		{
			if (!hasIndex(row, column, parent))
				return QModelIndex();

			TreeItem *it;
			if (!parent.isValid()) {
				it = mRoot;
			}
			else {
				it = item(parent);
			}

			if (it->childCount() > row) {
				return createIndex(row, column, it->child(row));
			}
			else {
				return QModelIndex();
			}
		}

		Q_INVOKABLE QModelIndex TreeModel::parent(const QModelIndex & child) const
		{
			if (!child.isValid())
				return QModelIndex();


			TreeItem *parent = item(child)->parentItem();
			if (parent == mRoot)
				return QModelIndex();

			return createIndex(parent->parentIndex(), 0, parent);
		}

		Q_INVOKABLE int TreeModel::rowCount(const QModelIndex & parent) const
		{
			TreeItem *it;
			if (!parent.isValid())
				it = mRoot;
			else {
				it = item(parent);
			}
			return it->childCount();
		}

		Q_INVOKABLE int TreeModel::columnCount(const QModelIndex & parent) const
		{
			return mColumnCount;
		}

		Q_INVOKABLE QVariant TreeModel::data(const QModelIndex & index, int role) const
		{

			if (!index.isValid())
				return QVariant();

			if (role != Qt::DisplayRole && role != Qt::DecorationRole && role != Qt::EditRole)
				return QVariant();

			TreeItem *el = item(index);
			switch (role) {
			case Qt::DisplayRole:
			case Qt::EditRole:
				return el->cellData(index.column());
			case Qt::DecorationRole:
				return el->icon();
			default:
				return QVariant();
			}
		}

		QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
		{
			if (role != Qt::DisplayRole)
				return QVariant();

			if (orientation == Qt::Horizontal)
				return header(section);
			else
				return QVariant();
		}

		TreeSorter * TreeModel::sorted()
		{
			return &mSorter;
		}

		TreeItem * TreeModel::item(const QModelIndex & index) const
		{
			return static_cast<TreeItem*>(index.internalPointer());
		}	

		void TreeModel::handleContextMenuRequest(const QModelIndex & p, QMenu& menu)
		{
			if (p.isValid()) {
				item(p)->extendContextMenu(menu);
			}
			else {
				mRoot->extendContextMenu(menu);
			}
		}

		void TreeModel::itemDoubleClicked(const QModelIndex &index) {
			if (index.isValid()) {
				item(index)->doubleClicked();
			}
		}


		QVariant TreeModel::header(int section) const
		{
			return QVariant();
		}

	}
}

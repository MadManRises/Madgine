#include "maditormodellib.h"

#include "treeunit.h"

#include "Madgine/serialize/container/serializablecontainer.h"

namespace Maditor {
	namespace Model {

		TreeUnitBase::TreeUnitBase(int columnCount) :
			TreeModel(this, columnCount),
			TreeUnitItemBase(this),
			mResetting(false)
		{

		}

		TreeUnitItemBase::TreeUnitItemBase(TreeUnitBase * tree) :
			mParent(nullptr),
			mTree(tree)
		{
		}

		void TreeUnitItemBase::notifyDataChange(int column)
		{
			notifyDataChange(column, column);
		}

		void TreeUnitItemBase::notifyDataChange(int fromCol, int toCol)
		{
			mTree->onDataChanged(getIndex(), parentIndex(), fromCol, toCol);
		}

		void TreeUnitItemBase::handleOperation(int row, int op)
		{
			mTree->handleOperation(getIndex(), row, op);
		}

		TreeUnitItemBase::TreeUnitItemBase(TreeUnitItemBase * parent) :
			mParent(parent),
			mTree(parent->mTree)
		{
		}

		TreeUnitItemBase * TreeUnitItemBase::parentItem() const
		{
			return mParent;
		}

		QModelIndex TreeUnitItemBase::getIndex()
		{
			return mParent ? mTree->index(parentIndex(), 0, mParent->getIndex()) : QModelIndex();
		}

		QVariant TreeUnitBase::cellData(int col) const
		{
			return QVariant();
		}

		void TreeUnitBase::handleOperation(const QModelIndex & parent, int row, int op)
		{
			using namespace Engine::Serialize;

			switch (op) {
			case INSERT_ITEM:
				if (!mResetting) {
					beginInsertRows(parent, row, row);
					endInsertRows();
				}
				break;
			case BEFORE | RESET:
				beginResetModel();
				mResetting = true;
				break;
			case AFTER | RESET:
				endResetModel();
				mResetting = false;
				break;
			case BEFORE | REMOVE_ITEM:
				if (!mResetting) {
					beginRemoveRows(parent, row, row);
				}
				break;
			case AFTER | REMOVE_ITEM:
				if (!mResetting) {
					endRemoveRows();
				}
				break;
			default:
				throw 0;
			}
		}

		void TreeUnitBase::onDataChanged(const QModelIndex & parent, int row, int fromCol, int toCol)
		{
			emit dataChanged(index(row, fromCol, parent), index(row, toCol, parent));
		}

	}
}

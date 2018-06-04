#include "maditormodellib.h"

#include "tablemodel.h"

namespace Maditor {
	namespace Model {

		TableModel::TableModel(int columnCount) :
			mColumnCount(columnCount)
		{
		}

		Q_INVOKABLE int TableModel::columnCount(const QModelIndex & parent) const
		{
			return mColumnCount;
		}

		Q_INVOKABLE QVariant TableModel::data(const QModelIndex & index, int role) const
		{
			if (index.column() >= mColumnCount || index.row() >= rowCount())
				return QVariant();
			return Q_INVOKABLE data(index.row(), index.column(), role);
		}

	}
}

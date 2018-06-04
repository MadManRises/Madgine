#pragma once

namespace Maditor {
	namespace Model {


class TableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	TableModel(int columnCount);

	// Inherited via QAbstractItemModel

	virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const = 0;

	virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

	virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	virtual QVariant data(int row, int column, int role = Qt::DisplayRole) const = 0;

	using QAbstractTableModel::beginInsertRows;
	using QAbstractTableModel::endInsertRows;
	using QAbstractTableModel::beginRemoveRows;
	using QAbstractTableModel::endRemoveRows;


private:
	int mColumnCount;


};

	}
}


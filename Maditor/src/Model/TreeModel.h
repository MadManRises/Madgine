#pragma once


namespace Maditor {
	namespace Model {


class TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	TreeModel(TreeItem *root, int columnCount);

	void handleContextMenuRequest(const QModelIndex &p, QMenu &menu);

	// Inherited via QAbstractItemModel
	virtual Q_INVOKABLE QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

	virtual Q_INVOKABLE QModelIndex parent(const QModelIndex & child) const override;

	virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

	virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

	virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

	using QAbstractItemModel::beginInsertRows;
	using QAbstractItemModel::endInsertRows;
	using QAbstractItemModel::beginRemoveRows;
	using QAbstractItemModel::endRemoveRows;

protected:
	void setModelContextMenuItems(std::list<std::pair<QString, std::function<void()>>> &&contextMenuItems);

	void extendContextMenu(QMenu &menu);

private:
	TreeItem *mRoot;
	int mColumnCount;

	std::list<std::pair<QString, std::function<void()>>> mContextMenuItems;

};

	}
}


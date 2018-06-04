#pragma once

#include "treesorter.h"

namespace Maditor {
	namespace Model {


class MADITOR_MODEL_EXPORT TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	TreeModel(TreeItem *root, int columnCount, bool sortByParentNodes = false);

	void handleContextMenuRequest(const QModelIndex &p, QMenu &menu);

	// Inherited via QAbstractItemModel
	virtual Q_INVOKABLE QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

	virtual Q_INVOKABLE QModelIndex parent(const QModelIndex & child) const override;

	virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

	virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

	virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override final;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	TreeSorter *sorted();

	using QAbstractItemModel::beginInsertRows;
	using QAbstractItemModel::endInsertRows;
	using QAbstractItemModel::beginRemoveRows;
	using QAbstractItemModel::endRemoveRows;

protected:
	TreeItem *item(const QModelIndex &index) const;
	virtual QVariant header(int section) const;

public slots:
	void itemDoubleClicked(const QModelIndex &index);


private:
	TreeItem *mRoot;
	TreeSorter mSorter;
	int mColumnCount;

	std::list<std::pair<QString, std::function<void()>>> mContextMenuItems;

};

	}
}


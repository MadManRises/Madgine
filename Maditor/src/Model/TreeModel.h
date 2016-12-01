#pragma once

#include "TreeSorter.h"

namespace Maditor {
	namespace Model {


class TreeModel : public QAbstractItemModel
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

	virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;


	TreeSorter *sorted();

	using QAbstractItemModel::beginInsertRows;
	using QAbstractItemModel::endInsertRows;
	using QAbstractItemModel::beginRemoveRows;
	using QAbstractItemModel::endRemoveRows;

protected:
	TreeItem *item(const QModelIndex &index) const;

public slots:
	void itemDoubleClicked(const QModelIndex &index);

signals:
	//void insertRowsQueued(const QModelIndex &parent, int start, int end);
	//void removeRowsQueued(const QModelIndex &parent, int start, int end);

protected slots:
	//void performRowsInsert(const QModelIndex &parent, int start, int end);
	//virtual void performRowsRemove(const QModelIndex &parent, int start, int end);

private:
	TreeItem *mRoot;
	TreeSorter mSorter;
	int mColumnCount;

	std::list<std::pair<QString, std::function<void()>>> mContextMenuItems;

};

	}
}


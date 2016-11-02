#pragma once


namespace Maditor {
	namespace Model {


class TreeSorter : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	TreeSorter(bool sortByParentNodes = false);

public slots:
	void itemDoubleClicked(const QModelIndex &index);

signals:
	void doubleClicked(const QModelIndex &index);

protected:
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
	bool mSortByParentNodes;

};

	}
}


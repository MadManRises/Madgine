#pragma once


namespace Maditor {
	namespace Model {


class MADITOR_MODEL_EXPORT LogSorterFilter : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	LogSorterFilter();

public slots:
	void itemDoubleClicked(const QModelIndex &index);

signals:
	void doubleClicked(const QModelIndex &index);

protected:
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};

	}
}


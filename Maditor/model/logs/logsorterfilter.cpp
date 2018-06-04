#include "maditormodellib.h"

#include "logsorterfilter.h"

namespace Maditor {
	namespace Model {
		LogSorterFilter::LogSorterFilter() {
		}

		void LogSorterFilter::itemDoubleClicked(const QModelIndex & index)
		{
			emit doubleClicked(mapToSource(index));
		}
		bool LogSorterFilter::lessThan(const QModelIndex & left, const QModelIndex & right) const
		{
			QVariant leftData = sourceModel()->data(left);
			QVariant rightData = sourceModel()->data(right);

			if (leftData.type() == QVariant::Double) {
				if (leftData.toDouble() != rightData.toDouble())
					return leftData.toDouble() < rightData.toDouble();
			}
			else {
				return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
			}

			return left.row() < right.row();
		}

	}
}

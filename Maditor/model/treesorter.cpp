#include "maditormodellib.h"

#include "treesorter.h"

namespace Maditor {
	namespace Model {

		TreeSorter::TreeSorter(bool sortByParentNodes) :
			mSortByParentNodes(sortByParentNodes)
		{

		}

		bool TreeSorter::lessThan(const QModelIndex &left, const QModelIndex &right) const {



			if (mSortByParentNodes) {
				bool leftHasChildren = sourceModel()->rowCount(left) > 0;
				bool rightHasChildren = sourceModel()->rowCount(right) > 0;
				if (leftHasChildren != rightHasChildren)
					return leftHasChildren;
			}

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

		void TreeSorter::itemDoubleClicked(const QModelIndex &index) {
			emit doubleClicked(mapToSource(index));
		}

	}
}

#include "maditormodellib.h"

#include "tableunit.h"

#include "Madgine/serialize/container/serializablecontainer.h"

namespace Maditor {
	namespace Model {

		TableUnitBase::TableUnitBase(int columnCount) :
			TableModel(columnCount),
			mResetting(false)
		{

		}

		void TableUnitBase::handleOperation(int row, int op)
		{
			using namespace Engine::Serialize;

			switch (op) {
			case BEFORE | RESET:
				beginResetModel();
				mResetting = true;
				break;
			case AFTER | RESET:
				endResetModel();
				mResetting = false;
				break;
			case INSERT_ITEM:
				if (!mResetting) {
					beginInsertRows(QModelIndex(), row, row);
					endInsertRows();
				}
				break;
			case BEFORE | REMOVE_ITEM:
				if (!mResetting) {
					beginRemoveRows(QModelIndex(), row, row);
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

	}
}

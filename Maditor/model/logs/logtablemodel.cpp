#include "maditormodellib.h"

#include "logtablemodel.h"

//#include "Model\Editors\EditorManager.h"

namespace Maditor {
	namespace Model {

			LogTableModel::LogTableModel()
			{				
				mSorted.setDynamicSortFilter(true);
				mSorted.setSourceModel(this);

				connect(&mSorted, &LogSorterFilter::doubleClicked, this, &LogTableModel::doubleClicked);
			}

			void LogTableModel::addMessage(const QString &msg, Engine::Util::MessageType level, const QString &logName, const QString &traceback, const std::string &fileName, int lineNr) {
				beginInsertRows(QModelIndex(), mItems.size(), mItems.size());
				mItems.emplace_front(level, msg, logName, traceback, fileName, lineNr);
				endInsertRows();
			}

			void LogTableModel::doubleClicked(const QModelIndex & index)
			{
				auto it = mItems.begin();
				std::advance(it, index.row());

				const std::string &fileName = std::get<4>(*it);
			
				if (fileName == "<unknown>")
					return;

				//Editors::EditorManager::getSingleton().openByExtension(traceback.mFile, traceback.mLineNr);
			}

			void LogTableModel::clear()
			{
				beginResetModel();
				mItems.clear();
				endResetModel();
			}

			LogSorterFilter * LogTableModel::sorted()
			{
				return &mSorted;
			}

			QVariant LogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
			{
				if (orientation != Qt::Horizontal)
					return QVariant();

				if (role != Qt::DisplayRole)
					return QVariant();

				switch (section) {
				case 0:
					return QVariant();
				case 1:
					return "Message";
				case 2:
					return "Source";
				case 3:
					return "Traceback";
				}
				return QVariant();
			}

			Q_INVOKABLE int LogTableModel::rowCount(const QModelIndex & parent) const
			{
				return mItems.size();
			}

			Q_INVOKABLE int LogTableModel::columnCount(const QModelIndex & parent) const
			{
				return 4;
			}

			Q_INVOKABLE QVariant LogTableModel::data(const QModelIndex & index, int role) const
			{
				if (!index.isValid())
					return QVariant();
				
				if (index.row() >= mItems.size())
					return QVariant();

				if (index.column() >= 4)
					return QVariant();



				auto it = mItems.begin();
				std::advance(it, index.row());

				if (index.column() == 0) {
					if (role != Qt::DecorationRole)
						return QVariant();
					QIcon icon;
					switch (std::get<0>(*it)) {
					case Engine::Util::LOG_TYPE:
						icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxInformation));
						break;
					case Engine::Util::WARNING_TYPE:
						icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxWarning));
						break;
					case Engine::Util::ERROR_TYPE:
						icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxCritical));
						break;
					}
					return icon;
				}
				else if (index.column() == 1) {
					if (role != Qt::DisplayRole)
						return QVariant();
					return QVariant(std::get<1>(*it));
				}
				else if (index.column() == 3) {
					if (role != Qt::DisplayRole)
						return QVariant();
					return QVariant(std::get<3>(*it));
				}
				else if (index.column() == 2) {
					if (role != Qt::DisplayRole)
						return QVariant();
					return QVariant(std::get<2>(*it));
				}

				return QVariant();
			}


	}
}

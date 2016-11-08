#include "madgineinclude.h"

#include "LogTableModel.h"

#include "Model\Editors\EditorManager.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			LogTableModel::LogTableModel()
			{				
			}

			void LogTableModel::addMessage(const QString &msg, Ogre::LogMessageLevel level, const QList<Engine::Util::TraceBack> &traceback) {
				QString tracebackString;
				for (const Engine::Util::TraceBack &t : traceback) {
					if (!tracebackString.isEmpty()) tracebackString += "\n";
					tracebackString += QString("%1(%2): %3").arg(QString::fromStdString(t.mFile), QString::number(t.mLineNr), QString::fromStdString(t.mFunction));
				}
				beginInsertRows(QModelIndex(), 0, 0);
				mItems.emplace_front(level, msg, tracebackString, traceback.empty() ? Engine::Util::TraceBack() : traceback.back());
				endInsertRows();
			}

			void LogTableModel::doubleClicked(const QModelIndex & index)
			{
				auto it = mItems.begin();
				std::advance(it, index.row());

				Engine::Util::TraceBack &traceback = std::get<3>(*it);

				if (traceback.mFile == "<unknown>")
					return;

				Editors::EditorManager::getSingleton().openByExtension(traceback.mFile, traceback.mLineNr);
			}

			void LogTableModel::clear()
			{
				beginResetModel();
				mItems.clear();
				endResetModel();
			}

			Q_INVOKABLE int LogTableModel::rowCount(const QModelIndex & parent) const
			{
				return mItems.size();
			}

			Q_INVOKABLE int LogTableModel::columnCount(const QModelIndex & parent) const
			{
				return 3;
			}

			Q_INVOKABLE QVariant LogTableModel::data(const QModelIndex & index, int role) const
			{
				if (!index.isValid())
					return QVariant();
				
				if (index.row() >= mItems.size())
					return QVariant();

				if (index.column() >= 3)
					return QVariant();



				auto it = mItems.begin();
				std::advance(it, index.row());

				if (index.column() == 0) {
					if (role != Qt::DecorationRole)
						return QVariant();
					QIcon icon;
					switch (std::get<0>(*it)) {
					case Ogre::LML_TRIVIAL:
						icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxInformation));
						break;
					case Ogre::LML_NORMAL:
						icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxWarning));
						break;
					case Ogre::LML_CRITICAL:
						icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxCritical));
						break;
					}
					return icon;
				}
				else if (index.column() == 2) {
					if (role != Qt::DisplayRole)
						return QVariant();
					return QVariant(std::get<1>(*it));
				}
				else if (index.column() == 1) {
					if (role != Qt::DisplayRole)
						return QVariant();
					return QVariant(std::get<2>(*it));
				}

				return QVariant();
			}

		}
	}
}
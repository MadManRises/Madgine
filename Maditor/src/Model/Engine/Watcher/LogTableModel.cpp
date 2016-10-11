#include "LogWatcher.h"
#include "LogTableModel.h"
#include <QApplication>
#include <QStyle>
#include <qdebug.h>
#include <qfileinfo.h>


namespace Maditor {
	namespace Model {
		namespace Watcher {

			LogTableModel::LogTableModel(const QString &root) :
			mSourcesRoot(root + "src/"){
				qDebug() << mSourcesRoot.path();
				mErrorIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxCritical));
				mMsgIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxInformation));
			}

			void LogTableModel::addMessage(const QString &msg, Ogre::LogMessageLevel level, const QList<Engine::Util::UtilMethods::TraceBack> &traceback) {
				beginInsertRows(QModelIndex(), mItems.size(), mItems.size());
				QString tracebackString;

				for (const Engine::Util::UtilMethods::TraceBack &t : traceback) {
					if (!tracebackString.isEmpty()) tracebackString += "\n";
					tracebackString += QString("%1(%2): %3").arg(mSourcesRoot.relativeFilePath(QString::fromStdString(t.mFile)), QString::number(t.mLineNr), QString::fromStdString(t.mFunction));
				}
				mItems.emplace_back(level, msg, tracebackString, traceback.empty() ? Engine::Util::UtilMethods::TraceBack() : traceback.back());
				endInsertRows();
			}

			void LogTableModel::doubleClicked(const QModelIndex & index)
			{
				auto it = mItems.begin();
				std::advance(it, index.row());

				Engine::Util::UtilMethods::TraceBack &traceback = std::get<3>(*it);

				if (traceback.mFile == "<unknown>")
					return;

				QFileInfo file(traceback.mFile.c_str());
				if (file.suffix() == "script") {
					for (Ogre::ResourceGroupManager::ResourceLocation *loc : Ogre::ResourceGroupManager::getSingleton().getResourceLocationList("Scripting")) {
						const Ogre::StringVectorPtr& list = loc->archive->find(traceback.mFile);
						if (!list->empty()) {
							emit openScriptFile((loc->archive->getName() + "/" + traceback.mFile).c_str(), traceback.mLineNr);
							return;
						}
					}
					qDebug() << "Can't find Resource-File: " << traceback.mFile.c_str();
				}
				else if (QStringList({ "cpp", "h" }).contains(file.suffix())) {

					qDebug() << file.filePath();
				}
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
					if (std::get<0>(*it) == Ogre::LML_NORMAL)
						return mMsgIcon;
					else
						return mErrorIcon;
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
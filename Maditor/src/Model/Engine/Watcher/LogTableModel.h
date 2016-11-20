#pragma once

#include "OgreLog.h"
#include "Util\Traceback.h"
#include "Common\LogInfo.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			class LogTableModel : public QAbstractTableModel {
				Q_OBJECT

			public:
				LogTableModel();

			public slots:
				void addMessage(const QString &msg, MessageType level, const Traceback &traceback, const QString &fullTraceback);
				void doubleClicked(const QModelIndex &index);
				void clear();

			public:

				// Inherited via QAbstractTableModel
				virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;


			private:
				std::list<std::tuple<MessageType, QString, QString, Traceback>> mItems;
				
			};

		}
	}
}
#pragma once

#include "Madgine/util/utilmethods.h"
#include "logsorterfilter.h"

namespace Maditor {
	namespace Model {

		

			class MADITOR_MODEL_EXPORT LogTableModel : public QAbstractTableModel {
				Q_OBJECT

			public:
				LogTableModel();

			public slots:
				void addMessage(const QString &msg, Engine::Util::MessageType level, const QString &logName, const QString &traceback, const std::string &fileName, int lineNr);
				void doubleClicked(const QModelIndex &index);
				void clear();

				LogSorterFilter *sorted();

			public:

				virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

				// Inherited via QAbstractTableModel
				virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;


			private:
				std::list<std::tuple<Engine::Util::MessageType, QString, QString, QString, std::string, int>> mItems;
				
				LogSorterFilter mSorted;
			};

	}
}

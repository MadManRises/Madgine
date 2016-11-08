#pragma once


namespace Maditor {
	namespace Model {
		namespace Watcher {

			class LogTableModel : public QAbstractTableModel {
				Q_OBJECT

			public:
				LogTableModel();

			public slots:
				void addMessage(const QString &msg, Ogre::LogMessageLevel level, const QList<Engine::Util::TraceBack>& traceback);
				void doubleClicked(const QModelIndex &index);
				void clear();

			public:

				// Inherited via QAbstractTableModel
				virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;


			private:
				std::list<std::tuple<Ogre::LogMessageLevel, QString, QString, Engine::Util::TraceBack>> mItems;
				
			};

		}
	}
}
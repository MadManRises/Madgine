#pragma once

#include "Model\TreeModel.h"
#include "OgreNodeItem.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {			


			class ObjectsWatcher : public QAbstractTableModel{
				Q_OBJECT

			
			public:
				ObjectsWatcher();


				virtual void timerEvent(QTimerEvent *e) override;

				// Inherited via QAbstractTableModel
				virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

			private:


				struct Object {
					std::string mName;
					Engine::Util::ObjectState mState;
					void *mPtr;
				};
				std::list<Object> mObjects;
			};

		}
	}
}
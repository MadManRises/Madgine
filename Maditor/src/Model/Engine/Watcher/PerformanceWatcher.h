#pragma once

#include <qabstractitemmodel.h>

#include <qtimer.h>

namespace Maditor {
	namespace Model {
		namespace Watcher {			

			class ProfilerNode;

			class PerformanceWatcher : public QAbstractItemModel {
				Q_OBJECT

			public:
				PerformanceWatcher();
				
				void update();

			signals:
				void dataChangedQueued();

			private slots:
				void updateData();

			protected:


				// Inherited via QAbstractItemModel
				virtual Q_INVOKABLE QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QModelIndex parent(const QModelIndex & child) const override;

				virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

				virtual void timerEvent(QTimerEvent *event) override;

				virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			private:
				bool mUpdatePending;

				ProfilerNode *mRootItem;
			};

		}
	}
}
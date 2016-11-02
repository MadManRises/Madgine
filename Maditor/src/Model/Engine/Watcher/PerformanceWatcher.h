#pragma once

#include "Model\TreeModel.h"

#include "ProfilerNode.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {			

			class PerformanceWatcher : public TreeModel {
				Q_OBJECT

			public:
				PerformanceWatcher();
				
				void update();
				void clear();

			signals:
				void dataChangedQueued();
				void resetModelQueued();

			private slots:
				void updateData();
				void resetModel();

			protected:

				virtual void timerEvent(QTimerEvent *event) override;

				virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			private:
				bool mUpdatePending;

				RootProfilerNode mRootItem;
			};

		}
	}
}
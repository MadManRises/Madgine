#pragma once

#include "Model\TreeModel.h"
#include "OgreNodeItem.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {			


			class OgreSceneWatcher : public TreeModel{
				Q_OBJECT

			public:
				OgreSceneWatcher();
				
				void update();
				void clear();

			protected:

				virtual void timerEvent(QTimerEvent *event) override;

				virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

				virtual void performRowsRemove(const QModelIndex &parent, int from, int to) override;

			signals:
				void modelChanged();

			private slots:
				void onModelChanged();

			private:
				bool mUpdatePending;

				OgreMirroredRootSceneNodeItem mMirroredRootItem;
				OgreNodeItem mRootItem;				

				std::mutex mMutex;
			};

		}
	}
}
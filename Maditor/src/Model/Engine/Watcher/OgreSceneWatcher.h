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

			private:
				bool mUpdatePending;

				OgreRootSceneNodeItem mRootItem;
			};

		}
	}
}
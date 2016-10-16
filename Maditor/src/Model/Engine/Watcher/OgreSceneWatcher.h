#pragma once

#include <qabstractitemmodel.h>

namespace Ogre {
	class SceneNode;
}

namespace Maditor {
	namespace Model {
		namespace Watcher {			


			class OgreSceneWatcher : public QAbstractItemModel {
				Q_OBJECT

			public:
				OgreSceneWatcher();
				
				void update();

				void setRoot(Ogre::SceneNode *node);

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

				Ogre::SceneNode *mRootSceneNode;
			};

		}
	}
}
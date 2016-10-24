#pragma once

#include "Model\TreeItem.h"

namespace Ogre {
	class SceneNode;
	class MovableObject;
}

namespace Maditor {
	namespace Model {
		namespace Watcher {			

			class OgreSceneWatcher;
			class OgreSceneNodeItem;

			class OgreNodeItem : public QObject, public TreeItem {
				Q_OBJECT

			public:
				OgreNodeItem(OgreSceneNodeItem *parent, const std::string &name);

				virtual QVariant data(int col) const override;

				virtual OgreNodeItem *parentItem() override;

			private:
				OgreSceneNodeItem *mParent;
				std::string mName;
			};

			class OgreEntityItem : public OgreNodeItem {
			public:
				OgreEntityItem(OgreSceneNodeItem *parent, Ogre::MovableObject *o);

				Ogre::MovableObject *object();

				// Inherited via OgreNodeItem
				virtual int childCount() override;
				virtual TreeItem * child(int i) override;

			private:
				Ogre::MovableObject *mObject;


			};


			class OgreSceneNodeItem : public OgreNodeItem {
			public:
				OgreSceneNodeItem(OgreSceneNodeItem *parent, Ogre::SceneNode *node);
				virtual ~OgreSceneNodeItem();

				void update(OgreSceneWatcher *watcher, const QModelIndex &index);

				virtual Ogre::SceneNode *getNode();

				// Inherited via OgreNodeItem
				virtual int childCount() override;
				virtual TreeItem * child(int i) override;

			protected:
				OgreSceneNodeItem();



			private:
				std::list<OgreEntityItem*> mObjects;
				std::list<OgreSceneNodeItem*> mNodes;

				Ogre::SceneNode *mNode;


			};

			class OgreRootSceneNodeItem : public OgreSceneNodeItem {
			public:
				virtual Ogre::SceneNode *getNode() override;
			};



		}
	}
}
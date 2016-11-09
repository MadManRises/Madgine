#pragma once

#include "Model\TreeItem.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {			



			class OgreMirroredNodeItem {

			public:
				OgreMirroredNodeItem(const std::string &name);

				QString name() const;

				virtual std::set<OgreMirroredNodeItem*> getChildren();

			private:
				std::string mName;
			};


			class OgreMirroredSceneNodeItem : public OgreMirroredNodeItem {
			public:
				OgreMirroredSceneNodeItem(Ogre::SceneNode *node);

				virtual Ogre::SceneNode *getNode();

				void update();

				virtual std::set<OgreMirroredNodeItem*> getChildren() override;

			protected:
				OgreMirroredSceneNodeItem();

			private:
				std::list<OgreMirroredNodeItem> mObjects;
				std::list<OgreMirroredSceneNodeItem> mNodes;

				Ogre::SceneNode *mNode;


			};

			class OgreMirroredRootSceneNodeItem : public OgreMirroredSceneNodeItem {
			public:
				virtual Ogre::SceneNode *getNode() override;
			};

			class OgreNodeItem : public TreeItem {
			public:
				OgreNodeItem(OgreMirroredNodeItem *mirror, OgreNodeItem *parent = 0);

				void update(OgreSceneWatcher *watcher, const QModelIndex &index);

				void clear();


				// Inherited via TreeItem
				virtual int childCount() override;
				virtual TreeItem * child(int i) override;
				virtual TreeItem * parentItem() override;
				virtual QVariant data(int col) const override;

			protected:
				OgreMirroredNodeItem *getMirror();

			private:
				std::list<OgreNodeItem> mChildren;
				OgreMirroredNodeItem *mMirror;
				OgreNodeItem *mParent;
				QString mName;

			};

		}
	}
}
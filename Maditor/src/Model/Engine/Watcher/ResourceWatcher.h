#pragma once

#include <qplaintextedit.h>
#include "OgreResourceGroupManager.h"
#include "LogTableModel.h"
#include <memory>
#include "Model\TreeItem.h"
#include "ResourceGroupItem.h"
#include "ResourceItem.h"

namespace Maditor {
	namespace Model {
		namespace Watcher {

			class ResourceGroupItem;

			class ResourceWatcher : public QAbstractItemModel, public Ogre::ResourceGroupListener {
				Q_OBJECT
				
			public:
				ResourceWatcher();
				~ResourceWatcher();

				void setup();
				void init();

			signals:
				void openScriptFile(const QString &path, int line);

			public slots:
				void itemDoubleClicked(const QModelIndex &index);

			protected:
				void open(const Ogre::ResourcePtr &res);
							

				// Inherited via ResourceGroupListener
				virtual void resourceGroupScriptingStarted(const Ogre::String & groupName, size_t scriptCount) override;

				virtual void scriptParseStarted(const Ogre::String & scriptName, bool & skipThisScript) override;

				virtual void scriptParseEnded(const Ogre::String & scriptName, bool skipped) override;

				virtual void resourceGroupScriptingEnded(const Ogre::String & groupName) override;

				virtual void resourceGroupLoadStarted(const Ogre::String & groupName, size_t resourceCount) override;

				virtual void resourceLoadStarted(const Ogre::ResourcePtr & resource) override;

				virtual void resourceLoadEnded(void) override;

				virtual void worldGeometryStageStarted(const Ogre::String & description) override;

				virtual void worldGeometryStageEnded(void) override;

				virtual void resourceGroupLoadEnded(const Ogre::String & groupName) override;


				// Inherited via QAbstractItemModel
				virtual Q_INVOKABLE QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QModelIndex parent(const QModelIndex & child) const override;

				virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;

				virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

				virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			private:
				TreeItem *mRootItem;

				std::map<std::string, ResourceGroupItem*> mGroups;
				std::map<std::string, ResourceItem*> mResources;
				
			};

		}
	}
}
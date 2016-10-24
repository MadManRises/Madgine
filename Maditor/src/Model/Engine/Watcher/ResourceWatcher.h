#pragma once


#include "Model\TreeItem.h"
#include "ResourceGroupItem.h"
#include "ResourceItem.h"
#include "Model\TreeModel.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {

			class ResourceGroupItem;

			class ResourceWatcher : public TreeModel, public TreeItem {
				Q_OBJECT
				
			public:
				ResourceWatcher();
				~ResourceWatcher();

				void init();


				// Inherited via TreeItem
				virtual int childCount() override;

				virtual TreeItem * child(int i) override;

				virtual TreeItem * parentItem() override;

				virtual QVariant data(int col) const override;

			signals:
				void openScriptFile(const QString &path, int line);

			public slots:
				void itemDoubleClicked(const QModelIndex &index);

			protected:
				void open(const Ogre::ResourcePtr &res);
							



				virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			private:

				std::unordered_map<std::string, ResourceGroupItem*> mGroups;
				

				

			};

		}
	}
}
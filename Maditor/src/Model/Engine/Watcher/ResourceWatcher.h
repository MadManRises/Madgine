#pragma once


#include "Model\TreeItem.h"
#include "Model\TreeModel.h"


namespace Maditor {
	namespace Model {
		namespace Watcher {

			class ResourceWatcher : public TreeModel, public TreeItem {
				Q_OBJECT
				
			public:
				ResourceWatcher();
				~ResourceWatcher();

				void init();
				void clear();

				// Inherited via TreeItem
				virtual int childCount() override;

				virtual TreeItem * child(int i) override;

				virtual TreeItem * parentItem() override;

				virtual QVariant data(int col) const override;

			signals:
				void resetModelQueued();

			public slots:
				void reloadScriptFile(const QString &fileName, const QString &group);

			private slots:				
				void resetModel();


			protected:
				virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			private:
				std::unordered_map<std::string, ResourceGroupItem> mGroups;

				bool mRunning;

			};

		}
	}
}
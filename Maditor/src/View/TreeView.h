#pragma once


namespace Maditor {
	namespace View {

			class TreeView : public QTreeView {

			public:
				TreeView(QWidget *parent = nullptr);
				~TreeView();

				void setModel(Model::TreeModel *model);


			private:

				std::list<QMetaObject::Connection> mConnections;
			};

	}
}
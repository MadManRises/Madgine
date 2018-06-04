#pragma once


namespace Maditor {
	namespace View {

			class TreeView : public QTreeView {

			public:
				TreeView(QWidget *parent = nullptr);
				~TreeView();

				void setModel(Model::TreeModel *model);
				void clearModel();

			private slots:
				void buildContextMenu(const QPoint &p);

			private:

				Model::TreeModel *mModel;

			};

	}
}
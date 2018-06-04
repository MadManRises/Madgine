#pragma once


namespace Maditor {
	namespace View {

			class FilesView : public QTreeView {

			public:
				FilesView(QWidget *parent = nullptr);
				~FilesView();

				void setModel(QFileSystemModel *model);
				void clearModel();

			private slots:
				//void buildContextMenu(const QPoint &p);

			private:

			};

	}
}
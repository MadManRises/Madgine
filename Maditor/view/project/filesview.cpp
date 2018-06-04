#include "maditorviewlib.h"

#include "filesview.h"


namespace Maditor {
	namespace View {


		FilesView::FilesView(QWidget * parent) :
			QTreeView(parent)
		{
			//connect(this, &QTreeView::customContextMenuRequested, this, &TreeView::buildContextMenu);
			//setContextMenuPolicy(Qt::CustomContextMenu);

			

			if(header()->isSortIndicatorShown())
				header()->setSortIndicator(0, Qt::AscendingOrder);
			

		}

		FilesView::~FilesView()
		{
		}

		void FilesView::setModel(QFileSystemModel * model)
		{
			QTreeView::setModel(model);
			setRootIndex(model->index(model->rootPath()));

			header()->setSectionResizeMode(0, QHeaderView::Stretch);
			header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

			hideColumn(3);
			hideColumn(2);

		}

		/*void TreeView::buildContextMenu(const QPoint &p) {
			if (!mModel) return;
			QMenu menu;
			mModel->handleContextMenuRequest(mModel->sorted()->mapToSource(indexAt(p)), menu);
			if (menu.actions().size() > 0)
				menu.exec(viewport()->mapToGlobal(p));
		}*/

		void FilesView::clearModel()
		{
			setModel(nullptr);
		}		 

	}
}

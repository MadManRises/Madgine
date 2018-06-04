#include "maditorviewlib.h"

#include "treeview.h"

#include "model/treemodel.h"

namespace Maditor {
	namespace View {


		TreeView::TreeView(QWidget * parent) :
			QTreeView(parent),
			mModel(nullptr)
		{
			connect(this, &QTreeView::customContextMenuRequested, this, &TreeView::buildContextMenu);
			setContextMenuPolicy(Qt::CustomContextMenu);

			if(header()->isSortIndicatorShown())
				header()->setSortIndicator(0, Qt::AscendingOrder);
		}

		TreeView::~TreeView()
		{
		}

		void TreeView::setModel(Model::TreeModel * model)
		{
			if (model) {
				QTreeView::setModel(model->sorted());
				connect(this, &QTreeView::doubleClicked, model->sorted(), &Model::TreeSorter::itemDoubleClicked);
			}
			else {
				QTreeView::setModel(nullptr);
			}

			mModel = model;
		}

		void TreeView::buildContextMenu(const QPoint &p) {
			if (!mModel) return;
			QMenu menu;
			mModel->handleContextMenuRequest(mModel->sorted()->mapToSource(indexAt(p)), menu);
			if (menu.actions().size() > 0)
				menu.exec(viewport()->mapToGlobal(p));
		}

		void TreeView::clearModel()
		{
			setModel(nullptr);
		}		 

	}
}

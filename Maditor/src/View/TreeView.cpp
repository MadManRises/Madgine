#include "maditorlib.h"

#include "TreeView.h"

#include "Model\TreeModel.h"

namespace Maditor {
	namespace View {


		TreeView::TreeView(QWidget * parent) :
			QTreeView(parent)
		{
		}

		TreeView::~TreeView()
		{
			for (QMetaObject::Connection &conn : mConnections)
				QObject::disconnect(conn);
		}

		void TreeView::setModel(Model::TreeModel * model)
		{
			mConnections.clear();

			QTreeView::setModel(model->sorted());
			setContextMenuPolicy(Qt::CustomContextMenu);
			mConnections.emplace_back(connect(this, &QTreeView::customContextMenuRequested, [=](const QPoint &p) {
				QMenu menu;
				model->handleContextMenuRequest(model->sorted()->mapToSource(indexAt(p)), menu);
				if (menu.actions().size() > 0)
					menu.exec(viewport()->mapToGlobal(p));
			}));

			mConnections.emplace_back(connect(this, &QTreeView::doubleClicked, model->sorted(), &Model::TreeSorter::itemDoubleClicked));

		}
	}
}
#include "maditorinclude.h"

#include "ProjectView.h"

#include "Model\Project\Project.h"

namespace Maditor {
	namespace View {
		namespace Project {


			ProjectView::ProjectView(QWidget * parent) :
				QTreeView(parent)
			{
			}

			ProjectView::~ProjectView()
			{
				for (QMetaObject::Connection &conn : mConnections)
					QObject::disconnect(conn);
			}

			void ProjectView::setProject(Model::Project * model)
			{
				setModel(model);
				setContextMenuPolicy(Qt::CustomContextMenu);
				mConnections.emplace_back(connect(this, &QTreeView::customContextMenuRequested, [=](const QPoint &p) {
					QMenu menu;
					model->handleContextMenuRequest(indexAt(p), menu);
					if (menu.actions().size() > 0)
						menu.exec(viewport()->mapToGlobal(p));
				}));
			}

		}
	}
}
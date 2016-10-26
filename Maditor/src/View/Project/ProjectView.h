#pragma once


namespace Maditor {
	namespace View {
		namespace Project {

			class ProjectView : public QTreeView {

			public:
				ProjectView(QWidget *parent = nullptr);
				~ProjectView();

				void setProject(Model::Project *model);


			private:

				std::list<QMetaObject::Connection> mConnections;
			};

		}
	}
}
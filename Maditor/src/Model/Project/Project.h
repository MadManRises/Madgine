#pragma once



#include "ProjectElement.h"

#include "Model\TreeModel.h"
#include "ModuleList.h"

namespace Maditor {
	namespace Model {
		class Project : public TreeModel, public ProjectElement {
			Q_OBJECT

		public:
			Project(const QString &path, const QString &name, QDomDocument doc = QDomDocument());
			Project(QDomDocument doc, const QString &path);
			~Project();

			const QString &root();

			static Project *load(const QString &path);

			bool isValid();

			virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
					
			void save();

			ModuleList *moduleList();

		private:
			void init();

			void copyTemplate(QMessageBox::StandardButton *answer);

			

		
		private:
			QDomDocument mDocument;
			
			QString mPath, mRoot;

			bool mValid;

			static const QString sProjectFileName;

			ModuleList mModules;

			// Inherited via ProjectElement
			virtual int childCount() override;

			virtual ModuleList* child(int i) override;

		};
	}
}
#pragma once

#include "Generator\CmakeProject.h"
#include "Module.h"
#include "ProjectElement.h"

#include "Model\TreeModel.h"

namespace Maditor {
	namespace Model {
		class Project : public TreeModel, public ProjectElement {
			Q_OBJECT

		public:
			Project(const QString &path, const QString &name, QDomDocument doc = QDomDocument());
			Project(QDomDocument doc, const QString &path);
			~Project();

			const QString &root();
			QString srcRoot();

			void generate();

			Generator::CmakeProject *cmake();

			static Project *load(const QString &path);

			bool hasModule(const QString &name);

			bool isValid();

			const std::list<std::unique_ptr<Module>> &modules();
			Module *getModuleByName(const QString &name);
			std::list<Module *> initDependencies();


			virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			

		protected:
			void save();

		private:
			void init();

			void copyTemplate(QMessageBox::StandardButton *answer);

			void addModule(Module *module);

		public slots:
			void createModule(const QString &name);
			void drawDependenciesGraph();

		signals:
			void newModuleRequest();
			void moduleAdded(Module *);

		private:
			QDomDocument mDocument;
			
			QString mPath, mRoot;

			Generator::CmakeProject mCmake;

			std::list<std::unique_ptr<Module>> mModules;

			QIcon mFileIcon;

			bool mValid;

			static const QString sProjectFileName;



			// Inherited via ProjectElement
			virtual int childCount() override;

			virtual Module * child(int i) override;

		};
	}
}
#pragma once


#include "ProjectElement.h"

#include "Generator\CmakeProject.h"

#include "Module.h"

namespace Maditor {
	namespace Model {
		class MADITOR_EXPORT ModuleList : public QObject, public ProjectElement {
			Q_OBJECT

		public:

			ModuleList(const QString &projectRoot, Project *parent, const QString &name);
			ModuleList(QDomElement element, const QString &projectRoot, Project *parent);
			~ModuleList();

			const QString &root();

			void generate();

			Generator::CmakeProject *cmake();

			bool hasModule(const QString &name);

			Module *getModuleByName(const QString &name);
			std::list<Module *> initDependencies();

			std::list<std::unique_ptr<Module>>::const_iterator begin();
			std::list<std::unique_ptr<Module>>::const_iterator end();

			virtual QVariant icon() const override;

			virtual Project *project() override;

		private:
			void init();

			void addModule(Module *module);

		public slots:
			void createModule(const QString &name);
			void drawDependenciesGraph();

		signals:
			void newModuleRequest();
			void moduleAdded(Module *);


		private:
			Project *mParent;

			QString mRoot;

			Generator::CmakeProject mCmake;

			std::list<std::unique_ptr<Module>> mModules;

			// Inherited via ProjectElement
			virtual int childCount() override;

			virtual Module * child(int i) override;
		};
	}
}
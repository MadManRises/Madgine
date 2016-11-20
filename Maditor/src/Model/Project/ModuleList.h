#pragma once


#include "ProjectElement.h"

#include "Generators\CmakeProject.h"

#include "Module.h"

namespace Maditor {
	namespace Model {
		class MADITOR_EXPORT ModuleList : public QObject, public ProjectElement {
			Q_OBJECT

		public:

			ModuleList(Project *parent);
			ModuleList(QDomElement element, Project *parent);
			~ModuleList();

			virtual QString path() const override;

			void generate();
			void release();

			Generators::CmakeProject *cmake();

			bool hasModule(const QString &name);

			Module *getModuleByName(const QString &name);
			std::list<Module *> initDependencies();

			std::list<std::unique_ptr<Module>>::const_iterator begin();
			std::list<std::unique_ptr<Module>>::const_iterator end();

			virtual QVariant icon() const override;

			virtual Project *project() override;

			// Inherited via ProjectElement
			virtual int childCount() override;

			virtual Module * child(int i) override;

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

			QString mPath;

			Generators::CmakeProject mCmake;

			std::list<std::unique_ptr<Module>> mModules;

			
		};
	}
}
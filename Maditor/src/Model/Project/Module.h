
#pragma once

#include "Generator\CmakeSubProject.h"
#include "Generator\ClassGenerator.h"
#include "ProjectElement.h"

namespace Maditor {
	namespace Model {

		class Module : public QObject, public ProjectElement {
			Q_OBJECT
		public:
			Module(Project *parent, const QString &name);
			Module(Project *parent, QDomElement data);

			QString root();

			void addNewClass(Generator::ClassGenerator *generator);
			
			QString moduleInclude();

			bool hasClass(const QString &name);

			virtual QVariant icon() const override;
			

			bool addDependency(const QString &dep);
			void removeDependency(const QString &dep);
			const QStringList &dependencies();
			const std::set<Module *> &dependencyModules();
			void initDependencies();

			void fillReloadOrder(std::list<const Module*> &reloadOrder) const;

			Project *parent();

			// Inherited via ProjectElement
			virtual int childCount() override;

			virtual Generator::ClassGenerator * child(int i) override;

		private:
			void addClass(Generator::ClassGenerator *generator);

			void init();

		signals:
			void newClassRequest();
			void propertiesDialogRequest(Module *);

		private:
			Project *mParent;

			Generator::CmakeSubProject mCmake;

			std::list<std::unique_ptr<Generator::ClassGenerator>> mClasses;
			
			std::set<Module *> mDependencies, mDependedBy;



		};

	}
}
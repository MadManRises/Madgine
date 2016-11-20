
#pragma once

#include "Generators\CmakeSubProject.h"
#include "Generators\ClassGenerator.h"
#include "ProjectElement.h"

namespace Maditor {
	namespace Model {

		class Module : public QObject, public ProjectElement {
			Q_OBJECT
		public:
			Module(ModuleList *parent, const QString &name);
			Module(QDomElement data, ModuleList *parent);

			virtual QString path() const override;

			void addNewClass(Generators::ClassGenerator *generator);
			
			QString moduleInclude();

			bool hasClass(const QString &name);

			virtual QVariant icon() const override;
			

			bool addDependency(const QString &dep);
			void removeDependency(const QString &dep);
			const QStringList &dependencies();
			const std::set<Module *> &dependencyModules();
			void initDependencies();

			void fillReloadOrder(std::list<const Module*> &reloadOrder) const;

			ModuleList *parent();

			// Inherited via ProjectElement
			virtual int childCount() override;

			virtual Generators::ClassGenerator * child(int i) override;

			virtual Project *project() override;

			const std::list<std::unique_ptr<Generators::ClassGenerator>> &getClasses();

			void removeClass(Generators::ClassGenerator *generator);

		private:
			void addClass(Generators::ClassGenerator *generator, bool callInsert = true);

			void init();

		signals:
			void newClassRequest();
			void propertiesDialogRequest(Module *);

			void classAdded(Generators::ClassGenerator *generator);

		private:
			ModuleList *mParent;

			Generators::CmakeSubProject mCmake;

			std::list<std::unique_ptr<Generators::ClassGenerator>> mClasses;
			
			std::set<Module *> mDependencies, mDependedBy;



		};

	}
}
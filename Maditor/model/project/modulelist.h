#pragma once


#include "projectelement.h"

#include "generators/cmakeproject.h"

#include "module.h"

#include "applicationconfig.h"

namespace Maditor {
	namespace Model {
		class MADITOR_MODEL_EXPORT ModuleList : public QObject, public ProjectElement{
			Q_OBJECT
		public:

			ModuleList(Project *parent);
			ModuleList(QDomElement element, Project *parent);
			ModuleList(const ModuleList &) = delete;
			~ModuleList();

			virtual QString path() const override;

			void generate();

			Generators::CmakeProject *cmake();

			Generators::ClassGenerator *getClass(const QString &fullName);

			bool hasModule(const QString &name) const;

			Module *getModule(const QString &name);
			const Module *getModule(const QString &name) const;

			std::list<std::unique_ptr<Module>>::const_iterator begin() const;
			std::list<std::unique_ptr<Module>>::const_iterator end() const;

			virtual QVariant icon() const override;

			// Inherited via ProjectElement
			virtual int childCount() const override;

			virtual Module * child(int i) override;

			template <class T>
			T *getClass(const QString &fullName) {
				QStringList l = fullName.split("::");
				if (l.size() != 2)
					return nullptr;
				return getModule(l[0])->getClass<T>(l[1]);
			}

			template <class T>
			std::list<T*> getClasses() {
				std::list<T*> result;
				for (const std::unique_ptr<Module> &module : mModules) {
					result.splice(result.end(), module->getClasses<T>());
				}
				return result;
			}

		protected:
			virtual void writeData() override;

		public slots:
			void createModule(const QString &name);
			void drawDependenciesGraph();

			void updateConfigs(ApplicationConfig *config,
				ApplicationConfig::Launcher newLauncher, ApplicationConfig::Launcher oldLauncher,
				Shared::LauncherType newLauncherType, Shared::LauncherType oldLauncherType);

		private:
			void init();

			void addModule(Module *module);
			void newModule();

		signals:
			void moduleAdded(Module *);
			void classAdded(Generators::ClassGenerator*);
			

		private:
			Project *mParent;

			QString mPath;

			Generators::CmakeProject mCmake;

			std::list<std::unique_ptr<Module>> mModules;



		};
	}
}

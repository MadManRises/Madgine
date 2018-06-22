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

			virtual QDir path() const override;

			void generate();

			Generators::CmakeProject *cmake();

			Generators::ClassGenerator *getClass(const QString &fullName);

			bool hasModule(const QString &name) const;

			Module *getModule(const QString &name);
			const Module *getModule(const QString &name) const;

			std::vector<std::unique_ptr<Module>>::const_iterator begin() const;
			std::vector<std::unique_ptr<Module>>::const_iterator end() const;

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
			std::vector<T*> getClasses() {
				std::vector<T*> result;
				for (const std::unique_ptr<Module> &module : mModules) {
					std::vector<T*> v = module->getClasses<T>();
					std::copy(v.begin(), v.end(), std::back_inserter(result));					
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

			void addModule(std::unique_ptr<Module> &&module);
			void newModule();

		signals:
			void moduleAdded(Module *);
			void classAdded(Generators::ClassGenerator*);
			

		private:
			Project *mParent;

			QDir mPath;

			Generators::CmakeProject mCmake;

			std::vector<std::unique_ptr<Module>> mModules;



		};
	}
}

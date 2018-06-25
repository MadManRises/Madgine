#pragma once


#include "projectelement.h"

#include "cmakeserver.h"

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


			void generate();

			CmakeServer *cmake();

			Generators::ClassGenerator *getClass(const QString &fullName);

			bool hasModule(const QString &name) const;

			Module *getModule(const QString &name);
			const Module *getModule(const QString &name) const;

			std::vector<std::unique_ptr<Module>>::const_iterator begin() const;
			std::vector<std::unique_ptr<Module>>::const_iterator end() const;

			virtual int childCount() const override;
			virtual Module * child(int i) override;

			virtual QVariant icon() const override;
			virtual QDir path() const override;



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
			void deleteModule(Module *module);
			void newModule();
			
			void drawDependenciesGraph();

			void updateConfigs(ApplicationConfig *config,
				ApplicationConfig::Launcher newLauncher, ApplicationConfig::Launcher oldLauncher,
				Shared::LauncherType newLauncherType, Shared::LauncherType oldLauncherType);

		private:
			void init();

			void addModule(std::unique_ptr<Module> &&module);
			void removeModule(Module *module);
			

		signals:
			void moduleAdded(Module *);
			void moduleRemoved(Module *);
			void classAdded(Generators::ClassGenerator*);
			

		private:
			Project *mParent;

			QDir mPath;

			CmakeServer mCmake;

			std::vector<std::unique_ptr<Module>> mModules;



		};
	}
}

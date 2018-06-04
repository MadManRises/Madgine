#pragma once


#include "projectelement.h"

#include "generators/cmakeproject.h"

#include "module.h"

#include "applicationconfig.h"

namespace Maditor {
	namespace Model {
		class MADITOR_MODEL_EXPORT ConfigList : public QObject, public ProjectElement{
			Q_OBJECT

		public:

			ConfigList(Project *parent);
			ConfigList(QDomElement element, Project *parent);
			ConfigList(const ConfigList &) = delete;
			~ConfigList();			

			virtual QString path() const override;

			bool hasConfig(const QString &name) const;

			ApplicationConfig *getConfig(const QString &name);
			const ApplicationConfig *getConfig(const QString &name) const;


			std::list<std::unique_ptr<ApplicationConfig>>::const_iterator begin() const;
			std::list<std::unique_ptr<ApplicationConfig>>::const_iterator end() const;

			virtual QVariant icon() const override;

			// Inherited via ProjectElement
			virtual int childCount() const override;

			virtual ApplicationConfig * child(int i) override;

			QStringList getConfigs();

		public slots:
			void createConfig(const QString &name);

		private:
			void init();

			void addConfig(ApplicationConfig *module);
			void newConfig();

		signals:
			void configAdded(ApplicationConfig *);		
			void instanceAdded(ApplicationLauncher *);
			void instanceDestroyed(ApplicationLauncher *);

			void openConfig(ApplicationConfig *);

			void applicationLauncherTypeChanged(ApplicationConfig *, 
				ApplicationConfig::Launcher newLauncher, ApplicationConfig::Launcher oldLauncher, 
				Shared::LauncherType newLauncherType, Shared::LauncherType oldLauncherType);

		private slots:
			void launcherChanged(ApplicationConfig::Launcher launcher, ApplicationConfig::Launcher oldLauncher);
			void launcherTypeChanged(Shared::LauncherType type, Shared::LauncherType oldLauncherType);

		private:
			Project *mParent;

			std::list<std::unique_ptr<ApplicationConfig>> mConfigs;


		};
	}
}

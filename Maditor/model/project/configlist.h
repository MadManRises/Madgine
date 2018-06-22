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

			virtual QDir path() const override;

			bool hasConfig(const QString &name) const;

			ApplicationConfig *getConfig(const QString &name);
			const ApplicationConfig *getConfig(const QString &name) const;


			std::vector<std::unique_ptr<ApplicationConfig>>::const_iterator begin() const;
			std::vector<std::unique_ptr<ApplicationConfig>>::const_iterator end() const;

			virtual QVariant icon() const override;

			// Inherited via ProjectElement
			virtual int childCount() const override;

			virtual ApplicationConfig * child(int i) override;

			QStringList getConfigs();

		public slots:
			void createConfig(const QString &name);
			void deleteConfig(ApplicationConfig *config);

		private:
			void init();

			void addConfig(std::unique_ptr<ApplicationConfig> &&onfig);
			void removeConfig(ApplicationConfig *module);
			void newConfig();

		signals:
			void configAdded(ApplicationConfig *);		
			void configRemoved(ApplicationConfig *);
			void instanceAdded(const std::shared_ptr<ApplicationLauncher>&);
			void instanceDestroyed(const std::shared_ptr<ApplicationLauncher>&);

			void openConfig(ApplicationConfig *);

			void applicationLauncherTypeChanged(ApplicationConfig *, 
				ApplicationConfig::Launcher newLauncher, ApplicationConfig::Launcher oldLauncher, 
				Shared::LauncherType newLauncherType, Shared::LauncherType oldLauncherType);

		private slots:
			void launcherChanged(ApplicationConfig::Launcher launcher, ApplicationConfig::Launcher oldLauncher);
			void launcherTypeChanged(Shared::LauncherType type, Shared::LauncherType oldLauncherType);

		private:
			Project *mParent;

			std::vector<std::unique_ptr<ApplicationConfig>> mConfigs;


		};
	}
}

#include "maditormodellib.h"

#include "configlist.h"

#include "project.h"

#include "dialogmanager.h"

#include "application/applicationlauncher.h"

namespace Maditor {
	namespace Model {

		
		ConfigList::ConfigList(Project *parent) :
			ProjectElement("Configs", "Configs", parent),
			mParent(parent)
		{

		}

		ConfigList::ConfigList(QDomElement element, Project *parent) :
			ProjectElement(element, parent),
			mParent(parent)
		{
			init();

			for (QDomElement config = element.firstChildElement("Config"); !config.isNull(); config = config.nextSiblingElement("Config")) {
				addConfig(new ApplicationConfig(config, this));
			}

		}

		ConfigList::~ConfigList()
		{
		}

		QString ConfigList::path() const
		{
			return mParent->path();
		}

		void ConfigList::init()
		{
			setContextMenuItems({
				{ "New Config", [=]() {newConfig(); } }
			});
		}

		void ConfigList::addConfig(ApplicationConfig *config)
		{
			//beginInsertRows(index(0, 0), mModules.size(), mModules.size());
			mConfigs.emplace_back(config);
			//endInsertRows();

			connect(config, &ApplicationConfig::documentCreated, this, &ConfigList::instanceAdded);
			connect(config, &ApplicationConfig::documentDestroyed, this, &ConfigList::instanceDestroyed);
			
			connect(config, &ApplicationConfig::launcherChanged, this, &ConfigList::launcherChanged);
			connect(config, &ApplicationConfig::launcherTypeChanged, this, &ConfigList::launcherTypeChanged);

			//emit moduleAdded(module);
		}

		void ConfigList::launcherTypeChanged(Shared::LauncherType type, Shared::LauncherType oldLauncherType)
		{
			ApplicationConfig *conf = static_cast<ApplicationConfig*>(sender());
			emit applicationLauncherTypeChanged(conf, conf->launcher(), conf->launcher(), type, oldLauncherType);
		}

		void ConfigList::launcherChanged(ApplicationConfig::Launcher launcher, ApplicationConfig::Launcher oldLauncher)
		{
			ApplicationConfig *conf = static_cast<ApplicationConfig*>(sender());
			emit applicationLauncherTypeChanged(conf, launcher, oldLauncher, conf->launcherType(), conf->launcherType());
		}

		void ConfigList::newConfig()
		{
			QString name;
			if (DialogManager::showNewConfigDialogStatic(this, name)) {
				createConfig(name);
			}
		}

		void ConfigList::createConfig(const QString & name)
		{
			addConfig(new ApplicationConfig(this, name));

			mParent->writeToDisk();
		}

		bool ConfigList::hasConfig(const QString & name) const
		{
			return std::find_if(mConfigs.begin(), mConfigs.end(), [&](const std::unique_ptr<ApplicationConfig> &ptr) {return ptr->name() == name; }) != mConfigs.end();
		}

		ApplicationConfig * ConfigList::getConfig(const QString & name)
		{
			auto it = std::find_if(mConfigs.begin(), mConfigs.end(), [&](const std::unique_ptr<ApplicationConfig> &mod) {return mod->name() == name; });
			if (it == mConfigs.end())
				throw 0;
			return it->get();
		}

		const ApplicationConfig * ConfigList::getConfig(const QString & name) const
		{
			auto it = std::find_if(mConfigs.begin(), mConfigs.end(), [&](const std::unique_ptr<ApplicationConfig> &mod) {return mod->name() == name; });
			if (it == mConfigs.end())
				throw 0;
			return it->get();
		}

		std::list<std::unique_ptr<ApplicationConfig>>::const_iterator ConfigList::begin() const
		{
			return mConfigs.begin();
		}

		std::list<std::unique_ptr<ApplicationConfig>>::const_iterator ConfigList::end() const
		{
			return mConfigs.end();
		}

		int ConfigList::childCount() const {
			return mConfigs.size();
		}

		ApplicationConfig * ConfigList::child(int i) {
			auto it = mConfigs.begin();
			std::advance(it, i);
			return it->get();
		}

		QStringList ConfigList::getConfigs()
		{
			QStringList result;
			for (const std::unique_ptr<ApplicationConfig> &config : *this) {
				result << config->name();
			}
			return result;
		}

		QVariant ConfigList::icon() const
		{
			QIcon icon;
			icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DirHomeIcon),
				QIcon::Normal);
			return icon;
		}


	}
}

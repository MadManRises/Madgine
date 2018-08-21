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
			init();
		}

		ConfigList::ConfigList(QDomElement element, Project *parent) :
			ProjectElement(element, parent),
			mParent(parent)
		{
			init();

			for (QDomElement config = element.firstChildElement("Config"); !config.isNull(); config = config.nextSiblingElement("Config")) {
				addConfig(std::make_unique<ApplicationConfig>(config, this));
			}

		}

		ConfigList::~ConfigList()
		{
		}

		QDir ConfigList::path() const
		{
			return mParent->path();
		}

		void ConfigList::init()
		{
			setContextMenuItems({
				{ "New Config", [=]() {newConfig(); } }
			});
		}

		void ConfigList::addConfig(std::unique_ptr<ApplicationConfig> &&config)
		{

			bool b = beginInsertRows(static_cast<int>(mConfigs.size()), static_cast<int>(mConfigs.size()));
			ApplicationConfig *c = mConfigs.emplace_back(std::forward<std::unique_ptr<ApplicationConfig>>(config)).get();
			endInsertRows(b);

			connect(c, &ApplicationConfig::documentCreated, this, &ConfigList::instanceAdded);
			connect(c, &ApplicationConfig::documentDestroyed, this, &ConfigList::instanceDestroyed);
			
			connect(c, &ApplicationConfig::launcherChanged, this, &ConfigList::launcherChanged);
			connect(c, &ApplicationConfig::launcherTypeChanged, this, &ConfigList::launcherTypeChanged);

			emit configAdded(c);
		}

		void ConfigList::removeConfig(ApplicationConfig* config)
		{
			emit configRemoved(config);

			auto it = std::find_if(mConfigs.begin(), mConfigs.end(), [=](const std::unique_ptr<ApplicationConfig> &conf) {return conf.get() == config; });
			int i = static_cast<int>(std::distance(mConfigs.begin(), it));
			bool b = beginRemoveRows(i, i);
			mConfigs.erase(it);
			endRemoveRows(b);
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
			addConfig(std::make_unique<ApplicationConfig>(this, name));

			mParent->writeToDisk();
		}

		void ConfigList::deleteConfig(ApplicationConfig* config)
		{		
			if (DialogManager::showDeleteConfigDialogStatic(config)) {
				removeConfig(config);
				mParent->writeToDisk();
			}
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

		std::vector<std::unique_ptr<ApplicationConfig>>::const_iterator ConfigList::begin() const
		{
			return mConfigs.begin();
		}

		std::vector<std::unique_ptr<ApplicationConfig>>::const_iterator ConfigList::end() const
		{
			return mConfigs.end();
		}

		int ConfigList::childCount() const {
			return static_cast<int>(mConfigs.size());
		}

		ApplicationConfig * ConfigList::child(int i) {
			return mConfigs[i].get();
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

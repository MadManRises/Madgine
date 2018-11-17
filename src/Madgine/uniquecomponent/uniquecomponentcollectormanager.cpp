#include "../baselib.h"

#include "uniquecomponentcollectormanager.h"

#include "uniquecomponentcollector.h"

#include "Interfaces/plugins/pluginmanager.h"

namespace Engine {



	UniqueComponentCollectorManager::UniqueComponentCollectorManager(Plugins::PluginManager & pluginMgr) :
		mMgr(pluginMgr)
	{
		mMgr.addListener(this);
		for (std::pair<const std::string, Plugins::PluginSection> &sec : pluginMgr) {
			onSectionAdded(&sec.second);
		}
		Plugins::Plugin base("Base");
		base.load();
		onPluginLoad(&base);
	}

	UniqueComponentCollectorManager::~UniqueComponentCollectorManager()
	{
		Plugins::Plugin base("Base");
		base.load();
		aboutToUnloadPlugin(&base);
		for (std::pair<const std::string, Plugins::PluginSection> &sec : mMgr) {
			aboutToRemoveSection(&sec.second);
		}
		mMgr.removeListener(this);
	}

	void UniqueComponentCollectorManager::onSectionAdded(Plugins::PluginSection * sec)
	{
		sec->addListener(this);
		for (const std::pair<const std::string, Plugins::Plugin> &p : *sec) {
			if (p.second.isLoaded())
				onPluginLoad(&p.second);
		}
	}

	void UniqueComponentCollectorManager::aboutToRemoveSection(Plugins::PluginSection * sec)
	{
		for (const std::pair<const std::string, Plugins::Plugin> &p : *sec) {
			if (p.second.isLoaded())
				aboutToUnloadPlugin(&p.second);
		}
		sec->removeListener(this);
	}

	void UniqueComponentCollectorManager::onPluginLoad(const Plugins::Plugin * p)
	{
#ifndef STATIC_BUILD
		auto f = (std::vector<CollectorInfo*>&(*)())p->getSymbol("collectorRegistry");

		if (f) {
			for (CollectorInfo *info : f()) {
				registryRegistry().at(info->mRegistryInfo->mFullName)->addCollector(info);
			}
		}
#endif
	}

	bool UniqueComponentCollectorManager::aboutToUnloadPlugin(const Plugins::Plugin * p)
	{
#ifndef STATIC_BUILD
		auto f = (std::vector<CollectorInfo*>&(*)())p->getSymbol("collectorRegistry");

		if (f) {
			for (CollectorInfo *info : f()) {
				registryRegistry().at(info->mRegistryInfo->mFullName)->removeCollector(info);
			}
		}
#endif
		return true;
	}

}
#include "../baselib.h"

#include "uniquecomponentcollectormanager.h"

#include "uniquecomponentcollector.h"

#include "Interfaces/plugins/pluginmanager.h"

namespace Engine {



	UniqueComponentCollectorManager::UniqueComponentCollectorManager(Plugins::PluginManager & pluginMgr) :
		mMgr(pluginMgr)
	{
		mMgr.addListener(this);
		Plugins::Plugin base("Base");
		base.load();
		onPluginLoad(&base);
	}

	UniqueComponentCollectorManager::~UniqueComponentCollectorManager()
	{
		Plugins::Plugin base("Base");
		base.load();
		aboutToUnloadPlugin(&base);
		mMgr.removeListener(this);
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
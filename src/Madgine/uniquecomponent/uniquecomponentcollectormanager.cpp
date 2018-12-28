#include "../baselib.h"

#include "uniquecomponentcollectormanager.h"

#include "uniquecomponentcollector.h"

#include "Interfaces/plugins/pluginmanager.h"

namespace Engine {



	UniqueComponentCollectorManager::UniqueComponentCollectorManager(Plugins::PluginManager & pluginMgr) :
		mMgr(pluginMgr)
	{
		mMgr.addListener(this);
	}

	UniqueComponentCollectorManager::~UniqueComponentCollectorManager()
	{
		mMgr.removeListener(this);
	}

	void UniqueComponentCollectorManager::onPluginLoad(const Plugins::Plugin * p)
	{
#ifndef STATIC_BUILD
		const Plugins::BinaryInfo *info = (const Plugins::BinaryInfo *)p->getSymbol("binaryInfo");

		for (auto &[name, reg] : registryRegistry()) {
			reg->onPluginLoad(info);
		}
#endif
	}

	bool UniqueComponentCollectorManager::aboutToUnloadPlugin(const Plugins::Plugin * p)
	{
#ifndef STATIC_BUILD
		const Plugins::BinaryInfo *info = (const Plugins::BinaryInfo *)p->getSymbol("binaryInfo");

		for (auto &[name, reg] : registryRegistry()) {
			reg->onPluginUnload(info);
		}
#endif
		return true;
	}

}
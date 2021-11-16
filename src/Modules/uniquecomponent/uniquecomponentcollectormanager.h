#pragma once

#if ENABLE_PLUGINS

namespace Engine {

	struct MODULES_EXPORT UniqueComponentCollectorManager{

		UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr);

	private:
		Plugins::PluginManager &mMgr;

	};

}

#endif
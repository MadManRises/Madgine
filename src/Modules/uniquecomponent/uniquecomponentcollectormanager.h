#pragma once

#if ENABLE_PLUGINS

#include "../plugins/pluginlistener.h"

namespace Engine {

	struct MODULES_EXPORT UniqueComponentCollectorManager : Plugins::PluginListener{

		UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr);
		virtual ~UniqueComponentCollectorManager();

	protected:
		void onPluginLoad(const Plugins::Plugin *p) override;
		bool aboutToUnloadPlugin(const Plugins::Plugin *p) override;

	private:
		Plugins::PluginManager &mMgr;

	};

}

#endif
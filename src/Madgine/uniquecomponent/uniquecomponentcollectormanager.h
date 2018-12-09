#pragma once

#include "Interfaces/plugins/pluginlistener.h"
#include "Interfaces/plugins/pluginsectionlistener.h"

namespace Engine {

	struct UniqueComponentCollectorManager : Plugins::PluginListener{

		UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr);
		~UniqueComponentCollectorManager();

	protected:
		void onPluginLoad(const Plugins::Plugin *p) override;
		bool aboutToUnloadPlugin(const Plugins::Plugin *p) override;

	private:
		Plugins::PluginManager &mMgr;

	};

}
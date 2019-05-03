#pragma once

#ifndef STATIC_BUILD

#include "../plugins/pluginlistener.h"

namespace Engine {

	struct INTERFACES_EXPORT UniqueComponentCollectorManager : Plugins::PluginListener{

		UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr);
		~UniqueComponentCollectorManager();

	protected:
		void onPluginLoad(const Plugins::Plugin *p) override;
		bool aboutToUnloadPlugin(const Plugins::Plugin *p) override;

	private:
		Plugins::PluginManager &mMgr;

	};

}

#endif
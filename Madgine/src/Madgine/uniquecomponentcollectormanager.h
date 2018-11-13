#pragma once

#include "Interfaces/plugins/pluginmanager.h"

namespace Engine {

	struct UniqueComponentCollectorManager : Plugins::PluginListener, Plugins::PluginSectionListener {

		UniqueComponentCollectorManager(Plugins::PluginManager &pluginMgr);
		~UniqueComponentCollectorManager();

	protected:
		void onSectionAdded(Plugins::PluginSection *sec) override;
		void aboutToRemoveSection(Plugins::PluginSection *sec) override;

		void onPluginLoad(const Plugins::Plugin *p) override;
		bool aboutToUnloadPlugin(const Plugins::Plugin *p) override;

	private:
		Plugins::PluginManager &mMgr;

	};

}
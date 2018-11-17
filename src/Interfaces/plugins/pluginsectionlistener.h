#pragma once

namespace Engine {
	namespace Plugins {
		

		struct PluginSectionListener {
			virtual void onSectionAdded(PluginSection *sec) {}
			virtual void aboutToRemoveSection(PluginSection *sec) {}
		};

	}
}
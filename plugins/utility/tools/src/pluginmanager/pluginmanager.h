#pragma once

#ifndef STATIC_BUILD

#include "../tool.h"
#include "../tinyxml/tinyxml2.h"

namespace Engine
{
	namespace Tools
	{

		class PluginManager : public Tool<PluginManager>
		{
		public:
			PluginManager(ImGuiRoot &root);

			virtual void render() override;

			const char* key() const override;

		private:
			Plugins::PluginManager &mManager;
		};

	}
}

RegisterType(Engine::Tools::PluginManager);

#endif
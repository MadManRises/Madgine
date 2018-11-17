#pragma once

#include "Madgine/app/globalapicomponent.h"
#include "Madgine/core/framelistener.h"

#include "../toolscollector.h"

#include "Madgine/uniquecomponent/uniquecomponentcontainer.h"

#include "Interfaces/plugins/pluginlistener.h"

namespace Engine
{
	namespace Tools
	{
		
		class MADGINE_TOOLS_EXPORT ImGuiRoot : public App::GlobalAPIComponent<ImGuiRoot>, public Core::FrameListener, public Plugins::PluginListener
		{
		public:
			ImGuiRoot(App::Application &app);
			~ImGuiRoot();

			bool init() override;
			void finalize() override;

			bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
			bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

			bool aboutToUnloadPlugin(const Plugins::Plugin *p) override;
			void onPluginLoad(const Plugins::Plugin *p) override;

		private:
			void createManager();
			void destroyManager();

		private:
			std::unique_ptr<ImGuiManager> mManager;

			ToolsContainer mCollector;

		};

	}
}

RegisterClass(Engine::Tools::ImGuiRoot);
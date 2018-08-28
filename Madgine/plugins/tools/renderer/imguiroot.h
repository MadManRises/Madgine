#pragma once

#include "Madgine/scripting/types/globalapicomponent.h"
#include "Madgine/core/framelistener.h"

#include "../toolscollector.h"

#include "Madgine/uniquecomponentcollectorinstance.h"

namespace Engine
{
	namespace Tools
	{
		
		class __declspec(dllexport) ImGuiRoot : public Scripting::GlobalAPIComponent<ImGuiRoot>, public Core::FrameListener
		{
		public:
			ImGuiRoot(App::Application &app);
			~ImGuiRoot();

			virtual bool init() override;
			virtual void finalize() override;

			virtual bool frameStarted(std::chrono::microseconds timeSinceLastFrame) override;
			virtual bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

		private:
			std::unique_ptr<Ogre::ImguiManager> mManager;

			ToolsCollectorInstance mCollector;

		};

	}
}

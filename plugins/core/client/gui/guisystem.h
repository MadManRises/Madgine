#pragma once

#include "Interfaces/scripting/types/scope.h"
#include "Madgine/scene/scenecomponentbase.h"
#include "Madgine/core/madgineobject.h"
#include "Interfaces/uniquecomponent/uniquecomponentselector.h"
#include "../render/renderercollector.h"
#include "Interfaces/threading/framelistener.h"
#include "Madgine/app/globalapicollector.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT GUISystem :
			public App::GlobalAPI<GUISystem>, 
			public Threading::FrameListener
		{
		public:
			GUISystem(App::Application &app);
			GUISystem(const GUISystem &) = delete;
			virtual ~GUISystem();			

			Widget* getWidget(const std::string& name);		

			UI::UIManager &ui(bool = true);

			GUISystem &getSelf(bool = true);
			
			void registerWidget(Widget* w);

			void unregisterWidget(Widget *w);
			
			void clear();

			KeyValueMapList maps() override;

			const std::vector<std::unique_ptr<TopLevelWindow>> &topLevelWindows();			

			void closeTopLevelWindow(TopLevelWindow *w);

			Render::RendererBase &renderer();

		protected:

			virtual bool init() override;
			virtual void finalize() override;
			
			TopLevelWindow *createTopLevelWindow();

			bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

		private:

			std::map<std::string, Widget *> mWidgets;
			std::vector<std::unique_ptr<TopLevelWindow>> mWindows;			

			std::unique_ptr<UI::UIManager> mUI;


			Render::RendererSelector mRenderer;

		};
	}
}

RegisterType(Engine::GUI::GUISystem);
#pragma once

#include "Interfaces/scripting/types/scope.h"
#include "../scene/scenecomponentbase.h"
#include "../core/madgineobject.h"
#include "../uniquecomponent/uniquecomponentselector.h"
#include "../render/renderercollector.h"
#include "../core/framelistener.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT GUISystem :
			public Scripting::Scope<GUISystem>, public Core::MadgineObject, public Core::FrameListener
		{
		public:
			GUISystem(App::ClientApplication &app);
			GUISystem(const GUISystem &) = delete;
			virtual ~GUISystem();			

			Widget* getWidgetByName(const std::string& name);		



			App::ClientApplication &app(bool = true);

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);
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

			App::ClientApplication &mApp;

			std::unique_ptr<UI::UIManager> mUI;


			Render::RendererSelector mRenderer;

		};
	}
}

RegisterType(Engine::GUI::GUISystem);
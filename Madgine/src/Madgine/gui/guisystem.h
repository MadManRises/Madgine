#pragma once

#include "../scripting/types/scope.h"
#include "../scene/scenecomponentbase.h"
#include "../core/frameloop.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT GUISystem :			
			public Scripting::Scope<GUISystem>,
			public Core::FrameLoop
		{
		public:
			GUISystem(App::ClientApplication &app);
			GUISystem(const GUISystem &) = delete;
			virtual ~GUISystem();

			

			virtual int go() override = 0;

			

			Widget* getWidgetByName(const std::string& name);
			



			App::ClientApplication &app(bool = true);

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);
			UI::UIManager &ui(bool = true);

			GUISystem &getSelf(bool = true);

			
			void registerWidget(Widget* w);

			void unregisterWidget(Widget *w);
			
			void clear();

			KeyValueMapList maps() override;

			bool singleFrame() override;

			const std::vector<std::unique_ptr<TopLevelWindow>> &topLevelWindows();

		protected:

			virtual bool init() override;
			virtual void finalize() override;

			bool sendFrameRenderingQueued(std::chrono::microseconds timeSinceLastFrame);
			
			void addTopLevelWindow(std::unique_ptr<TopLevelWindow> &&window);

		private:

			std::map<std::string, Widget *> mWidgets;
			std::vector<std::unique_ptr<TopLevelWindow>> mWindows;

			App::ClientApplication &mApp;

			std::unique_ptr<UI::UIManager> mUI;

		};
	}
}

#pragma once

#include "Madgine/scene/scenecomponentbase.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"
#include "../render/renderercollector.h"
#include "Madgine/threading/framelistener.h"
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


			GUISystem &getSelf(bool = true);

			
			void clear();

			//KeyValueMapList maps() override;

			std::vector<std::unique_ptr<TopLevelWindow>> &topLevelWindows();			

			void closeTopLevelWindow(TopLevelWindow *w);

			Render::RendererBase &renderer();

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);

			const std::vector<std::unique_ptr<TopLevelWindow>> &windows();

		protected:

			virtual bool init() override;
			virtual void finalize() override;
			
			TopLevelWindow *createTopLevelWindow();

			bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

		private:

			std::vector<std::unique_ptr<TopLevelWindow>> mWindows;			

			Render::RendererSelector mRenderer;

		};
	}
}

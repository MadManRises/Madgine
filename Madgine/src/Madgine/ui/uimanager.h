#pragma once

#include "guihandler.h"
#include "gamehandler.h"
#include "../scripting/types/scope.h"
#include "../core/framelistener.h"


namespace Engine
{
	namespace UI
	{
		class MADGINE_CLIENT_EXPORT UIManager : public Scripting::Scope<UIManager>,
			public MadgineObject, public Core::FrameListener
		{
		public:
			UIManager(GUI::GUISystem &gui);
			~UIManager();

			
			void clear();

			void hideCursor(bool keep = true);
			void showCursor();
			bool isCursorVisible() const;

			void swapCurrentRoot(GuiHandlerBase* newRoot);
			void openModalWindow(GuiHandlerBase* handler);
			void closeModalWindow(GuiHandlerBase* handler);
			void openWindow(GuiHandlerBase* handler);
			void closeWindow(GuiHandlerBase* handler);

			virtual bool frameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context);
			virtual bool frameFixedUpdate(float timeSinceLastFrame, Scene::ContextMask context);

			Scene::ContextMask currentContext();

			GUI::GUISystem &gui(bool = true) const;

			std::set<GameHandlerBase*> getGameHandlers();
			std::set<GuiHandlerBase*> getGuiHandlers();
			App::Application& app(bool = true);

			static const constexpr int sMaxInitOrder = 4;

			const char* key() const override;

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

			GameHandlerBase &getGameHandler(size_t i, bool = true);

			GuiHandlerBase &getGuiHandler(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);

			UIManager &getSelf(bool = true);

		protected:

			bool init() override;
			void finalize() override;

			KeyValueMapList maps() override;

		private:
			GUI::GUISystem &mGUI;
			
			GuiHandlerCollector mGuiHandlers;
			GameHandlerCollector mGameHandlers;

			GuiHandlerBase* mCurrentRoot;			

			std::stack<GuiHandlerBase *> mModalWindowList;

		private:
			Vector2 mKeptCursorPosition;
			bool mKeepingCursorPos;
		};
	}
}

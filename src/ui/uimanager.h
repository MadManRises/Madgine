#pragma once

#include "guihandler.h"
#include "gamehandler.h"
#include "../scripting/types/scope.h"
#include "../scripting/types/globalapicomponentbase.h"


namespace Engine
{
	namespace UI
	{
		class OGREMADGINE_EXPORT UIManager : public Scripting::Scope<UIManager>,
		                                     public MadgineObject
		{
		public:
			UIManager(GUI::GUISystem &gui);
			~UIManager();

			bool preInit();
			bool init() override;
			void finalize() override;

			void clear();

			void hideCursor(bool keep = true);
			void showCursor();
			bool isCursorVisible() const;

			void swapCurrentRoot(GuiHandlerBase* newRoot);
			void openModalWindow(GuiHandlerBase* handler);
			void closeModalWindow(GuiHandlerBase* handler);
			void openWindow(GuiHandlerBase* handler);
			void closeWindow(GuiHandlerBase* handler);

			void update(float timeSinceLastFrame);
			void fixedUpdate(float timeStep);

			Scene::ContextMask currentContext();

			GUI::GUISystem &gui() const;

			std::set<GameHandlerBase*> getGameHandlers();
			std::set<GuiHandlerBase*> getGuiHandlers();
			App::Application& app();

			static const constexpr int sMaxInitOrder = 4;

			const char* key() const override;

			Scene::SceneComponentBase &getSceneComponent(size_t i);

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i);

			GameHandlerBase &getGameHandler(size_t i);

			GuiHandlerBase &getGuiHandler(size_t i);

			Scene::SceneManagerBase &sceneMgr();

		protected:
			KeyValueMapList maps() override;

		private:
			GUI::GUISystem &mGUI;
			
			GuiHandlerCollector mGuiHandlers;
			GameHandlerCollector mGameHandlers;

			GuiHandlerBase* mCurrentRoot;			

			std::stack<GuiHandlerBase *> mModalWindowList;

		private:
			Ogre::Vector2 mKeptCursorPosition;
			bool mKeepingCursorPos;
		};
	}
}

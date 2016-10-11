#pragma once

#include "uniquecomponentcollector.h"
#include "guihandler.h"
#include "gamehandler.h"

namespace Engine {
	namespace UI {

		template MADGINE_EXPORT class UniqueComponentCollector<GuiHandlerBase>;
		template MADGINE_EXPORT class UniqueComponentCollector<GameHandlerBase>;

		class MADGINE_EXPORT UIManager : public Ogre::Singleton<UIManager>,
			public Ogre::WindowEventListener,
			 public Ogre::GeneralAllocatedObject,
			public Scripting::GlobalAPI<UIManager>
		{
		public:
			UIManager(Ogre::RenderWindow *window, OGRE::SceneManager *sceneMgr, GUI::GUISystem *gui);
			~UIManager();

			void init();

			void openLoadingScreen();

			void hideCursor(bool keep = true);

			void showCursor();
			//Scripting::ValueType showCursor(const Scripting::ArgumentList &args);

			bool isCursorVisible();

			void setWindowProperties(bool fullscreen, size_t width, size_t height);

			virtual void windowResized(Ogre::RenderWindow * rw) override;

			//void windowClosed(Ogre::RenderWindow * rw);

			void swapCurrentRoot(GuiHandlerBase * newRoot);

			void openModalWindow(GuiHandlerBase *handler);

			void openWindow(GuiHandlerBase *handler);

			void closeModalWindow(GuiHandlerBase *handler);

			void closeWindow(GuiHandlerBase *handler);

			void update(float timeSinceLastFrame);

			void callSafe(std::function<void()> f);

			GUI::GUISystem *gui();

			void renderFrame();

			Ogre::RenderWindow *renderWindow();

			std::set<GameHandlerBase*> getGameHandlers();
			std::set<GuiHandlerBase*> getGuiHandlers();

			static const constexpr size_t sMaxInitOrder = 4;

		private:
			Ogre::unique_ptr<UniqueComponentCollector<GuiHandlerBase>> mGuiHandlers;
			Ogre::unique_ptr<UniqueComponentCollector<GameHandlerBase>> mGameHandlers;

			GuiHandlerBase *mCurrentRoot;

			Ogre::RenderWindow *mWindow;

			std::queue<std::function<void()>> mSafeCallQueue;

			OGRE::SceneManager *mSceneMgr;
			GUI::GUISystem *mGUI;

			std::stack<UI::GuiHandlerBase *> mModalWindowList;

		private:
			Ogre::Vector2 mKeptCursorPosition;
			bool mKeepingCursorPos;
		};

	}
}
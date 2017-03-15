#pragma once

#include "guihandler.h"
#include "gamehandler.h"

namespace Engine {



	namespace UI {

		class OGREMADGINE_EXPORT UIManager : public Singleton<UIManager>,
			public MadgineObject<UIManager>,
			public Scripting::GlobalAPI<UIManager>,
			public Ogre::GeneralAllocatedObject
		{
		public:
			UIManager(GUI::GUISystem *gui);
			~UIManager();

			virtual bool init() override;
			virtual void finalize() override;

			void clear();

			void hideCursor(bool keep = true);
			void showCursor();
			bool isCursorVisible();

			void swapCurrentRoot(GuiHandlerBase * newRoot);
			void openModalWindow(GuiHandlerBase *handler);
			void closeModalWindow(GuiHandlerBase *handler);
			void openWindow(GuiHandlerBase *handler);
			void closeWindow(GuiHandlerBase *handler);

			void update(float timeSinceLastFrame);

			App::ContextMask currentContext();

			GUI::GUISystem *gui();

			std::set<GameHandlerBase*> getGameHandlers();
			std::set<GuiHandlerBase*> getGuiHandlers();

			static const constexpr int sMaxInitOrder = 4;

		private:
			OgreUniqueComponentCollector<GuiHandlerBase> mGuiHandlers;
			OgreUniqueComponentCollector<GameHandlerBase> mGameHandlers;

			GuiHandlerBase *mCurrentRoot;			

			GUI::GUISystem *mGUI;

			std::stack<GuiHandlerBase *> mModalWindowList;

		private:
			Ogre::Vector2 mKeptCursorPosition;
			bool mKeepingCursorPos;
		};

	}
}
#pragma once

#include "guihandler.h"
#include "gamehandler.h"
#include "Scripting\Types\scope.h"
#include "Scripting\Types\globalapicomponentbase.h"

namespace Engine {



	namespace UI {

		class OGREMADGINE_EXPORT UIManager : public Singleton<UIManager>,
			public Scripting::Scope<UIManager, MadgineObject>			
		{
		public:
			UIManager(GUI::GUISystem *gui);
			~UIManager();

			bool preInit();
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
			void fixedUpdate(float timeStep);

			Scene::ContextMask currentContext();

			GUI::GUISystem *gui();

			std::set<GameHandlerBase*> getGameHandlers();
			std::set<GuiHandlerBase*> getGuiHandlers();

			static const constexpr int sMaxInitOrder = 4;
			static const std::string sName;

			const std::string &key() const;

		protected:
			virtual Scripting::KeyValueMapList maps() override;

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
#include "madginelib.h"

#include "UIManager.h"

#include "GUI/GUISystem.h"

#include "GUI/Windows/Window.h"

#include "gamehandler.h"


namespace Engine {

	API_IMPL(UI::UIManager, &showCursor);

	namespace UI {


		UIManager::UIManager(GUI::GUISystem *gui) :
			mKeepingCursorPos(false),
			mGUI(gui),
			mCurrentRoot(0)
		{

			//Register as a Window listener
			

		}

		UIManager::~UIManager()
		{			
		}

		void UIManager::init()
		{
			MadgineObject::init();

			for (int i = 0; i < sMaxInitOrder; ++i)
				for (const Ogre::unique_ptr<UI::GuiHandlerBase> &handler : mGuiHandlers)
					handler->init(i);

			for (const Ogre::unique_ptr<UI::GameHandlerBase> &handler : mGameHandlers) {
				handler->init();
			}

		}

		void UIManager::finalize()
		{
			for (const Ogre::unique_ptr<UI::GameHandlerBase> &handler : mGameHandlers) {
				handler->finalize();
			}


			for (int i = sMaxInitOrder - 1; i >= -1; --i)
				for (const Ogre::unique_ptr<UI::GuiHandlerBase> &handler : mGuiHandlers)
					handler->finalize(i);

			MadgineObject::finalize();
		}

		void UIManager::clear()
		{
			while (!mModalWindowList.empty()) {
				closeModalWindow(mModalWindowList.top());
			}
		}

		void UIManager::hideCursor(bool keep)
		{
			if (!isCursorVisible()) return;
			mKeepingCursorPos = keep;
			if (keep) {
				/*const OIS::MouseState &mouseState = mMouse->getMouseState();
				mKeptCursorPosition = { (float)mouseState.X.abs, (float)mouseState.Y.abs };*/
			}
			mGUI->hideCursor();
			for (const Ogre::unique_ptr<UI::GameHandlerBase> &h : mGameHandlers) {
				h->onMouseVisibilityChanged(false);
			}
			for (const Ogre::unique_ptr<UI::GuiHandlerBase> &h : mGuiHandlers) {
				h->onMouseVisibilityChanged(false);
			}
		}

		void UIManager::showCursor()
		{
			if (isCursorVisible()) return;
			if (mKeepingCursorPos) {
				/*OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
				mutableMouseState.X.abs = mKeptCursorPosition.x;
				mutableMouseState.Y.abs = mKeptCursorPosition.y;
				callSafe([&]() {
					mouseMoved(OIS::MouseEvent(mMouse, mutableMouseState));*/
					mGUI->showCursor();
				/*});*/
			}
			else {
				mGUI->showCursor();
			}
			for (const Ogre::unique_ptr<UI::GameHandlerBase> &h : mGameHandlers) {
				h->onMouseVisibilityChanged(true);
			}
			for (const Ogre::unique_ptr<UI::GuiHandlerBase> &h : mGuiHandlers) {
				h->onMouseVisibilityChanged(true);
			}
		}

		bool UIManager::isCursorVisible()
		{
			return mGUI->isCursorVisible();
		}

		std::set<GameHandlerBase*> UIManager::getGameHandlers()
		{
			std::set<GameHandlerBase*> result;
			for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
				result.insert(h.get());
			}
			return result;
		}

		std::set<GuiHandlerBase*> UIManager::getGuiHandlers()
		{
			std::set<GuiHandlerBase*> result;
			for (const std::unique_ptr<GuiHandlerBase> &h : mGuiHandlers) {
				result.insert(h.get());
			}
			return result;
		}

		
		void UIManager::update(float timeSinceLastFrame)
		{
			App::ContextMask context = currentContext();

			for (const Ogre::unique_ptr<UI::GameHandlerBase> &h : mGameHandlers) {
				h->update(timeSinceLastFrame, context);
			}
		}

		App::ContextMask UIManager::currentContext()
		{
			return (mModalWindowList.empty() ? (mCurrentRoot ? mCurrentRoot->context() : App::ContextMask::NoContext) : mModalWindowList.top()->context());
		}


		void UIManager::swapCurrentRoot(UI::GuiHandlerBase *newRoot)
		{
			if (mCurrentRoot) mCurrentRoot->window()->hide();
			mCurrentRoot = newRoot;
			newRoot->window()->show();
		}

		void UIManager::openModalWindow(UI::GuiHandlerBase *handler)
		{
			handler->window()->showModal();
			mModalWindowList.emplace(handler);
		}

		void UIManager::openWindow(UI::GuiHandlerBase * handler)
		{
			handler->window()->show();
		}

		void UIManager::closeModalWindow(UI::GuiHandlerBase *handler)
		{
			assert(mModalWindowList.size() > 0 && mModalWindowList.top() == handler);
			handler->window()->hideModal();
			mModalWindowList.pop();
			if (mModalWindowList.size() > 0)
				mModalWindowList.top()->window()->activate();
			else
				mCurrentRoot->window()->activate();
		}

		void UIManager::closeWindow(UI::GuiHandlerBase * handler)
		{
			handler->window()->hide();
		}

		
		GUI::GUISystem * UIManager::gui()
		{
			return mGUI;
		}


	}
}
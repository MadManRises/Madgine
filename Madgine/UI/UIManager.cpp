#include "libinclude.h"

#include "UIManager.h"

#include "GUI/GUISystem.h"

#include "Ogre/scenemanager.h"

#include "GUI/Windows/Window.h"

#include "gamehandler.h"


namespace Engine {

	API_IMPL(UI::UIManager, &showCursor);

	namespace UI {


		UIManager::UIManager(Ogre::RenderWindow *window, OGRE::SceneManager *sceneMgr, GUI::GUISystem *gui) :
			mKeepingCursorPos(false),
			mWindow(window),
			mGUI(gui),
			mCurrentRoot(0),
			mSceneMgr(sceneMgr)
		{

			mGuiHandlers = OGRE_MAKE_UNIQUE(UniqueComponentCollector<GuiHandlerBase>)();

			mGameHandlers = OGRE_MAKE_UNIQUE(UniqueComponentCollector<GameHandlerBase>)();

			//Register as a Window listener
			Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

		}

		UIManager::~UIManager()
		{
			mGuiHandlers.reset();
			mGameHandlers.reset();

			Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
		}

		void UIManager::renderFrame()
		{
			mGUI->renderSingleFrame(); // Show Loading Screen
			mWindow->update();
		}

		void UIManager::init()
		{
			for (int i = 0; i < sMaxInitOrder; ++i)
				for (const Ogre::unique_ptr<UI::GuiHandlerBase> &handler : *mGuiHandlers)
					handler->init(i);

			for (const Ogre::unique_ptr<UI::GameHandlerBase> &handler : *mGameHandlers) {
				handler->init();
			}

			windowResized(mWindow);
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
			for (const Ogre::unique_ptr<UI::GameHandlerBase> &h : *mGameHandlers) {
				h->onMouseVisibilityChanged(false);
			}
			for (const Ogre::unique_ptr<UI::GuiHandlerBase> &h : *mGuiHandlers) {
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
			for (const Ogre::unique_ptr<UI::GameHandlerBase> &h : *mGameHandlers) {
				h->onMouseVisibilityChanged(true);
			}
			for (const Ogre::unique_ptr<UI::GuiHandlerBase> &h : *mGuiHandlers) {
				h->onMouseVisibilityChanged(true);
			}
		}

		/*Scripting::ValueType UIManager::showCursor(const Scripting::ArgumentList & args)
		{
			showCursor();
			return Scripting::ValueType();
		}*/

		bool UIManager::isCursorVisible()
		{
			return mGUI->isCursorVisible();
		}

		void UIManager::setWindowProperties(bool fullscreen, size_t width, size_t height)
		{
			mWindow->setFullscreen(fullscreen, width, height);
			windowResized(mWindow);
		}
		
		Ogre::RenderWindow * UIManager::renderWindow()
		{
			return mWindow;
		}

		std::set<GameHandlerBase*> UIManager::getGameHandlers()
		{
			std::set<GameHandlerBase*> result;
			for (const std::unique_ptr<GameHandlerBase> &h : *mGameHandlers) {
				result.insert(h.get());
			}
			return result;
		}

		std::set<GuiHandlerBase*> UIManager::getGuiHandlers()
		{
			std::set<GuiHandlerBase*> result;
			for (const std::unique_ptr<GuiHandlerBase> &h : *mGuiHandlers) {
				result.insert(h.get());
			}
			return result;
		}

		
		void UIManager::update(float timeSinceLastFrame)
		{
			
			//Need to inject timestamps to GUI System.
			mGUI->injectTimePulse(timeSinceLastFrame);

			App::ContextMask context = (mModalWindowList.empty() ? mCurrentRoot : mModalWindowList.top())->context();

			for (const Ogre::unique_ptr<UI::GameHandlerBase> &h : *mGameHandlers) {
				h->update(timeSinceLastFrame, context);
			}

			mSceneMgr->update(timeSinceLastFrame, context);

			

		}

		//---------------------------------------------------------------------------
		// Adjust mouse clipping area
		void UIManager::windowResized(Ogre::RenderWindow *rw)
		{
			unsigned int width, height, depth;
			int left, top;
			rw->getMetrics(width, height, depth, left, top);

			//Ogre::LogManager::getSingleton().getLog("Madgine.log")->logMessage(std::string("Setting Resolution to: ") + std::to_string(width) + "x" + std::to_string(height));

			mGUI->notifyDisplaySizeChanged(Ogre::Vector2(width, height));

			for (const Ogre::unique_ptr<UI::GuiHandlerBase> &handler : *mGuiHandlers) {
				if (handler->isRootWindow())
					handler->sizeChanged();
			}
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
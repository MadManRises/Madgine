#include "madginelib.h"

#include "uimanager.h"

#include "gui/guisystem.h"

#include "gui/windows/window.h"

#include "gamehandler.h"



namespace Engine {

	API_IMPL(UI::UIManager, MAP_F(showCursor));

	
#ifdef _MSC_VER
	template class OGREMADGINE_EXPORT OgreUniqueComponentCollector<UI::GuiHandlerBase>;
	template class OGREMADGINE_EXPORT OgreUniqueComponentCollector<UI::GameHandlerBase>;
#endif

	namespace UI {

		UIManager::UIManager(GUI::GUISystem *gui) :
			mCurrentRoot(nullptr),
			mGUI(gui),
			mKeepingCursorPos(false)			
		{
		}

		UIManager::~UIManager()
		{			
		}

		bool UIManager::preInit()
		{
			for (const std::unique_ptr<UI::GuiHandlerBase> &handler : mGuiHandlers)
				if (!handler->init(-1))
					return false;
			return MadgineObject::init();
		}

		bool UIManager::init()
		{

			for (int i = 0; i < sMaxInitOrder; ++i)
				for (const std::unique_ptr<UI::GuiHandlerBase> &handler : mGuiHandlers)
					if (!handler->init(i))
						return false;

			for (const std::unique_ptr<UI::GameHandlerBase> &handler : mGameHandlers) {
				if (!handler->init())
					return false;
			}

			return true;

		}

		void UIManager::finalize()
		{
			for (const std::unique_ptr<UI::GameHandlerBase> &handler : mGameHandlers) {
				handler->finalize();
			}


			for (int i = sMaxInitOrder - 1; i >= -1; --i)
				for (const std::unique_ptr<UI::GuiHandlerBase> &handler : mGuiHandlers)
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
			for (const std::unique_ptr<UI::GameHandlerBase> &h : mGameHandlers) {
				h->onMouseVisibilityChanged(false);
			}
			for (const std::unique_ptr<UI::GuiHandlerBase> &h : mGuiHandlers) {
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
			for (const std::unique_ptr<UI::GameHandlerBase> &h : mGameHandlers) {
				h->onMouseVisibilityChanged(true);
			}
			for (const std::unique_ptr<UI::GuiHandlerBase> &h : mGuiHandlers) {
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
			Scene::ContextMask context = currentContext();

			for (const std::unique_ptr<UI::GameHandlerBase> &h : mGameHandlers) {
				h->update(timeSinceLastFrame, context);
			}
		}
		
		void UIManager::fixedUpdate(float timeStep)
		{
			Scene::ContextMask context = currentContext();

			for (const std::unique_ptr<UI::GameHandlerBase> &h : mGameHandlers) {
				h->fixedUpdate(timeStep, context);
			}
		}

		Scene::ContextMask UIManager::currentContext()
		{
			return (mModalWindowList.empty() ? (mCurrentRoot ? mCurrentRoot->context() : Scene::ContextMask::NoContext) : mModalWindowList.top()->context());
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
			else if(mCurrentRoot)
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

		KeyValueMapList UIManager::maps()
		{
			return Scope::maps().merge(mGuiHandlers, mGameHandlers);
		}

		const char *UIManager::key() const
		{
			return "UI";
		}

	}
}
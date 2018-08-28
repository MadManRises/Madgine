#include "../clientlib.h"

#include "uimanager.h"

#include "../gui/guisystem.h"

#include "../gui/widgets/widget.h"

#include "../app/clientapplication.h"
#include "../app/clientappsettings.h"

#include "../generic/keyvalueiterate.h"

#include "../scripting/types/api.h"

RegisterClass(Engine::UI::UIManager);

namespace Engine
{	

	namespace UI
	{

		template MADGINE_CLIENT_EXPORT UniqueComponentCollector<GuiHandlerBase, UI::UIManager&>;
		template MADGINE_CLIENT_EXPORT UniqueComponentCollector<GameHandlerBase, UI::UIManager&>;

		UIManager::UIManager(GUI::GUISystem &gui) :
			Scope(&gui),
			mCurrentRoot(nullptr),
			mGUI(gui),
			mKeepingCursorPos(false),
		    mGuiHandlers(*this),
		    mGameHandlers(*this)
		{
			gui.addFrameListener(this);
		}

		UIManager::~UIManager()
		{
			mGUI.removeFrameListener(this);
		}

		UIManager& UIManager::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		bool UIManager::init()
		{			
			markInitialized();

        	if (mGUI.app().settings().mRunMain) {
				std::optional<Scripting::ArgumentList> res = app().callMethodIfAvailable("afterViewInit", {});
				if (res && !res->empty() && (!res->front().is<bool>() || !res->front().as<bool>()))
					return false;
			}
        
			for (const std::unique_ptr<GuiHandlerBase>& handler : mGuiHandlers)
				if (!handler->callInit())
					return false;

			for (const std::unique_ptr<GameHandlerBase>& handler : mGameHandlers)
			{
				if (!handler->callInit())
					return false;
			}

			return true;
		}

		void UIManager::finalize()
		{
			for (const std::unique_ptr<GameHandlerBase>& handler : mGameHandlers)
			{
				handler->callFinalize();
			}


			for (const std::unique_ptr<GuiHandlerBase>& handler : mGuiHandlers)
				handler->callFinalize();

		}

		void UIManager::clear()
		{
			while (!mModalWindowList.empty())
			{
				closeModalWindow(mModalWindowList.top());
			}
		}

		void UIManager::hideCursor(bool keep)
		{
			if (!isCursorVisible()) return;
			mKeepingCursorPos = keep;
			if (keep)
			{
				/*const OIS::MouseState &mouseState = mMouse->getMouseState();
				mKeptCursorPosition = { (float)mouseState.X.abs, (float)mouseState.Y.abs };*/
			}
			//mGUI.hideCursor();
			for (const std::unique_ptr<GameHandlerBase>& h : mGameHandlers)
			{
				h->onMouseVisibilityChanged(false);
			}
			for (const std::unique_ptr<GuiHandlerBase>& h : mGuiHandlers)
			{
				h->onMouseVisibilityChanged(false);
			}
		}

		void UIManager::showCursor()
		{
			if (isCursorVisible()) return;
			if (mKeepingCursorPos)
			{
				/*OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
				mutableMouseState.X.abs = mKeptCursorPosition.x;
				mutableMouseState.Y.abs = mKeptCursorPosition.y;
				callSafe([&]() {
					mouseMoved(OIS::MouseEvent(mMouse, mutableMouseState));*/
				//mGUI.showCursor();
				/*});*/
			}
			else
			{
				//mGUI.showCursor();
			}
			for (const std::unique_ptr<GameHandlerBase>& h : mGameHandlers)
			{
				h->onMouseVisibilityChanged(true);
			}
			for (const std::unique_ptr<GuiHandlerBase>& h : mGuiHandlers)
			{
				h->onMouseVisibilityChanged(true);
			}
		}

		bool UIManager::isCursorVisible() const
		{
			return /* mGUI.isCursorVisible()*/true;
		}

		std::set<GameHandlerBase*> UIManager::getGameHandlers()
		{
			std::set<GameHandlerBase*> result;
			for (const std::unique_ptr<GameHandlerBase>& h : mGameHandlers)
			{
				result.insert(h.get());
			}
			return result;
		}

		std::set<GuiHandlerBase*> UIManager::getGuiHandlers()
		{
			std::set<GuiHandlerBase*> result;
			for (const std::unique_ptr<GuiHandlerBase>& h : mGuiHandlers)
			{
				result.insert(h.get());
			}
			return result;
		}

		App::Application& UIManager::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGUI.app(init);
		}

		bool UIManager::frameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context)
		{
			for (const std::unique_ptr<GameHandlerBase>& h : mGameHandlers)
			{
				h->update(timeSinceLastFrame, context);
			}
			return true;
		}

		bool UIManager::frameFixedUpdate(float timeSinceLastFrame, Scene::ContextMask context)
		{
			for (const std::unique_ptr<GameHandlerBase>& h : mGameHandlers)
			{
				h->fixedUpdate(timeSinceLastFrame, context);
			}
			return true;
		}

		Scene::ContextMask UIManager::currentContext()
		{
			return mModalWindowList.empty()
				       ? (mCurrentRoot ? mCurrentRoot->context() : Scene::ContextMask::NoContext)
				       : mModalWindowList.top()->context();
		}


		void UIManager::swapCurrentRoot(GuiHandlerBase* newRoot)
		{
			if (mCurrentRoot) mCurrentRoot->widget()->hide();
			mCurrentRoot = newRoot;
			newRoot->widget()->show();
		}

		void UIManager::openModalWindow(GuiHandlerBase* handler)
		{
			handler->widget()->showModal();
			mModalWindowList.emplace(handler);
		}

		void UIManager::openWindow(GuiHandlerBase* handler)
		{
			handler->widget()->show();
		}

		void UIManager::closeModalWindow(GuiHandlerBase* handler)
		{
			assert(mModalWindowList.size() > 0 && mModalWindowList.top() == handler);
			handler->widget()->hideModal();
			mModalWindowList.pop();
			if (mModalWindowList.size() > 0)
				mModalWindowList.top()->widget()->activate();
			else if (mCurrentRoot)
				mCurrentRoot->widget()->activate();
		}

		void UIManager::closeWindow(GuiHandlerBase* handler)
		{
			handler->widget()->hide();
		}


		GUI::GUISystem &UIManager::gui(bool init) const
		{
			if (init)
			{
				checkInitState();
			}
			return mGUI.getSelf(init);
		}

		KeyValueMapList UIManager::maps()
		{
			return Scope::maps().merge(mGuiHandlers, mGameHandlers, MAP_F(showCursor));
		}

		const char* UIManager::key() const
		{
			return "UI";
		}

		Scene::SceneComponentBase& UIManager::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGUI.getSceneComponent(i, init);
		}

		Scripting::GlobalAPIComponentBase& UIManager::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGUI.getGlobalAPIComponent(i, init);
		}

		GameHandlerBase& UIManager::getGameHandler(size_t i, bool init)
		{
			GameHandlerBase &handler = mGameHandlers.get(i);
			if (init)
			{
				checkInitState();
				handler.callInit();
			}
			return handler.getSelf(init);
		}

		GuiHandlerBase& UIManager::getGuiHandler(size_t i, bool init)
		{
			GuiHandlerBase &handler = mGuiHandlers.get(i);
			if (init)
			{
				checkInitState();
				handler.callInit();
			}
			return handler.getSelf(init);
		}

		Scene::SceneManager& UIManager::sceneMgr(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGUI.sceneMgr(init);
		}
	}
}

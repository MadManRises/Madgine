#include "../clientlib.h"
#include "handler.h"
#include "../ui/uimanager.h"
#include "../gui/guisystem.h"
#include "../gui/windows/window.h"
#include "../app/clientapplication.h"


namespace Engine
{
	namespace UI
	{
		Handler::Handler(UIManager &ui, const std::string& windowName) :
			ScopeBase(ui.app(false).createTable()),
			mWindow(nullptr),
			mUI(ui),
			mWindowName(windowName),
		mMouseMoveSlot(this),
		mMouseDownSlot(this),
		mMouseUpSlot(this)
		{
		}


		Scene::SceneComponentBase& Handler::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.getSceneComponent(i, init);
		}

		Scene::SceneManager& Handler::sceneMgr(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.sceneMgr(init);
		}

		Scripting::GlobalAPIComponentBase& Handler::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.getGlobalAPIComponent(i, init);
		}

		UI::GuiHandlerBase& Handler::getGuiHandler(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.getGuiHandler(i, init);
		}

		GameHandlerBase& Handler::getGameHandler(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.getGameHandler(i, init);
		}

		bool Handler::installToWindow(GUI::Window* w)
		{
			mWindow = w;

			mWindow->mouseMoveEvent().connect(mMouseMoveSlot);
			mWindow->mouseDownEvent().connect(mMouseDownSlot);
			mWindow->mouseUpEvent().connect(mMouseUpSlot);

			for (const WindowDescriber& des : mWindows)
			{
				GUI::Window* window = w->getChildRecursive(des.mWindowName);

				if (!window)
				{
					LOG_ERROR(Database::Exceptions::windowNotFound(des.mWindowName));
					return false;
				}

				if (!des.mInit(window))
					return false;

			}

			mWindows.clear();

			return true;
		}


		void Handler::sizeChanged()
		{
		}

		App::Application& Handler::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.app(init);
		}

		UIManager& Handler::ui(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.getSelf(init);
		}

		bool Handler::init()
		{
			return installToWindow(mUI.gui().getWindowByName(mWindowName));
		}

		void Handler::finalize()
		{			
		}

		bool Handler::init(GUI::Window* window)
		{
			return installToWindow(window);				
		}

		void Handler::injectMouseMove(GUI::MouseEventArgs& evt)
		{
			onMouseMove(evt);
		}

		void Handler::injectMouseDown(GUI::MouseEventArgs& evt)
		{
			onMouseDown(evt);
		}

		void Handler::injectMouseUp(GUI::MouseEventArgs& evt)
		{
			onMouseUp(evt);
		}

		bool Handler::injectKeyPress(const GUI::KeyEventArgs& evt)
		{
			return onKeyPress(evt);
		}

		void Handler::onMouseMove(GUI::MouseEventArgs& me)
		{
		}

		void Handler::onMouseDown(GUI::MouseEventArgs& me)
		{
		}

		void Handler::onMouseUp(GUI::MouseEventArgs& me)
		{
		}

		bool Handler::onKeyPress(const GUI::KeyEventArgs& evt)
		{
			return false;
		}

		void Handler::registerWindow(const std::string& name, std::function<bool(GUI::Window*)> init)
		{
			assert(!mWindow);
			mWindows.emplace_back(name, init);
		}

		void Handler::onMouseVisibilityChanged(bool b)
		{
		}

		GUI::Window* Handler::window() const
		{
			return mWindow;
		}


	}
}

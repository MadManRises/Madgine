#include "../clientlib.h"
#include "handler.h"
#include "../ui/uimanager.h"
#include "../gui/guisystem.h"
#include "../gui/windows/window.h"
#include "../app/application.h"


namespace Engine
{
	namespace UI
	{
		Handler::Handler(UIManager &ui, const std::string& windowName) :
			ScopeBase(ui.gui().app().createTable()),
			mWindow(nullptr),
			mUI(ui),
			mWindowName(windowName)
		{
		}


		Scene::SceneComponentBase& Handler::getSceneComponent(size_t i)
		{
			return mUI.getSceneComponent(i);
		}

		Scene::SceneManager& Handler::sceneMgr()
		{
			return mUI.sceneMgr();
		}

		Scripting::GlobalAPIComponentBase& Handler::getGlobalAPIComponent(size_t i)
		{
			return mUI.getGlobalAPIComponent(i);
		}

		UI::GuiHandlerBase& Handler::getGuiHandler(size_t i)
		{
			return mUI.getGuiHandler(i);
		}

		GameHandlerBase& Handler::getGameHandler(size_t i)
		{
			return mUI.getGameHandler(i);
		}

		bool Handler::installToWindow(GUI::Window* w)
		{
			mWindow = w;

			//TODO connect

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

		App::Application& Handler::app()
		{
			return mUI.app();
		}

		UIManager& Handler::ui()
		{
			return mUI;
		}

		bool Handler::init()
		{
			if (!MadgineObject::init())
				return false;
			if (!installToWindow(mUI.gui().getWindowByName(mWindowName)))			
				return false;
			return true;
		}

		void Handler::finalize()
		{
			MadgineObject::finalize();
		}

		bool Handler::init(GUI::Window* window)
		{
			if (!MadgineObject::init())
				return false;
			if (!installToWindow(window))
				return false;
			return true;
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

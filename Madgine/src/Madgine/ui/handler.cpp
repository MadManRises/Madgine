#include "../clientlib.h"
#include "handler.h"
#include "../ui/uimanager.h"
#include "../gui/guisystem.h"
#include "../gui/widgets/widget.h"

namespace Engine
{
	namespace UI
	{
		Handler::Handler(UIManager &ui, const std::string& windowName) :
			ScopeBase(&ui),
			mWidget(nullptr),
			mUI(ui),
			mWidgetName(windowName),
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

		bool Handler::installToWidget(GUI::Widget* w)
		{
			mWidget = w;

			mWidget->mouseMoveEvent().connect(mMouseMoveSlot);
			mWidget->mouseDownEvent().connect(mMouseDownSlot);
			mWidget->mouseUpEvent().connect(mMouseUpSlot);

			for (const WindowDescriber& des : mWidgets)
			{
				GUI::Widget* window = w->getChildRecursive(des.mWidgetName);

				if (!window)
				{
					LOG_ERROR(Database::Exceptions::windowNotFound(des.mWidgetName));
					return false;
				}

				if (!des.mInit(window))
					return false;

			}

			mWidgets.clear();

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
			return installToWidget(mUI.gui().getWidgetByName(mWidgetName));
		}

		void Handler::finalize()
		{			
		}

		bool Handler::init(GUI::Widget* window)
		{
			return installToWidget(window);				
		}

		void Handler::injectMouseMove(Input::MouseEventArgs& evt)
		{
			onMouseMove(evt);
		}

		void Handler::injectMouseDown(Input::MouseEventArgs& evt)
		{
			onMouseDown(evt);
		}

		void Handler::injectMouseUp(Input::MouseEventArgs& evt)
		{
			onMouseUp(evt);
		}

		bool Handler::injectKeyPress(const Input::KeyEventArgs& evt)
		{
			return onKeyPress(evt);
		}

		void Handler::onMouseMove(Input::MouseEventArgs& me)
		{
		}

		void Handler::onMouseDown(Input::MouseEventArgs& me)
		{
		}

		void Handler::onMouseUp(Input::MouseEventArgs& me)
		{
		}

		bool Handler::onKeyPress(const Input::KeyEventArgs& evt)
		{
			return false;
		}

		void Handler::registerWidget(const std::string& name, std::function<bool(GUI::Widget*)> init)
		{
			assert(!mWidget);
			mWidgets.emplace_back(name, init);
		}

		void Handler::onMouseVisibilityChanged(bool b)
		{
		}

		GUI::Widget* Handler::widget() const
		{
			return mWidget;
		}


	}
}

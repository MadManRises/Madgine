#include "ogrelib.h"
#include "guihandler.h"
//#include "Database/translationunit.h"

#include "gui/guisystem.h"
#include "ui/uimanager.h"
#include "gui/windows/window.h"


namespace Engine
{
	namespace UI
	{
		GuiHandlerBase::GuiHandlerBase(const std::string& windowName, WindowType type, const std::string& layoutFile,
		                               const std::string& parentName) :
			Handler(windowName),
			mLayoutFile(layoutFile),
			mParentName(parentName),
			mType(type),
			mOrder(layoutFile.empty() ? 2 : (parentName != WindowNames::rootWindow ? 1 : 0)),
			mContext(Scene::ContextMask::NoContext)
		{
		}


		bool GuiHandlerBase::init(int order)
		{
			if (mOrder == order)
				return init();
			return true;
		}

		bool GuiHandlerBase::init()
		{
			GUI::Window* window = nullptr;

			if (!mLayoutFile.empty())
			{
				window = mUI->gui()->loadLayout(mLayoutFile, mParentName);
				if (!window)
				{
					LOG_ERROR(Exceptions::guiHandlerInitializationFailed(mWindowName));
					return false;
				}
				if (mType == WindowType::ROOT_WINDOW)
				{
					window->hide();
				}
			}
			if (window)
				return Handler::init(window);
			return Handler::init();
		}

		void GuiHandlerBase::finalize(int order)
		{
			if (mOrder == order)
				finalize();
		}

		void GuiHandlerBase::finalize()
		{
			Handler::finalize();
			if (mWindow && !mLayoutFile.empty())
			{
				mWindow->destroy();
				mWindow = nullptr;
			}
		}

		void GuiHandlerBase::open()
		{
			if (getState() == ObjectState::CONSTRUCTED)
			{
				LOG_ERROR("Failed to open unitialized GuiHandler!");
				return;
			}

			if (isOpen()) return;

			switch (mType)
			{
			case WindowType::MODAL_OVERLAY:
				mUI->openModalWindow(this);
				break;
			case WindowType::NONMODAL_OVERLAY:
				mUI->openWindow(this);
				break;
			case WindowType::ROOT_WINDOW:
				mUI->swapCurrentRoot(this);
				break;
			}
		}

		void GuiHandlerBase::close()
		{
			switch (mType)
			{
			case WindowType::MODAL_OVERLAY:
				mUI->closeModalWindow(this);
				break;
			case WindowType::NONMODAL_OVERLAY:
				mUI->closeWindow(this);
				break;
			case WindowType::ROOT_WINDOW:
				throw 0;
			}
		}

		bool GuiHandlerBase::isOpen() const
		{
			return mWindow->isVisible();
		}


		bool GuiHandlerBase::isRootWindow() const
		{
			return mType == WindowType::ROOT_WINDOW;
		}

		Scene::ContextMask GuiHandlerBase::context() const
		{
			return mContext;
		}

		void GuiHandlerBase::setInitialisationOrder(int order)
		{
			mOrder = order;
		}

		void GuiHandlerBase::setContext(Scene::ContextMask context)
		{
			mContext = context;
		}
	} // namespace GuiHandler
} // namespace Cegui

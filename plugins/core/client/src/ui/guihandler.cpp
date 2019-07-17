#include "../clientlib.h"
#include "guihandler.h"
//#include "Database/translationunit.h"

#include "../gui/guisystem.h"
#include "../ui/uimanager.h"
#include "../gui/widgets/widget.h"

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"

namespace Engine
{
	namespace UI
	{
		GuiHandlerBase::GuiHandlerBase(UIManager &ui, const std::string& windowName, WindowType type) :
			Handler(ui, windowName),
			mType(type),
			mContext(Scene::ContextMask::NoContext)
		{
		}

		void GuiHandlerBase::open()
		{
			if (getState() != ObjectState::INITIALIZED)
			{
				LOG_ERROR("Failed to open unitialized GuiHandler!");
				return;
			}

			if (isOpen()) return;

			/*switch (mType)
			{
			case WindowType::MODAL_OVERLAY:
				mUI.openModalWindow(this);
				break;
			case WindowType::NONMODAL_OVERLAY:
				mUI.openWindow(this);
				break;
			case WindowType::ROOT_WINDOW:
				mUI.swapCurrentRoot(this);
				break;
			}*/
		}

		void GuiHandlerBase::close()
		{
			/*switch (mType)
			{
			case WindowType::MODAL_OVERLAY:
				mUI.closeModalWindow(this);
				break;
			case WindowType::NONMODAL_OVERLAY:
				mUI.closeWindow(this);
				break;
			case WindowType::ROOT_WINDOW:
				throw 0;
			}*/
		}

		bool GuiHandlerBase::isOpen() const
		{
			return mWidget->isVisible();
		}


		bool GuiHandlerBase::isRootWindow() const
		{
			return mType == WindowType::ROOT_WINDOW;
		}

		Scene::ContextMask GuiHandlerBase::context() const
		{
			return mContext;
		}

		GuiHandlerBase& GuiHandlerBase::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		void GuiHandlerBase::setContext(Scene::ContextMask context)
		{
			mContext = context;
		}
	} // namespace GuiHandler
} // namespace Cegui

DEFINE_UNIQUE_COMPONENT(Engine::UI, GuiHandler)

METATABLE_BEGIN(Engine::UI::GuiHandlerBase)
METATABLE_END(Engine::UI::GuiHandlerBase)

RegisterType(Engine::UI::GuiHandlerBase);
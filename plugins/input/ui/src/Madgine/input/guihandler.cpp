#include "../uilib.h"
#include "guihandler.h"

#include "Madgine/widgets/widget.h"
#include "Madgine/widgets/widgetmanager.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentregistry.h"

DEFINE_UNIQUE_COMPONENT(Engine::Input, GuiHandler)

METATABLE_BEGIN_BASE(Engine::Input::GuiHandlerBase, Engine::Input::Handler)
METATABLE_END(Engine::Input::GuiHandlerBase)

namespace Engine {
namespace Input {
    GuiHandlerBase::GuiHandlerBase(UIManager &ui, WindowType type, std::string_view widgetName)
        : Handler(ui, widgetName)
        , mType(type)
    {
    }

    void GuiHandlerBase::open()
    {
        if (!mWidget)
            return;

        auto state = this->state();
        if (!state.is_ready() || !state) {
            LOG_ERROR("Failed to open unitialized GuiHandler!");
            return;
        }

        if (isOpen())
            return;

        switch (mType) {
        case WindowType::MODAL_OVERLAY:
            mWidget->manager().openModalWidget(mWidget);
            break;
        case WindowType::NONMODAL_OVERLAY:
            mWidget->manager().openWidget(mWidget);
            break;
        case WindowType::ROOT_WINDOW:
            mWidget->manager().swapCurrentRoot(mWidget);
            break;
        }
    }

    void GuiHandlerBase::close()
    {
        switch (mType) {
        case WindowType::MODAL_OVERLAY:
            mWidget->manager().closeModalWidget(mWidget);
            break;
        case WindowType::NONMODAL_OVERLAY:
            mWidget->manager().closeWidget(mWidget);
            break;
        case WindowType::ROOT_WINDOW:
            std::terminate();
        }
    }

    bool GuiHandlerBase::isOpen() const
    {
        return mWidget->mVisible;
    }

    bool GuiHandlerBase::isRootWindow() const
    {
        return mType == WindowType::ROOT_WINDOW;
    }

} // namespace GuiHandler
} // namespace Cegui

#include "../uilib.h"
#include "guihandler.h"

#include "Madgine/widgets/widget.h"
#include "Madgine/widgets/widgetmanager.h"
#include "uimanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

DEFINE_UNIQUE_COMPONENT(Engine::UI, GuiHandler)

METATABLE_BEGIN_BASE(Engine::UI::GuiHandlerBase, Engine::UI::Handler)
METATABLE_END(Engine::UI::GuiHandlerBase)



namespace Engine {
namespace UI {
    GuiHandlerBase::GuiHandlerBase(UIManager &ui, WindowType type)
        : Handler(ui)
        , mType(type)
        , mContext(Threading::ContextMask::NoContext)
    {
    }

    void GuiHandlerBase::open()
    {
        if (!mWidget)
            return;

        if (getState() != ObjectState::INITIALIZED) {
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

    Threading::ContextMask GuiHandlerBase::context() const
    {
        return mContext;
    }

    GuiHandlerBase &GuiHandlerBase::getSelf(bool init)
    {
        if (init) {
            checkDependency();
        }
        return *this;
    }

    void GuiHandlerBase::setContext(Threading::ContextMask context)
    {
        mContext = context;
    }

} // namespace GuiHandler
} // namespace Cegui

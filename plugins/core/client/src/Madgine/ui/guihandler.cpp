#include "../clientlib.h"
#include "guihandler.h"
//#include "Database/translationunit.h"

#include "../gui/guisystem.h"
#include "../gui/widgets/widget.h"
#include "../ui/uimanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

DEFINE_UNIQUE_COMPONENT(Engine::UI, GuiHandler)

METATABLE_BEGIN_BASE(Engine::UI::GuiHandlerBase, Engine::UI::Handler)
METATABLE_END(Engine::UI::GuiHandlerBase)

RegisterType(Engine::UI::GuiHandlerBase);

namespace Engine {
namespace UI {
    GuiHandlerBase::GuiHandlerBase(UIManager &ui, WindowType type)
        : Handler(ui)
        , mType(type)
        , mContext(Scene::ContextMask::NoContext)
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
            ui().window().openModalWidget(mWidget);
            break;
        case WindowType::NONMODAL_OVERLAY:
            ui().window().openWidget(mWidget);
            break;
        case WindowType::ROOT_WINDOW:
            ui().window().swapCurrentRoot(mWidget);
            break;
        }
    }

    void GuiHandlerBase::close()
    {
        switch (mType) {
        case WindowType::MODAL_OVERLAY:
            ui().window().closeModalWidget(mWidget);
            break;
        case WindowType::NONMODAL_OVERLAY:
            ui().window().closeWidget(mWidget);
            break;
        case WindowType::ROOT_WINDOW:
            throw 0;
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

    Scene::ContextMask GuiHandlerBase::context() const
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

    void GuiHandlerBase::setContext(Scene::ContextMask context)
    {
        mContext = context;
    }
} // namespace GuiHandler
} // namespace Cegui

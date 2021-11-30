#include "../uilib.h"
#include "handler.h"
#include "Madgine/widgets/widget.h"
#include "uimanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::UI::Handler)
PROPERTY(Widget, widget, setWidget)
METATABLE_END(Engine::UI::Handler)

SERIALIZETABLE_BEGIN(Engine::UI::Handler)
ENCAPSULATED_POINTER(mWidget, widget, setWidget)
SERIALIZETABLE_END(Engine::UI::Handler)

namespace Engine {
namespace UI {
    Handler::Handler(UIManager &ui)
        : mUI(ui)
    {
    }

    GuiHandlerBase &Handler::getGuiHandler(size_t i)
    {
        return mUI.getGuiHandler(i);
    }

    GameHandlerBase &Handler::getGameHandler(size_t i)
    {
        return mUI.getGameHandler(i);
    }

    void Handler::sizeChanged()
    {
    }

    bool Handler::init()
    {
        return true;
    }

    void Handler::finalize()
    {
        mWidget = nullptr;
    }

    void Handler::setWidget(Widgets::WidgetBase *widget)
    {
        if (mWidget != widget) {
            if (mWidget) {
                mConStore.clear();
            }
            mWidget = widget;

            if (mWidget) {
                mWidget->pointerMoveEvent().connect(&Handler::injectPointerMove, this, &mConStore);
                mWidget->pointerDownEvent().connect(&Handler::injectPointerDown, this, &mConStore);
                mWidget->pointerUpEvent().connect(&Handler::injectPointerUp, this, &mConStore);
                mWidget->axisEvent().connect(&Handler::injectAxisEvent, this, &mConStore);
            }
        }
    }

    void Handler::injectPointerMove(const Input::PointerEventArgs &evt)
    {
        onPointerMove(evt);
    }

    void Handler::injectPointerDown(const Input::PointerEventArgs &evt)
    {
        onPointerDown(evt);
    }

    void Handler::injectPointerUp(const Input::PointerEventArgs &evt)
    {
        onPointerUp(evt);
    }

    bool Handler::injectKeyPress(const Input::KeyEventArgs &evt)
    {
        return onKeyPress(evt);
    }

    void Handler::injectAxisEvent(const Input::AxisEventArgs &evt)
    {
        onAxisEvent(evt);
    }

    void Handler::onPointerMove(const Input::PointerEventArgs &me)
    {
    }

    void Handler::onPointerDown(const Input::PointerEventArgs &me)
    {
    }

    void Handler::onPointerUp(const Input::PointerEventArgs &me)
    {
    }

    bool Handler::onKeyPress(const Input::KeyEventArgs &evt)
    {
        return false;
    }

    void Handler::onAxisEvent(const Input::AxisEventArgs& evt)
    {
    }

    void Handler::onMouseVisibilityChanged(bool b)
    {
    }

    Widgets::WidgetBase *Handler::widget() const
    {
        return mWidget;
    }
        
    Threading::TaskQueue *Handler::taskQueue() const
    {
        return mUI.taskQueue();
    }

}
}

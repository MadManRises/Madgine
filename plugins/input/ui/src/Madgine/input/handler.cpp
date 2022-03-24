#include "../uilib.h"
#include "handler.h"
#include "Madgine/widgets/widget.h"
#include "uimanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Input::Handler)
PROPERTY(Widget, widget, setWidget)
METATABLE_END(Engine::Input::Handler)

SERIALIZETABLE_BEGIN(Engine::Input::Handler)
ENCAPSULATED_POINTER(mWidget, widget, setWidget)
SERIALIZETABLE_END(Engine::Input::Handler)

namespace Engine {
namespace Input {
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

    Threading::Task<bool> Handler::init()
    {
        co_return true;
    }

    Threading::Task<void> Handler::finalize()
    {
        mWidget = nullptr;
        co_return;
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

    void Handler::injectPointerMove(const PointerEventArgs &evt)
    {
        onPointerMove(evt);
    }

    void Handler::injectPointerDown(const PointerEventArgs &evt)
    {
        onPointerDown(evt);
    }

    void Handler::injectPointerUp(const PointerEventArgs &evt)
    {
        onPointerUp(evt);
    }

    bool Handler::injectKeyPress(const KeyEventArgs &evt)
    {
        return onKeyPress(evt);
    }

    void Handler::injectAxisEvent(const AxisEventArgs &evt)
    {
        onAxisEvent(evt);
    }

    void Handler::onPointerMove(const PointerEventArgs &me)
    {
    }

    void Handler::onPointerDown(const PointerEventArgs &me)
    {
    }

    void Handler::onPointerUp(const PointerEventArgs &me)
    {
    }

    bool Handler::onKeyPress(const KeyEventArgs &evt)
    {
        return false;
    }

    void Handler::onAxisEvent(const AxisEventArgs &evt)
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

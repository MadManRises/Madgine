#include "../uilib.h"
#include "handler.h"
#include "Madgine/widgets/widget.h"
#include "Madgine/widgets/widgetmanager.h"
#include "Madgine/window/mainwindow.h"
#include "uimanager.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Input::Handler)
PROPERTY(Widget, widget, setWidget)
METATABLE_END(Engine::Input::Handler)

namespace Engine {
namespace Input {
    Handler::Handler(UIManager &ui, std::string_view widgetName)
        : mWidgetName(widgetName)
        , mUI(ui)
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
                mWidget->pointerClickEvent().connect(&Handler::injectPointerClick, this, &mConStore);
                mWidget->dragBeginEvent().connect(&Handler::injectDragBegin, this, &mConStore);
                mWidget->dragMoveEvent().connect(&Handler::injectDragMove, this, &mConStore);
                mWidget->dragEndEvent().connect(&Handler::injectDragEnd, this, &mConStore);
                mWidget->axisEvent().connect(&Handler::injectAxisEvent, this, &mConStore);
                mWidget->keyEvent().connect(&Handler::injectKeyPress, this, &mConStore);
            }
        }
    }

    void Handler::injectPointerMove(const PointerEventArgs &evt)
    {
        onPointerMove(evt);
    }

    void Handler::injectPointerClick(const PointerEventArgs &evt)
    {
        onPointerClick(evt);
    }

    void Handler::injectDragBegin(const PointerEventArgs &evt)
    {
        onDragBegin(evt);
    }

    void Handler::injectDragMove(const PointerEventArgs &evt)
    {
        onDragMove(evt);
    }

    void Handler::injectDragEnd(const PointerEventArgs &evt)
    {
        onDragEnd(evt);
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

    void Handler::onPointerClick(const PointerEventArgs &me)
    {
    }

    void Handler::onDragBegin(const PointerEventArgs &me)
    {
    }

    void Handler::onDragMove(const PointerEventArgs &me)
    {
    }

    void Handler::onDragEnd(const PointerEventArgs &me)
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

    void Handler::onUpdate()
    {
        setWidget(mUI.window().getWindowComponent<Widgets::WidgetManager>().getWidget(mWidgetName));
    }

    Widgets::WidgetBase *Handler::widget() const
    {
        return mWidget;
    }

    Threading::TaskQueue *Handler::viewTaskQueue() const
    {
        return mUI.viewTaskQueue();
    }

}
}

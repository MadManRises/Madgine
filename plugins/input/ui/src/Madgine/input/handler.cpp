#include "../uilib.h"
#include "handler.h"
#include "Madgine/widgets/widget.h"
#include "Madgine/widgets/widgetmanager.h"
#include "Madgine/window/mainwindow.h"
#include "uimanager.h"

#include "Meta/keyvalue/metatable_impl.h"

DEFINE_UNIQUE_COMPONENT(Engine::Input, Handler)

METATABLE_BEGIN(Engine::Input::HandlerBase)
PROPERTY(Widget, widget, setWidget)
METATABLE_END(Engine::Input::HandlerBase)

namespace Engine {
namespace Input {
    HandlerBase::HandlerBase(UIManager &ui, std::string_view widgetName, WidgetType type)
        : mWidgetName(widgetName)
        , mUI(ui)
        , mType(type)
    {
    }

    HandlerBase &HandlerBase::getHandler(size_t i)
    {
        return mUI.getHandler(i);
    }

    void HandlerBase::sizeChanged()
    {
    }

    Threading::Task<bool> HandlerBase::init()
    {
        co_return true;
    }

    Threading::Task<void> HandlerBase::finalize()
    {
        mWidget = nullptr;
        co_return;
    }

    void HandlerBase::setWidget(Widgets::WidgetBase *widget)
    {
        if (mWidget != widget) {
            if (mWidget) {
                mConStore.clear();
            }
            mWidget = widget;

            if (mWidget) {
                mWidget->pointerMoveEvent().connect(&HandlerBase::injectPointerMove, this, &mConStore);
                mWidget->pointerClickEvent().connect(&HandlerBase::injectPointerClick, this, &mConStore);
                mWidget->dragBeginEvent().connect(&HandlerBase::injectDragBegin, this, &mConStore);
                mWidget->dragMoveEvent().connect(&HandlerBase::injectDragMove, this, &mConStore);
                mWidget->dragEndEvent().connect(&HandlerBase::injectDragEnd, this, &mConStore);
                mWidget->axisEvent().connect(&HandlerBase::injectAxisEvent, this, &mConStore);
                mWidget->keyEvent().connect(&HandlerBase::injectKeyPress, this, &mConStore);
            }
        }
    }

    void HandlerBase::injectPointerMove(const PointerEventArgs &evt)
    {
        onPointerMove(evt);
    }

    void HandlerBase::injectPointerClick(const PointerEventArgs &evt)
    {
        onPointerClick(evt);
    }

    void HandlerBase::injectDragBegin(const PointerEventArgs &evt)
    {
        onDragBegin(evt);
    }

    void HandlerBase::injectDragMove(const PointerEventArgs &evt)
    {
        onDragMove(evt);
    }

    void HandlerBase::injectDragEnd(const PointerEventArgs &evt)
    {
        onDragEnd(evt);
    }

    bool HandlerBase::injectKeyPress(const KeyEventArgs &evt)
    {
        return onKeyPress(evt);
    }

    void HandlerBase::injectAxisEvent(const AxisEventArgs &evt)
    {
        onAxisEvent(evt);
    }

    void HandlerBase::onPointerMove(const PointerEventArgs &me)
    {
    }

    void HandlerBase::onPointerClick(const PointerEventArgs &me)
    {
    }

    void HandlerBase::onDragBegin(const PointerEventArgs &me)
    {
    }

    void HandlerBase::onDragMove(const PointerEventArgs &me)
    {
    }

    void HandlerBase::onDragEnd(const PointerEventArgs &me)
    {
    }

    bool HandlerBase::onKeyPress(const KeyEventArgs &evt)
    {
        return false;
    }

    void HandlerBase::onAxisEvent(const AxisEventArgs &evt)
    {
    }

    void HandlerBase::onMouseVisibilityChanged(bool b)
    {
    }

    void HandlerBase::onUpdate()
    {
        setWidget(mUI.window().getWindowComponent<Widgets::WidgetManager>().getWidget(mWidgetName));
    }

    Widgets::WidgetBase *HandlerBase::widget() const
    {
        return mWidget;
    }

    Threading::TaskQueue *HandlerBase::viewTaskQueue() const
    {
        return mUI.viewTaskQueue();
    }

    void HandlerBase::updateRender(std::chrono::microseconds timeSinceLastFrame)
    {
    }

    void HandlerBase::fixedUpdateRender(std::chrono::microseconds timeStep)
    {
    }

    void HandlerBase::updateApp(std::chrono::microseconds timeSinceLastFrame)
    {
    }

    void HandlerBase::open()
    {
        assert(mType != WidgetType::DEFAULT_WIDGET);

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
        case WidgetType::MODAL_OVERLAY:
            mWidget->manager().openModalWidget(mWidget);
            break;
        case WidgetType::NONMODAL_OVERLAY:
            mWidget->manager().openWidget(mWidget);
            break;
        case WidgetType::ROOT_WIDGET:
            mWidget->manager().swapCurrentRoot(mWidget);
            break;
        }
    }

    void HandlerBase::close()
    {
        assert(mType != WidgetType::DEFAULT_WIDGET);

        switch (mType) {
        case WidgetType::MODAL_OVERLAY:
            mWidget->manager().closeModalWidget(mWidget);
            break;
        case WidgetType::NONMODAL_OVERLAY:
            mWidget->manager().closeWidget(mWidget);
            break;
        case WidgetType::ROOT_WIDGET:
            std::terminate();
        }
    }

    bool HandlerBase::isOpen() const
    {
        return mWidget->mVisible;
    }

    bool HandlerBase::isRootWindow() const
    {
        return mType == WidgetType::ROOT_WIDGET;
    }

}
}
